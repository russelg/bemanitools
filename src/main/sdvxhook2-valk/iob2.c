#define LOG_MODULE "aio-iob2-hook"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
/**
 * If you use any of this code for support or reference for other tools,
 * please reach out so appropriate parties may be credited.
 *  - Xyen
 */
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#include <initguid.h>
#include <windows.h>

#include <cfgmgr32.h>
#include <setupapi.h>

#include "hook/table.h"

#include "util/array.h"
#include "util/defs.h"
#include "util/log.h"
#include "util/str.h"
#include "util/thread.h"
#include "util/time.h"

// technically not needed, but I'd like the logs to show up
#include "imports/avs.h"

// we never instanciated IIDXIO ourselves, we assume that the original iidxhook9
// does so
#include "bemanitools/sdvxio.h"
#include "bio2/bi2a-sdvx.h"

#define MAX_INSTANCES 1

struct AIO_NMGR_VTABLE {
    void *_dtor;
    void *AddNodeCtl;
    void *RemoveNodeCtl;
    void *PacketCreate;
    void *PacketSend;
    void *QueryPktRecvTimeout;
    void *PacketRecv;
    void *_GetCommStatus;
    void *_Reconnect;
    void *_unk;
    void *BeginManage;
};

struct AIO_WRFIRM {
    void *vtbl;
    void *filler;
};

struct AIO_NODE {
    void *vtbl;
    uint8_t data[0x13F0];
};

struct AIO_NMGR {
    struct AIO_NMGR_VTABLE *vtbl;
    uint8_t data[0x9F0];
};

struct AIO_SCI {
    void *vtbl;
    void *filler;
};

#pragma pack(push, 1)
struct bi2x_device_status {
    uint8_t unk1[4];
    uint8_t b_test;
    uint8_t b_service;
    uint8_t b_coinmech;
    uint8_t unk2;
    uint8_t b_start[2];
    uint8_t b_vefx;
    uint8_t b_effect;
    uint8_t b_headphone[2];

    uint8_t unk3[6]; // 0x10-0x13
    uint8_t a_turntable[2];
    uint8_t unk4;
    uint8_t unk6[4];
    uint8_t b_p1[7];
    uint8_t b_p2[7];
    uint8_t unk7[7];

    uint8_t unk8[0x9A]; // 0x30-0xC9
};
#pragma pack(pop)

_Static_assert(
    sizeof(struct bi2x_device_status) == 202,
    "bi2x_device_status is the wrong size");

static struct AIO_NODE *
my_aioIob2Bi2xUFC_Create(struct AIO_NMGR *node_mgr, uint8_t param);

struct dev_status;
static void my_GetDeviceStatus(void *this, struct dev_status *status);
static void my_SetWatchDogTimer(void *this, char timer);
static void my_IoReset(void *this, unsigned int state);

static void my_ControlCoinBlocker(void *this, int side_0, char state);
static void my_AddCounter(void *this, int side_0, char count);
static void my_SetIccrLed(void *this, unsigned int rgb);
static void my_SetPlayerButtonLamp(void *this, int sw_num, uint8_t state);

static void my_SetTapeLedData(void *this, unsigned int state, const void *data);

static const struct hook_symbol aio_iob2_syms[] = {
    {.name = "?AddCounter@AIO_IOB2_BI2X_UFC@@QEAAXII@Z",
     .patch = my_AddCounter,
     .link = NULL},
    {.name = "?SetWatchDogTimer@AIO_IOB2_BI2X_UFC@@QEAAXE@Z",
     .patch = my_SetWatchDogTimer,
     .link = NULL},
    {.name = "?ControlCoinBlocker@AIO_IOB2_BI2X_UFC@@QEAAXI_N@Z",
     .patch = my_ControlCoinBlocker,
     .link = NULL},
    {.name = "?IoReset@AIO_IOB2_BI2X_UFC@@QEAAXI@Z",
     .patch = my_IoReset,
     .link = NULL},
    {.name = "?SetIccrLed@AIO_IOB2_BI2X_UFC@@QEAAXI@Z",
     .patch = my_SetIccrLed,
     .link = NULL},
    {.name = "?SetPlayerButtonLamp@AIO_IOB2_BI2X_UFC@@QEAAXI_N@Z",
     .patch = my_SetPlayerButtonLamp,
     .link = NULL},
    {.name = "?SetTapeLedData@AIO_IOB2_BI2X_UFC@@QEAAXIPEBX@Z",
     .patch = my_SetTapeLedData,
     .link = NULL},
    {.name = "aioIob2Bi2xUFC_Create",
     .patch = my_aioIob2Bi2xUFC_Create,
     .link = NULL},
    {.name = "?GetDeviceStatus@AIO_IOB2_BI2X_UFC@@QEBAXAEAUDEVSTATUS@1@@Z",
     .patch = my_GetDeviceStatus,
     .link = NULL},
    // SetTapeLedDataLimit?
};

static struct AIO_SCI *my_aioIob2Bi2x_OpenSciUsbCdc(unsigned int device_num);
static struct AIO_NMGR *
my_aioNMgrIob2_Create(struct AIO_SCI *port, unsigned int delay);

static struct AIO_WRFIRM *
my_aioIob2Bi2x_CreateWriteFirmContext(unsigned int flags, int type);
static void my_aioIob2Bi2x_DestroyWriteFirmContext(struct AIO_WRFIRM *fw_obj);
static unsigned int my_aioIob2Bi2x_WriteFirmGetState(struct AIO_WRFIRM *fw_obj);

static const struct hook_symbol aio_iob_syms[] = {
    {.name = "aioIob2Bi2x_OpenSciUsbCdc",
     .patch = my_aioIob2Bi2x_OpenSciUsbCdc,
     .link = NULL},
    {.name = "aioNMgrIob2_Create",
     .patch = my_aioNMgrIob2_Create,
     .link = NULL},
    {.name = "aioIob2Bi2x_CreateWriteFirmContext",
     .patch = my_aioIob2Bi2x_CreateWriteFirmContext,
     .link = NULL},
    {.name = "aioIob2Bi2x_DestroyWriteFirmContext",
     .patch = my_aioIob2Bi2x_DestroyWriteFirmContext,
     .link = NULL},
    {.name = "aioIob2Bi2x_WriteFirmGetState",
     .patch = my_aioIob2Bi2x_WriteFirmGetState,
     .link = NULL},
};

static unsigned int my_aioNodeCtl_Destroy(struct AIO_NODE *node);
static unsigned int (*real_aioNodeCtl_Destroy)(struct AIO_NODE *node);

static unsigned int my_aioSci_Destroy(struct AIO_SCI *sci);
static unsigned int (*real_aioSci_Destroy)(struct AIO_SCI *sci);

static unsigned int my_aioNodeMgr_Destroy(struct AIO_NMGR *mgr);
static unsigned int (*real_aioNodeMgr_Destroy)(struct AIO_NMGR *mgr);

static const struct hook_symbol aio_syms[] = {
    // hook these to prevent freeing stuff that was never created
    // but leave intact so ICCA handler can cleanup after itself
    {.name = "aioNodeCtl_Destroy",
     .patch = my_aioNodeCtl_Destroy,
     .link = (void **) &real_aioNodeCtl_Destroy},
    {.name = "aioSci_Destroy",
     .patch = my_aioSci_Destroy,
     .link = (void **) &real_aioSci_Destroy},
    {.name = "aioNodeMgr_Destroy",
     .patch = my_aioNodeMgr_Destroy,
     .link = (void **) &real_aioNodeMgr_Destroy},
    // Technically not stubbing these out is fine, since we never attach our
    // node
    // {.name = "aioNodeCtl_UpdateDevicesStatus",
    //  .patch = my_aioNodeCtl_UpdateDevicesStatus,
    //  .link = NULL},
    // {.name = "?UpdateDevicesStatus@AIO_NCTL@@SAXXZ",
    //  .patch = my_aioNodeCtl_UpdateDevicesStatus,
    //  .link = NULL},
    // this one is how the ICC port gets opened, we can ignore
    // {.name = "aioSciComm_OpenB8PNS1",
    //  .patch = my_aioSciComm_OpenB8PNS1,
    //  .link = NULL},
};

//
// IMPL
//

static struct AIO_SCI *assigned_sci;
static struct AIO_NMGR *assigned_mgr;
static struct AIO_NODE *assigned_node;
static struct AIO_WRFIRM *assigned_fw_obj;

static bool poll_delay;
static bool force_headphones;

// libaio-iob2_video

static struct AIO_NODE *
my_aioIob2Bi2xUFC_Create(struct AIO_NMGR *node_mgr, uint8_t param)
{
    if (node_mgr) {
        if (node_mgr == assigned_mgr) {
            struct AIO_NODE *node = malloc(sizeof(struct AIO_NODE));
            memset(node, 0, sizeof(struct AIO_NODE));

            // the node returned must be of the right size
            // since the game will try to access parts of its memory
            // failure to do so will result in intermittent crashes

            log_info("my_aioIob2Bi2xUFC_Create: returning custom node");
            assigned_node = node;
            return node;
        }
    }

    log_warning("my_aioIob2Bi2xUFC_Create: called without custom mgr");
    return NULL;
}

static uint8_t check_pin(uint16_t value, uint8_t pin)
{
    return (value >> pin) & 1;
}

#pragma pack(push, 1)
struct sys_input {
    uint8_t dev_io_counter;
    uint8_t b_ex_io_a_err;
    uint8_t b_ex_io_b_err;
    uint8_t b_pc_power_on;
    uint8_t b_pc_power_check;
    uint8_t coin_count;
    uint8_t b_test;
    uint8_t b_service;
    uint8_t b_coin_sw;
    uint8_t b_coin_jam;
    uint8_t b_hp_detect;
};

struct game_input {
    uint16_t analog_left;
    uint16_t analog_right;
    struct {
        bool b_start : 1;
        bool b_a : 1;
        bool b_b : 1;
        bool b_c : 1;
        bool b_d : 1;
        bool b_fx_l : 1;
        bool b_fx_r : 1;
        uint8_t padding : 1;
    } buttons;
};

struct dev_status {
    uint8_t input_counter;
    uint8_t output_counter;
    uint8_t io_reset_counter;
    uint8_t tape_led_counter;
    uint8_t tape_led_rate[8];
    struct sys_input sys_input;
    uint8_t unk_1[289];
    struct game_input game_input[16];
    uint8_t unk_2[22];
};
#pragma pack(pop)

_Static_assert(
    sizeof(struct dev_status) == 414, "dev_status is the wrong size");

static uint8_t counter;
static void my_GetDeviceStatus(void *this, struct dev_status *status)
{
    // TODO: tapeled
    sdvx_io_set_pwm_light(0x0, 0);
    sdvx_io_set_pwm_light(0x1, 0);
    sdvx_io_set_pwm_light(0x2, 0);
    sdvx_io_set_pwm_light(0x3, 0);
    sdvx_io_set_pwm_light(0x4, 0);
    sdvx_io_set_pwm_light(0x5, 0);

    sdvx_io_set_pwm_light(0x6, 0);
    sdvx_io_set_pwm_light(0x7, 0);
    sdvx_io_set_pwm_light(0x8, 0);
    sdvx_io_set_pwm_light(0x9, 0);
    sdvx_io_set_pwm_light(0xA, 0);
    sdvx_io_set_pwm_light(0xB, 0);

    sdvx_io_set_pwm_light(0xC, 0);
    sdvx_io_set_pwm_light(0xD, 0);
    sdvx_io_set_pwm_light(0xE, 0);

    sdvx_io_set_pwm_light(0xF, 0);
    sdvx_io_set_pwm_light(0x10, 0);
    sdvx_io_set_pwm_light(0x11, 0);

    sdvx_io_write_output();

    if (poll_delay) {
        Sleep(1);
    }

    sdvx_io_read_input();
    uint8_t sys = sdvx_io_get_input_gpio_sys();
    uint16_t gpio0 = sdvx_io_get_input_gpio(0);
    uint16_t gpio1 = sdvx_io_get_input_gpio(1);

    status->input_counter = counter;
    status->sys_input.dev_io_counter = counter;
    counter++;

    status->sys_input.b_test = check_pin(sys, SDVX_IO_IN_GPIO_SYS_TEST);
    status->sys_input.b_service = check_pin(sys, SDVX_IO_IN_GPIO_SYS_SERVICE);
    status->sys_input.b_coin_sw = check_pin(sys, SDVX_IO_IN_GPIO_SYS_COIN);
    status->sys_input.coin_count += sdvx_io_get_coins();

    if (force_headphones) {
        status->sys_input.b_hp_detect = 1;
    } else {
        status->sys_input.b_hp_detect = check_pin(gpio0, SDVX_IO_IN_GPIO_0_HEADPHONE);
    }

    uint16_t analog_left = sdvx_io_get_spinner_pos(0) << 6;
    uint16_t analog_right = sdvx_io_get_spinner_pos(1) << 6;

    // tl;dr game uses the past 16 "polls" of input and uses counter2 as a
    // tracker
    for (size_t i = 0; i < 16; ++i) {
        status->game_input[i].analog_left = analog_left;
        status->game_input[i].analog_right = analog_right;

        status->game_input[i].buttons.b_start = check_pin(gpio0, SDVX_IO_IN_GPIO_0_START);
        status->game_input[i].buttons.b_a = check_pin(gpio0, SDVX_IO_IN_GPIO_0_A);
        status->game_input[i].buttons.b_b = check_pin(gpio0, SDVX_IO_IN_GPIO_0_B);
        status->game_input[i].buttons.b_c = check_pin(gpio0, SDVX_IO_IN_GPIO_0_C);
        status->game_input[i].buttons.b_d = check_pin(gpio1, SDVX_IO_IN_GPIO_1_D);
        status->game_input[i].buttons.b_fx_l = check_pin(gpio1, SDVX_IO_IN_GPIO_1_FX_L);
        status->game_input[i].buttons.b_fx_r = check_pin(gpio1, SDVX_IO_IN_GPIO_1_FX_R);
    }
}

static void my_SetWatchDogTimer(void *this, char timer)
{
    // nothing
}
static void my_IoReset(void *this, unsigned int state)
{
    // nothing
}

static void my_ControlCoinBlocker(void *this, int side_0, char state)
{
    sdvx_io_set_coin_blocker(state);
}
static void my_AddCounter(void *this, int side_0, char count)
{
    // nothing
}
static void my_SetIccrLed(void *this, unsigned int rgb)
{
    // nothing
}
static uint32_t sdvx_gpio_lights = 0;
static void my_SetPlayerButtonLamp(void *this, int sw_num, uint8_t state)
{
    /**
     * sw_num
     * 0: START, 1: BT_A, 2: BT_B, 3: BT_C, 4: BT_D, 5: FX_L, 6: FX_R
     *
     * state
     * 0: ON, 1: OFF
     */

    static const uint8_t sw_num_to_gpio[] = {
        SDVX_IO_OUT_GPIO_START,
        SDVX_IO_OUT_GPIO_A,
        SDVX_IO_OUT_GPIO_B,
        SDVX_IO_OUT_GPIO_C,
        SDVX_IO_OUT_GPIO_D,
        SDVX_IO_OUT_GPIO_FX_L,
        SDVX_IO_OUT_GPIO_FX_R,
    };

    if (sw_num < 0 || sw_num >= lengthof(sw_num_to_gpio)) {
        return;
    }

    if (state == 0) {
        sdvx_gpio_lights |= (1 << sw_num_to_gpio[sw_num]);
    } else {
        sdvx_gpio_lights &= ~(1 << sw_num_to_gpio[sw_num]);
    }

    sdvx_io_set_gpio_lights(sdvx_gpio_lights);

    // TODO: should sdvx_io_write_output() get called here?
}

static void my_SetTapeLedData(void *this, unsigned int state, const void *data)
{
    // TODO
}

// libaio-iob

static struct AIO_SCI *my_aioIob2Bi2x_OpenSciUsbCdc(unsigned int device_num)
{
    if (device_num < MAX_INSTANCES) {
        struct AIO_SCI *port = malloc(sizeof(struct AIO_SCI));
        memset(port, 0, sizeof(struct AIO_SCI));

        log_info("aioIob2Bi2x_OpenSciUsbCdc: returning custom port");
        assigned_sci = port;
        return port;
    }

    log_warning(
        "aioIob2Bi2x_OpenSciUsbCdc: called with device_num: %d", device_num);
    return NULL;
}

static struct AIO_NMGR_VTABLE nmgr_vtbl;

static void my_AIO_NMGR_BeginManage(struct AIO_NMGR *this)
{
    // see: my_aioNMgrIob2_Create for details on why this exists
    log_info("AIO_NMGR::BeginManage: stub");
}

static struct AIO_NMGR *
my_aioNMgrIob2_Create(struct AIO_SCI *port, unsigned int delay)
{
    if (port) {
        if (port == assigned_sci) {
            struct AIO_NMGR *mgr = malloc(sizeof(struct AIO_NMGR));
            memset(mgr, 0, sizeof(struct AIO_NMGR));

            // not only do we have to allocate the object, we need to make it a
            // fake C++ object since the game actually tries to call vfuncs
            mgr->vtbl = &nmgr_vtbl;
            nmgr_vtbl.BeginManage = &my_AIO_NMGR_BeginManage;

            log_info("aioNMgrIob2_Create: returning custom mgr");
            assigned_mgr = mgr;
            return mgr;
        }
    }

    log_warning("aioNMgrIob2_Create: called with without custom port");
    return NULL;
}

static struct AIO_WRFIRM *
my_aioIob2Bi2x_CreateWriteFirmContext(unsigned int flags, int type)
{
    if (type > 2) {
        log_warning(
            "aioIob2Bi2x_CreateWriteFirmContext: unknown type called: %d",
            type);
    }

    if (!assigned_fw_obj) {
        assigned_fw_obj = malloc(sizeof(struct AIO_WRFIRM));
        memset(assigned_fw_obj, 0, sizeof(struct AIO_WRFIRM));

        log_info("aioIob2Bi2x_CreateWriteFirmContext: returning custom fw obj");
        return assigned_fw_obj;
    }

    log_info("aioIob2Bi2x_CreateWriteFirmContext: fw obj already assigned?");
    return NULL;
}

static void my_aioIob2Bi2x_DestroyWriteFirmContext(struct AIO_WRFIRM *fw_obj)
{
    if (fw_obj == assigned_fw_obj) {
        free(fw_obj);
        assigned_fw_obj = NULL;
    }
}

static unsigned int my_aioIob2Bi2x_WriteFirmGetState(struct AIO_WRFIRM *fw_obj)
{
    // stage 8 is complete
    log_info("my_aioIob2Bi2x_WriteFirmGetState: Telling it we're done");
    return 8;
}

// libaio

static unsigned int my_aioNodeCtl_Destroy(struct AIO_NODE *node)
{
    if (node == assigned_node) {
        free(node);
        assigned_node = NULL;
        return 0;
    }

    return real_aioNodeCtl_Destroy(node);
}

static unsigned int my_aioSci_Destroy(struct AIO_SCI *sci)
{
    if (sci == assigned_sci) {
        free(sci);
        assigned_sci = NULL;
        return 0;
    }

    return real_aioSci_Destroy(sci);
}

static unsigned int my_aioNodeMgr_Destroy(struct AIO_NMGR *mgr)
{
    if (mgr == assigned_mgr) {
        free(mgr);
        assigned_mgr = NULL;
        return 0;
    }

    return real_aioNodeMgr_Destroy(mgr);
}

void aio_iob2_hook_init(bool disable_poll_limiter, bool force_headphones_val)
{
    poll_delay = !disable_poll_limiter;
    force_headphones = force_headphones_val;

    if (!poll_delay) {
        log_warning("aio_iob2_hook_init: poll_delay has been disabled");
    }

    if (force_headphones) {
        log_info("aio_iob2_hook_init: force_headphones has been enabled");
    }

    // if (!sdvx_io_init(
    //         avs_thread_create, avs_thread_join, avs_thread_destroy)) {
    //     log_fatal("Initializing SDVX IO backend failed");
    // }

    if (avs_is_active()) {
        log_to_external(
            log_body_misc, log_body_info, log_body_warning, log_body_fatal);
    } else {
        log_to_writer(log_writer_file, stdout);
    }

    hook_table_apply(
        NULL, "libaio-iob2_video.dll", aio_iob2_syms, lengthof(aio_iob2_syms));

    hook_table_apply(
        NULL, "libaio-iob.dll", aio_iob_syms, lengthof(aio_iob_syms));

    hook_table_apply(NULL, "libaio.dll", aio_syms, lengthof(aio_syms));

    log_info("Inserted AIO iob2_video hooks");
}
