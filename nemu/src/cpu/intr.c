#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.v);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  //依次入栈，有用的部分，前4位： 31..16 15  后4位： 15..0
  assert(NO*8 <= cpu.IDTR.limit);
  vaddr_t addr = cpu.IDTR.base + 8 * NO;
  uint32_t high_bit = vaddr_read(addr+4,4) & 0xffff0000, low_bit = vaddr_read(addr,4)&0x0000ffff;//上层31.。16 以及 下层15.。0
  decoding.is_jmp = 1;
  decoding.jmp_eip = high_bit|low_bit;
}

void dev_raise_intr() {
}
