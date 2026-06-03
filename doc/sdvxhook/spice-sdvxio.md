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
spice2x.exe -reader COM2 -api 1337 -apipass password
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

# Notes

- sdvxio-bio2 implements BIO2 device autodetection
  - if this fails please make a bug report, and set the port manually in the config
- You can change the port and baudrate by editing the sdvxio-bio2.conf that should be created by default
- Please connect any additional acio devices to their own port (ex: SDVX5 expects an ICCA by itself on COM2, COM1 for valk)
