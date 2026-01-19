# sdvxhook2-valk

This library hooks the BI2X API used by SDVX Valkyrie mode.

The main use case of this is to use `sdvxio-bio2` to get valkyrie-mode support on a Nemsys, including LED support.
The setup instructions below will be for this use case.

# Setup

You'll need the following files:

- `sdvx-hwio-x64.zip`
  - `sdvxio-bio2.dll` 
    - or `sdvxio-kfca.dll` if you want to use a KFCA board (SDVX 1-4)
  - `aciomgr.dll`
  - `eamio-icca.dll` (If you want IC card reader support) 
- `sdvx-05-to-06.zip`
  - `sdvxhook2-valk.dll`
  - `sdvxhook2-valk.conf`
  - `launcher.exe`

## Instructions  

- Move the following files to the `contents` folder:
   - `sdvxio-bio2.dll` 
   - `aciomgr.dll`
   - `eamio-icca.dll`
   - `sdvxhook2-valk.dll` 
   - `sdvxhook2-valk.conf`
- Move `launcher.exe` to `contents\modules`
- Rename `sdvxio-bio2.dll` to `sdvxio.dll`
- If you want IC card support via eamio-icca, ensure you also rename `eamio-icca.dll` to `eamio.dll`
- Edit `sdvxhook2-valk.conf` as needed. A sample config for a typical Nemsys setup is provided below.
- Ensure that your `gamestart.bat` calls `launcher.exe` in the following way:

```batch
modules\launcher.exe -H 268435456 -K sdvxhook2-valk.dll soundvoltex.dll --config sdvxhook2-valk.conf %*
```

Other common `launcher.exe` arguments:
- Service URL: `-S http://eamuse.konami.fun/`
- PCBID: `-P 00010203040506070809`
- Log file: `-Y log.txt`

## Sample config

All supported options are included in the bundled `sdvxhook2-valk.conf`.
Here's a sample `sdvxhook2-valk.conf` for a typical Nemsys setup.

```ini
# Disable BIO2 emulation and enable usage of real BIO2 hardware
# Keep this false unless you have a BI2X.
io.disable_bio2_emu=false

# This can be set to true if you have a compatible card reader (ICCC) already connected to the correct COM port
io.disable_card_reader_emu=false

# Emulated card reader serial port. Probably COM2.
# If the reader doesn't work try changing this to COM1.
io.card_reader_port=COM2

# Disables the poll limiter, warning very high CPU usage may arise
io.disable_poll_limiter=false

# Forces game to think headphones are attached
io.force_headphones=false

# Algorithm for picking the tape LED color. Change this to experiment with the lights.
# avg (Average color), first (First LED), middle (Middle LED), last (Last LED), off (Off)
io.tape_led_algorithm=middle

#
# The following hooks are disabled in this sample config as cabinets do not typically require these hooks.
#
# Disables the built in file hooks, requiring manual file creation (/dev/raw/j.dest)
io.disable_file_hooks=true

# Disables the built in power control hooks, allowing game to modify system power settings
io.disable_power_hooks=true

# Disables the built in NVAPI control hooks, allowing game to modify system monitor settings
io.disable_nvapi_hooks=true
```

The BI2X does not initialize the speaker amps, so `sdvxio-bio2.dll` is responsible for doing so.
The default volume is set to 48, which may be quiet on some setups. 

This can be changed in `sdvxio-bio2.conf`, this file will be created once you launch the game. 
Here's a sample `sdvxio-bio2.conf` for a typical Nemsys setup.

```ini
# Initialize amps on BIO2 init. Needed when running in Valkyrie mode with BI2X as it does not init amps anymore.
bio2.initialize_amps=true

# Digital amp volume (0-96) 0 is high, 96 is low.
bio2.amp_volume=48
```


# Notes

- sdvxio-bio2 implements BIO2 device autodetection
  - if this fails please make a bug report, and set the port manually in the config
- You can change the port and baudrate by editing the sdvxio-bio2.conf that should be created by default
- Please connect any additional acio devices to their own port (ex: SDVX5 expects an ICCA by itself on COM2, COM1 for valk)
