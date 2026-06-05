This library uses the sdvxio API of BT5 to communicate with spice2x. Thus, it allows you to 
use any sdvxio-supported device board with any version of SDVX that is supported by spice2x.

This is implemented similarly to KFChicken, which did the same thing except for KFCA -> spice.

The main use case of this is to use sdvxio-bio2 to get valkyrie-mode support without any patches, including LED support.
The setup instructions below will be for this use case.

# Setup

- Make sure that your BIO2 is flashed with a SDVX firmware (if you can boot SDVX5 with it, it's
  already the right firmware)
  - The firmware persists between boots, so as long as it's flashed to the correct firmware, you no
    longer need to do this step
- Rename `sdvxio-bio2.dll` to `sdvxio.dll`
- Run `spice-sdvxio.exe` so that the config file gets created
- Edit `spice-sdvxio.conf` as needed. Recommended to change `spice.password` at least
  - Take note of `spice.password` and the `spice.port` as both are needed for the spice2x launch options
- Ensure that your `gamestart.bat` calls spice2x in the following way:

```
spice64.exe -reader COM2 -api 1337 -apipass password
```

Explanation:
- `-reader COM2` is the port that your card reader is connected to. This will be COM1 for valk motherboards, and COM2 otherwise. Should match whatever you have it set to in device manager.
- `-api 1337` is the API port that spice2x uses. Should match `spice.port` in `spice-sdvxio.conf`.
- `-apipass password` is the password for the spice2x API. Should match `spice.password` in `spice-sdvxio.conf`.

These are the bare minimum options for this to function. 
You can add any other options you would normally have, such as `-url`, `-p`, `-sdvxnativetouch`, etc.

You should not pass `-exec soundvoltex.dll` otherwise the required emulation will be disabled.

# Usage

- Run `spice-sdvxio.exe`
- Start your game via `gamestart.bat`
- To quit the program, hit the TEST + SERVICE button at the same time

You can use the following for your `gamestart.bat`:

```batch
start spice-sdvxio.exe
spice64.exe -reader COM2 -api 1337 -apipass password
```

# LED Mapping

`spice-sdvxio.conf` supports the same LED mapping as sdvxhook2-valk.
Here's a sample of the LED mapping options available in `spice-sdvxio.conf`.

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


# Notes

- sdvxio-bio2 implements BIO2 device autodetection
  - if this fails please make a bug report, and set the port manually in the config
- You can change the port and baudrate by editing the sdvxio-bio2.conf that should be created by default
- Please connect any additional acio devices to their own port (ex: SDVX5 expects an ICCA by itself on COM2, COM1 for valk)
