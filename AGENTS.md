# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this
repository.

## What this is

Bemanitools 5 (BT5) is a collection of tools to run Konami Bemani arcade rhythm games (IIDX, DDR,
SOUND VOLTEX, jubeat, pop'n music, BeatStream) on non-arcade hardware. It works by injecting "hook"
DLLs into the game process that **emulate the protocols of the real arcade IO PCBs** (and patch
compatibility issues) rather than patching game code. This produces Win32 DLLs/EXEs cross-compiled
for Windows from a Linux/macOS host via mingw-w64.

## Build & test

The build is a hand-written recursive GNU Make system. All output goes to `build/`; the final
package is `build/bemanitools.zip`.

```sh
make                 # default build of everything
make release         # clean build + code-format + doc-format + run-tests (adds -Werror)
make clean
make doc-format      # mdformat --wrap 100 over all *.md
make run-tests       # builds tests.zip then runs unit tests under wine (requires wine)
make build-docker    # build in a container (no local toolchain needed); output in build/docker/
V=@                  # prepend to silence command echo, e.g. `make V=@`
```

- **Do not use `make -j`** — parallel make is known-broken here and produces flaky dependency
  errors. Run `make clean` first if you hit one.
- This repo uses **Nix + direnv** (`.envrc` runs `use flake`). The toolchain (`i686-w64-mingw32-`,
  `x86_64-w64-mingw32-`, clang-format, wine) comes from the flake; `direnv allow` to load it.
- Tests are Windows `.exe` unit tests listed in `GNUmakefile`'s `tests.zip` target, run via wine
  (`run-tests-wine.sh`). To run a single test, build it directly and run under wine, e.g.
  `make build/bin/indep-32/util-net-test.exe && wine build/bin/indep-32/util-net-test.exe`.

## Build system structure (important to understand before adding code)

Every module lives in `src/main/<module>/` (or `src/test/<module>/`) with its own `Module.mk`, and
**every Module.mk must be `include`d in the root `Module.mk`** (the long list ~line 412+). A module
declares itself by appending its name to one of these accumulator variables, which controls how it's
built and which output dirs it lands in:

- `dlls` / `exes` / `libs` — AVS-independent; built once into `build/bin/indep-32` and `indep-64`.
- `avsdlls` / `avsexes` — link against AVS (Konami's runtime); built **once per AVS version** into
  `build/bin/avs2_<ver>-<bits>/` (e.g. `avs2_1700-64`). Use these when the module calls AVS APIs.
- `testdlls` / `testexes` — unit-test artifacts.
- `imps` — import libraries generated from `.def` files in `src/imports/`.

Per-module variables follow the pattern `<var>_<module>`: `src_<m>` (`.c`), `cppsrc_<m>` (`.cpp`),
`rc_<m>` (`.rc`), `libs_<m>` (static libs to link, AVS-independent), `deplibs_<m>` (AVS-version
libs), `cppflags_<m>`, `cflags_<m>`, `ldflags_<m>`. A `dll` must have a `<module>.def` exports file
in its source dir. See `src/main/spice-sdvxio/Module.mk` (an exe) and `src/main/sdvxio/Module.mk` (a
dll) for minimal examples.

**AVS version encoding:** `AVS_VERSION = minor*100 + patch` (e.g. 2.16.7 → `1607`, `1700` = 2.17.0,
games with no AVS → `0`). `avsvers_32`/`avsvers_64` in the root Module.mk list which versions get
built; the big comment block there maps each version to the games that use it. Distribution `.zip`
targets at the bottom of the root Module.mk pin each game release to a specific `avs2_<ver>-<bits>`
build dir.

## Code architecture

The core idea (see `doc/architecture.md`, `doc/api.md`):

1. **Injection + hooking.** `inject.exe` / `launcher.exe` load a hook DLL into the game before its
   code runs. Hooks use **IAT detouring** (`hook` = vendored capnhook, `hooklib` = BT-specific
   helpers) to intercept Win32 calls. An **IRP (I/O Request Packet) abstraction** lets handlers be
   chained for file I/O (`iohook`) and Direct3D 9 (`d3d9`) — each handler can forward to the next or
   emulate the call entirely. Hardware emulators (`ezusb-emu`, `ezusb2-emu`, `acioemu`, `p3ioemu`,
   `p4ioemu`, `bio2emu`) hook into these chains.

1. **Game hook DLLs** (`<game>hook<N>`): the per-game/per-version glue. Each hook's main file mostly
   wires together the emulator and util modules it needs, plus a config layer. Multiple numbered
   variants exist because hardware/AVS/D3D differs across releases (e.g. `iidxhook1`–`iidxhook9`,
   `ddrhook1`/`ddrhook2`, `sdvxhook`/`sdvxhook2`). `<game>hook-util` modules hold logic shared
   across a game's hook variants.

1. **BT5 IO API** — the public SDK. Headers live in `src/main/bemanitools/` (`iidxio.h`, `sdvxio.h`,
   `ddrio.h`, `eamio.h`, `jbio.h`, etc.). This abstracts game IO (buttons, turntables, lights) from
   eamuse IO (card readers, keypad → `eamio`). A hook calls these interfaces; a swappable backend
   DLL implements them — e.g. `sdvxio.dll` (default keyboard/joystick), `sdvxio-bio2.dll`,
   `sdvxio-kfca.dll` drive different real/virtual hardware. To support new hardware you implement
   one of these interfaces. `vigem-*` backends expose IO as virtual Xbox controllers; `*-async` wrap
   another backend on a dedicated IO thread.

The hardware/protocol stacks layer as: low-level driver (`aciodrv`, `bio2drv`, `p3iodrv`, `ezusb`) →
emulator (`acioemu`, `bio2emu`, `ezusb-emu`) or real-hardware IO backend → BT5 IO API → game hook.

## Focus of this fork

This is a personal fork. Active work centers on two SDVX modules that both let a real `sdvxio`
backend (typically `sdvxio-bio2.dll` driving a BIO2/BI2X board, renamed to `sdvxio.dll`) drive
**SOUND VOLTEX Valkyrie mode** — including cabinet LEDs — without game patches. They are two
different integration strategies for the same goal; docs live in `doc/sdvxhook/spice-sdvxio.md` and
`doc/sdvxhook/sdvxhook2-valk.md`.

### spice-sdvxio (`src/main/spice-sdvxio/`, an `exe`)

A standalone bridge process — **not** an injected hook — between the BT5 `sdvxio` API and a running
**spice2x** instance. It plays the role KFChicken did for KFCA, but for any `sdvxio`-supported
board. spice2x runs the game with its own emulation (`-api`/`-apipass`/`-reader`, and crucially
*without* `-exec soundvoltex.dll`); this exe pumps IO to/from it.

Architecture:

- `main.c` — owns the poll loop: `sdvx_io_read_input()` → read GPIO/analogs → push buttons/analogs/
  coins to spice and read lights back over the spice API → `sdvx_io_write_output()`. Holds the
  button/analog/light mapping tables (`g_button_maps`, `g_analog_maps`). TEST+SERVICE together
  exits.
- `spice_connection.c/.h` — raw TCP socket to the spice2x API, RC4-encrypted (`spice_rc4.c`) after a
  password handshake; `spice_connection_request()` does request/response of JSON strings.
- `spice_wrappers.c/.h` — typed wrappers over the spice JSON API (`spice_buttons_write`,
  `spice_analogs_write`, `spice_lights_read`, `spice_coin_*`, `spice_info_launcher`, ...).
- `cJSON.c/.h` — vendored JSON lib (the only place it's used).
- `responsive-analog-read.c/.h` — analog smoothing/snapping for the VOL knobs.
- `config-spice-sdvxio.c/.h` — `cconfig`-backed config (`spice.host/port/password`, amp volume,
  analog tuning, keylight toggle). Generates `spice-sdvxio.conf` on first run.

### sdvxhook2-valk (`src/main/sdvxhook2-valk/`, an `avsdll`)

An injected hook DLL (loaded via `launcher.exe -K sdvxhook2-valk.dll soundvoltex.dll`) that hooks
the **BI2X API** used by SDVX Valkyrie mode, so a BIO2 board appears as the expected BI2X cabinet
IO. The standard BT5 hook pattern: `dllmain.c`'s `my_dll_entry_init` builds a `cconfig` and wires
together sub-feature modules:

- `iob2.c/.h` — the core BI2X IO emulation hooks (`aio_iob2_hook_init`), with poll-limiter and
  force-headphones options.
- `tapeled.c/.h` — converts the game's per-LED "tape" RGB strips into single colors for boards with
  fewer channels, via a selectable algorithm (`avg`/`first`/`middle`/`last`/`off`,
  `enum tapeled_color_pick_algorithm`).
- `config-io.c/.h`, `config-leds.c/.h` — `cconfig` definitions. `config-leds` maps each logical
  cabinet light (wings, woofer, control panel, V-unit/generator, speakers) to a PWM output
  pin/channel; `-1` means unassigned.
- Reuses shared hooks: `d3d9exhook`, `camhook`, `eamio`/`eamio-icca`, `hooklib` (memfile/rs232/acp),
  `sdvxhook-util` (acio/nvapi/power). `io.disable_*` config flags turn the
  file/power/nvapi/BIO2-emu/ card-reader hooks off.

Note: because Valkyrie's BI2X no longer initializes the speaker amps, the **`sdvxio-bio2` backend**
(its own `bio2.initialize_amps`/`bio2.amp_volume` config) is what brings the amps up — relevant when
debugging "no audio" or volume issues, and mirrored by `spice-sdvxio`'s `sdvx_io_set_amp_volume`
calls in `main.c`.

## Code style

Do **not** run `clang-format` on the codebase, as it isn't used anymore.

- **No trailing comments** — put the comment on its own line above the code.
- **Blank line before and after every control block** (`if`/`for`/`while`/`switch`/...).
- **Include guards** named `<NAMESPACE>_<MODULE>_H` (e.g. `BSTHOOK_ACIO_H`).
- **Includes:** all at top; `<...>` system then `"..."` project; project includes use the full path
  from `src/main`/`src/test` (e.g. `"util/log.h"`, never `"log.h"`); group by namespace block, blank
  line between groups, lex-sorted within a group.
- **Naming:** folders/files lower-case-with-dashes; functions/structs/enums/globals/statics
  snake_case and namespaced as `<namespace>_<module>_<name>`; local vars not namespaced; enum values
  fully namespaced and SCREAMING_CASE.
- **Doc comments** (`/** ... */`) on every exported enum/struct/function and on each module header.
- Reverse-engineering notes and protocol docs belong in `doc/dev/`.

## Contributing

Upstream is on GitLab and uses issue/MR templates (`CONTRIBUTING.md`). The default integration
branch is `master`. Documentation changes are treated as first-class contributions.
