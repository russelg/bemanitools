#ifndef SDVXHOOK2_AC_IO_H
#define SDVXHOOK2_AC_IO_H

#include <windows.h>

#include "hook/iohook.h"

void ac_io_port_init(const wchar_t *card_reader_port);
void ac_io_port_fini(void);
HRESULT ac_io_port_dispatch_irp(struct irp *irp);

#endif
