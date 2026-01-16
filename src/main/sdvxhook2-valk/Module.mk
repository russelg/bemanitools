avsdlls             += sdvxhook2-valk

ldflags_sdvxhook2-valk   := \
    -liphlpapi \
    -lsetupapi \
    -lcfgmgr32 \
    -lmf \
    -lmfplat \
    -lole32

deplibs_sdvxhook2-valk   := \
    avs \

libs_sdvxhook2-valk := \
    acioemu \
    camhook \
    d3d9exhook \
    sdvxio \
    hook \
    hooklib \
    cconfig \
    util \
    eamio

src_sdvxhook2-valk := \
    dllmain.c \
    iob2.c \
    config-io.c \
    ../sdvxhook2/acio.c \
    ../sdvxhook2/nvapi.c \
    ../sdvxhook2/power.c
