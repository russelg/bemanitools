#include <windows.h>

#include <stdbool.h>

#include "sdvxhook2-valk/iob2.h"

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    if (reason != DLL_PROCESS_ATTACH) {
        goto end;
    }

    aio_iob2_hook_init(false);

end:
    return TRUE;
}
