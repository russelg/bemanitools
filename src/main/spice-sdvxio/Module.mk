dlls                  += spice-sdvxio

ldflags_spice-sdvxio  := \
    -lsetupapi \

libs_spice-sdvxio     := \
    cconfig \
    sdvxhook-util \
    sdvxio \
    util \

src_spice-sdvxio      := \
    dllmain.c \
    config-spice-sdvxio.c \
    responsive-analog-read.c \
