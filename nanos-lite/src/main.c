#include "common.h"

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_ASYE
//#define HAS_PTE

void init_mm(void);
void init_ramdisk(void);
void init_device(void);
void init_irq(void);
void init_fs(void);
uint32_t loader(_Protect *, const char *);

int main() {
#ifdef HAS_PTE
  init_mm();
#endif

  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

  init_ramdisk(); //磁盘

  init_device(); //初始化设备

#ifdef HAS_ASYE
  Log("Initializing interrupt/exception handler...");
  init_irq();
#endif

  init_fs(); //初始化 文件系统

  uint32_t entry = loader(NULL, "/bin/text"); //加载程序
  //将 ramdisk 中从 0 开始的所有内容放置在 0x4000000
  ((void (*)(void))entry)();

  panic("Should not reach here");
}
