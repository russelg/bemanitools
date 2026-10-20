# Launcher

Launcher is a tool which is required to run the more recent titles. It sets up the AVS environment
for the target game to boot before it runs the it.

Typically, the tool is run when you use one of the bat script files to start any of the games that
require its usage, e.g. newer IIDX games.

It comes with option switches to configure the AVS environment and for development, e.g. Halting
until a debugger is attached, outputting debug output to a logfile etc. Just run the application
without any parameters to get a usage message:

```
launcher.exe
```

## Release code

The release code of the installed game is read from the bootstrap config
(`prop/bootstrap.xml`, option `-Z`):

```xml
<config>
    <release_code>2026082500</release_code>
    ...
</config>
```

That release code is what the arcade boot process puts into the `ext` field of the sidcode, hence it
takes precedence over the `ext` node of the ea3 config (`prop/ea3-config.xml`, option `-E`). The
value from the ea3 config is used as a fallback for setups without a bootstrap config or for
bootstrap configs that don't provide a release code. Launcher exits with an error if neither of the
two provides a release code.

The resulting sidcode is handed to the game's `dll_entry_init` and is also available to the game via
the `/env/profile/soft_id_code` environment variable.
