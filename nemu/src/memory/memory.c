#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_id = is_mmio(addr);
  if(mmio_id != -1){
    return mmio_read(addr,len,mmio_id);
  }
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int mmio_id = is_mmio(addr);
  if(mmio_id != -1)
    mmio_write(addr,len,data,mmio_id);
  else
    memcpy(guest_to_host(addr), &data, len);
}

bool is_writing = false;

paddr_t page_translate(vaddr_t vaddr){
  PDE pde; 
  PTE pte;
  //10 目录索引 10 页表索引 12 业内偏移
  paddr_t catalog_dir = (vaddr >> 22);
  paddr_t page_dir = (vaddr >> 12) & 0x3ff;
  paddr_t offest = vaddr & 0xfff;
  paddr_t page_addr = (cpu.cr3.page_directory_base << 12) + (catalog_dir) * 4;
  
  pde.val = paddr_read(page_addr,4); //4byte
  assert(pde.present);
  Log("pde.val=%x",pde.val);
  paddr_t content_addr = (pde.val & 0xfffff000) + (page_dir) * 4;

  pte.val = paddr_read(content_addr,4);
  Log("pte.val=%x",pte.val);
  assert(pte.present);
  //Load successfully
  paddr_t final_addr = (pte.val & 0xfffff000) + offest;
  pde.accessed = 1;
  if(pte.accessed == 0 || (pte.dirty == 0 && is_writing)){
    pte.accessed = 1;
    pte.dirty = 1;
  }
  
  paddr_write(page_addr,4,pde.val); //write back
  paddr_write(content_addr,4,pte.val);  //write back
  Log("final_addr=%x",final_addr);
  return final_addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  // return paddr_read(addr, len);
  is_writing = false;
  if(cpu.cr0.paging){
    //越界
    if(((addr & 0xfff) + len) > 0x1000){
      assert(0);
    }else{
      paddr_t paddr = page_translate(addr);
      return paddr_read(paddr,len);
    }
  }else 
  return paddr_read(addr,len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  // paddr_write(addr, len, data);
  is_writing = true;
  if(cpu.cr0.paging){
    if(((addr & 0xfff) + len) > 0x1000){
      assert(0);
    }else{
      paddr_t paddr = page_translate(addr);
      paddr_write(paddr,len,data);
    }
  }else{
    paddr_write(addr,len,data);
  }
}
