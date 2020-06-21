#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

void* new_page(void);
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

uintptr_t loader(_Protect *as, const char *filename) {
  //PA3 Add data:6.9 V 0.0
  //loader() 函数在 nanos-lite/src/loader.c 中定义，其中的 as 参数目前暂不使用，
  //可以忽略，而因为 ramdisk 中目前只要一个文件，filename 参数也可以忽略。
  //我们只需要用到 ramdisk_read 函数，其中第一个参数填入 DEFAULT_ENTRY，偏移量为 0，长度为 ramdisk 的大小即可。
  // ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());

  /* 
  Version2
  打开待装入的文件后，还需要获取文件大小；
  需要循环判断是否已创建足够的页来装入程序；  
  对于程序需要的每一页，做三个事情，即4，5，6步：
  使用 Nanos-lite 的 MM 提供的 new_page() 函数获取一个空闲物理页
  使用映射函数 _map() 将本虚拟空间内当前正在处理的这个页和上一步申请到的空闲物理页建立映射
  读一页内容，写到这个物理页上
  每一页都处理完毕后，关闭文件，并返回程序入口点地址（虚拟地址）
  */
  printf("loader start to work\n");
  int fd = fs_open(filename,0,0); //无需考虑flags和mode
  // fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd)); //读取文件到ENTRY位置
  size_t sz = fs_filesz(fd);
  int pages = sz / PGSIZE + (sz % PGSIZE != 0);
  void *va = DEFAULT_ENTRY;
  for(int i = 0 ; i < pages ; ++i){
    void *pa = new_page();  
    Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
    _map(as,va,pa);
    fs_read(fd,pa,PGSIZE); //Read a new page
    va += PGSIZE;
  }
  int tmp;
  sscanf("%d",&tmp);
  fs_close(fd); //释放文件资源
  return (uintptr_t)DEFAULT_ENTRY;
}
