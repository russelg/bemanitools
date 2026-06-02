exes                  += spice-sdvxio

cppflags_spice-sdvxio  := \
    -I src/imports \

ldflags_spice-sdvxio  := \
    -lsetupapi \
    -lws2_32 \

libs_spice-sdvxio     := \
    cconfig \
    sdvxhook-util \
    sdvxio \
    util \

src_spice-sdvxio      := \
    main.c \
    config-spice-sdvxio.c \
    cJSON.c \
    spice_rc4.c \
    spice_connection.c \
    spice_wrappers.c \
    responsive-analog-read.c \
