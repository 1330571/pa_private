#ifndef __COMMON_H__
#define __COMMON_H__

#include <am.h>
#include <klib.h>
#include "debug.h"

extern void ramdisk_read(void *buf, off_t offset, size_t len)
extern size_t get_ramdisk_size();
typedef char bool;
#define true 1
#define false 0

#endif
