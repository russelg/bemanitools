# BIO2 BI2A protocol (SDVX layout)

Reverse engineered from the SDK version of `libacio.dll`, which is the original client
implementation for BIO2 boards running SDVX firmware (image base `0x180000000`, all addresses below
refer to that build). The same board also implements the BI2X protocol used by Valkyrie mode, see
`libaio-iob2_video.dll`.

BIO2 nodes used by games that talk to them through libacio (SDVX, DDR, DANCERUSH, ...) are polled
with the KFCA poll command (`0x0113`, `BIO2_BI2A_CMD_ACIO_POLL_WD`). Unlike the KFCA protocol, the
payloads are always 40 bytes out and 16 bytes in. The payload layout differs per game (compare
`bio2/bi2a-sdvx.h` and `bio2/bi2a-ddr.h`), this note documents the SDVX one.

## Poll request (game to IO board, 40 bytes)

Built by `sub_18000B8E0` (fills the 40 byte buffer that `sub_18000C000` sends through
`sub_18000BAC0`), maps to `struct bi2a_sdvx_state_out` in `src/main/bio2/bi2a-sdvx.h`.

- Byte 0: coin counter, only the lower 6 bits are used. See below.
- Byte 1: unknown, always zero in libacio.
- Byte 2: coin blocker, one bit per coin slot, `0x80` = slot 1 and `0x40` = slot 2. Bit set = coin
  chute open.
- Bytes 3 - 37: LED brightness values, see the LED section below.
- Bytes 38 - 39: unknown, always zero in libacio.

## Poll response (IO board to game, 16 bytes)

Maps to `struct bi2a_sdvx_state_in` in `src/main/bio2/bi2a-sdvx.h`.

- Bytes 0 - 7: four 16 bit words, coin/service/test flags in the lowest 4 bits, 10 bit analog value
  above that.
- Byte 8: coin counter, only the lower 6 bits are used. See below.
- Byte 9: buttons, FX L, D, C, B, A, start, recorder, headphones.
- Byte 10: buttons, FX R, EX 1, EX 2.
- Bytes 11 - 15: unknown, unused.

## Coin counter

The cabinet's physical (mechanical) coin counter is driven by the IO board: the board pulses it
whenever the 6 bit coin counter value of the poll request (`c_counter`) increments. There is no
register to set an absolute counter value, the board only has to see the counter advance.

- The board counts the coins it detected itself and reports that count as a 6 bit wrapping counter
  in the poll response (byte 8). Every coin that passes the coin mech increments that value by one.
- libacio reads the counter on every poll (byte 8, masked with `0x3f`) and accumulates the
  difference to the previously read value. The result of that accumulation is what gets sent back to
  the board as part of the poll request (byte 0).
- While the coin counter is locked (`ac_io_bi2a_lock_coincounter`, `0x18000b700`) the increments are
  not added to the value that is sent to the board, see the accumulator at
  `byte_1800b2864[24 * slot]` in `sub_18000b9d0`.
- The accumulation itself happens in a full byte (`sub_18000b8e0`), only the value that is sent to
  the board is masked with `0x3f`.
- There is no `ac_io_bi2a_add_coin`, the counter is derived from the board's coin input only.

`sdvxio-bio2` derives the counter the same way, which is the only option for backends that are
driven by hooks which do not forward a counter value (Valkyrie mode and spice2x do not call
`ac_io_bi2a_*` at all).

## Coin blocker

`ac_io_bi2a_control_coin_blocker_open` (`0x18000b760`) sets and `..._close` (`0x18000b740`) clears
the corresponding bit in `byte_1800b6e53`, which is byte 2 of the poll request. The coin counter
lock (`ac_io_bi2a_lock_coincounter`, `..._unlock_coincounter`) is a separate flag and only affects
the counter as described above.

## LEDs

`ac_io_bi2a_control_led_bright(index, brightness)` (`0x18000b780`) writes one brightness value per
LED into a 35 byte array (`0x1800b6e30`, brightness clamped to `0x80`). That array is copied to
bytes 3 - 37 of the poll request. Index mapping for SDVX:

- 0 - 2: woofer R/G/B, index 2 unused.
- 4 - 6: control panel R/G/B.
- 8 - 14: start, BT A-D, FX L, FX R.
- 18 - 20: generator R/G/B.
- 24 - 26: pop, title left, title right.
- 28 - 30: wing upper R/G/B.
- 31 - 33: wing lower R/G/B.

The BIO2 has no separate left/right wing channels, so libacio and spice2x write the same value to
both sides. Compare with `sdvx_io_write_output` in `src/main/sdvxio-bio2/sdvxio.c`.

## BI2X equivalent

Valkyrie mode games do not use libacio, they talk to the same board through `libaio-iob2_video.dll`
using the BI2X protocol. There, the coin counter is not derived from the coin input but pushed to
the board explicitly: `AIO_IOB2_BI2X_UFC::AddCounter(index, count)` (`0x180001cd0`) adds `count` to
one of four counter bytes at object offset `10185 + index`. Those bytes are part of the outgoing
packet, right before the outputs written by `SetOutputData` (`0x180001d60`, index 1 is the coin
blocker). The game calls `AddCounter` with the difference of `Input.CoinCount` of the device status,
so both protocols carry the same information: "the cabinet counted this many more coins".
