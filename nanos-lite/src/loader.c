#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //PA3 Add data:6.9 V 0.0
  //loader() 函数在 nanos-lite/src/loader.c 中定义，其中的 as 参数目前暂不使用，
  //可以忽略，而因为 ramdisk 中目前只要一个文件，filename 参数也可以忽略。
  //我们只需要用到 ramdisk_read 函数，其中第一个参数填入 DEFAULT_ENTRY，偏移量为 0，长度为 ramdisk 的大小即可。
  ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  return (uintptr_t)DEFAULT_ENTRY;
}
