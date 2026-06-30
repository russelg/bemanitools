This library uses the sdvxio API of BT5 to communicate with spice2x via the Spice SDK. It bridges any
sdvxio-supported device board to any version of SDVX supported by spice2x, without requiring any game
patches.

The main use case is using sdvxio-bio2 to get Valkyrie-mode support including LED support. The setup
instructions below are for this use case.

Tested against spice2x **2025-06-10** (or later). If `init()` fails or required SDK functions are
absent, the DLL refuses to load and logs the reason — update spice2x if this happens.

# Setup

- Make sure that your BIO2 is flashed with a SDVX firmware (if you can boot SDVX5 with it, it's
  already the right firmware)
  - The firmware persists between boots, so as long as it's flashed to the correct firmware, you no
    longer need to do this step
- Rename `sdvxio-bio2.dll` to `sdvxio.dll`
- Load the hook DLL via spice's `-k` parameter. Example minimal `gamestart.bat`:

```batch
spice64.exe -k spice-sdvxio.dll -reader COM2
```

Explanation:

- `-k spice-sdvxio.dll` loads the hook DLL. Use `spice-sdvxio.dll` from the `indep-64` build when
  running `spice64.exe`, or from `indep-32` when running `spice32.exe`.
- `-reader COM2` is the port that your card reader is connected to. This will be COM1 for Valk
  motherboards, and COM2 otherwise. Should match whatever you have it set to in device manager.

You should **not** pass `-exec soundvoltex.dll` — the required emulation must remain enabled.

The `-api` / `-apipass` flags are **not** required with the SDK DLL.

# Configuration

On first run with `--config spice-sdvxio.conf` the default configuration file is created and the
program exits, prompting you to restart. Pass the path to spice2x:

```batch
spice64.exe -k spice-sdvxio.dll -reader COM2 --config spice-sdvxio.conf
```

Edit `spice-sdvxio.conf` as needed for amp volume, key lighting, analog tuning, and LED pin
mappings (see below). After first run you can omit `--config` to use built-in defaults, or keep it
to load custom settings every time.

# LED Mapping

`spice-sdvxio.conf` supports the same LED mapping as sdvxhook2-valk. Here's a sample of the LED
mapping options available in `spice-sdvxio.conf`.

```ini
# LED Mapping.
# Set to the desired PWM output pin/channel:
#  0: Wing Left Up R
#  1: Wing Left Up G
#  2: Wing Left Up B
#  3: Wing Right Up R
#  4: Wing Right Up G
#  5: Wing Right Up B
#  6: Wing Left Low R
#  7: Wing Left Low G
#  8: Wing Left Low B
#  9: Wing Right Low R
# 10: Wing Right Low G
# 11: Wing Right Low B
# 12: Woofer R
# 13: Woofer G
# 14: Woofer B
# 15: Controller R
# 16: Controller G
# 17: Controller B
# 18: Generator R
# 19: Generator G
# -1 to not assign a pin. This is the default so you can omit those lines.
# You can bind multiple pins to an output e.g:
#   leds.left_wing_avg_r_pin=0 6
# Using a BIO2, Wing L/R cannot be set independently, only Upper and Lower.
# Can have weird appearance (dim) if assigning to both L/R, if so try only assigning one side.
leds.left_wing_avg_r_pin=0
leds.left_wing_avg_g_pin=1
leds.left_wing_avg_b_pin=2
leds.right_wing_avg_r_pin=-1
leds.right_wing_avg_g_pin=-1
leds.right_wing_avg_b_pin=-1
leds.woofer_avg_r_pin=12
leds.woofer_avg_g_pin=13
leds.woofer_avg_b_pin=14
leds.control_panel_avg_r_pin=15
leds.control_panel_avg_g_pin=16
leds.control_panel_avg_b_pin=17
leds.v_unit_avg_r_pin=6
leds.v_unit_avg_g_pin=7
leds.v_unit_avg_b_pin=8
leds.title_avg_r_pin=-1
leds.title_avg_g_pin=-1
leds.title_avg_b_pin=-1
leds.upper_left_speaker_avg_r_pin=-1
leds.upper_left_speaker_avg_g_pin=-1
leds.upper_left_speaker_avg_b_pin=-1
leds.upper_right_speaker_avg_r_pin=-1
leds.upper_right_speaker_avg_g_pin=-1
leds.upper_right_speaker_avg_b_pin=-1
leds.lower_left_speaker_avg_r_pin=-1
leds.lower_left_speaker_avg_g_pin=-1
leds.lower_left_speaker_avg_b_pin=-1
leds.lower_right_speaker_avg_r_pin=-1
leds.lower_right_speaker_avg_g_pin=-1
leds.lower_right_speaker_avg_b_pin=-1
leds.ic_card_reader_r_pin=-1
leds.ic_card_reader_g_pin=-1
leds.ic_card_reader_b_pin=-1
```

# SDK caveats

- There is no coin API in the SDK. Coins are delivered by pulsing the CoinMech button (or the
  Service button if `sdvxio.service_coin=true`). The coin blocker state is not synchronised with
  spice (it is unlocked once at startup).

# Notes

- sdvxio-bio2 implements BIO2 device autodetection
  - if this fails please make a bug report, and set the port manually in the config
- You can change the port and baudrate by editing the sdvxio-bio2.conf that should be created by default
- Please connect any additional acio devices to their own port (ex: SDVX5 expects an ICCA by itself on COM2, COM1 for valk)
