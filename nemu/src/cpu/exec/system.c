#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
/*
PA3 6-9 V1.0
IF OperandSize = 16
Limit:Base = m16:24
Else
Limit:Base = m16:32
通过 IDTR 中的地址对 IDT 进行索引的时候，需要使用 vaddr_read()。
*/
  if(decoding.is_operand_size_16){
    cpu.IDTR.limit = vaddr_read(id_dest->addr,2);
    cpu.IDTR.base = vaddr_read(id_dest->addr+2,3);
  }else{
    cpu.IDTR.limit = vaddr_read(id_dest->addr,2);
    cpu.IDTR.base = vaddr_read(id_dest->addr+2,4);
  }
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  switch(id_dest->reg){
    case 0: cpu.cr0.val = id_src->val;break;
    case 3: cpu.cr3.val = id_src->val;break;
    default: assert(0);break;
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  switch(id_dest->reg){
    case 0:id_src->val = cpu.cr0.val;break;
    case 3:id_src->val = cpu.cr3.val;break;
    default: assert(0);break;
  }

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //PA3 Add
  raise_intr(id_dest->val,decoding.seq_eip);
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  rtl_pop(&decoding.jmp_eip);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.v);
  decoding.is_jmp = 1;

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  rtlreg_t tmp;
  tmp = pio_read(id_src->val,id_dest->width);
  operand_write(id_dest,&tmp);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  pio_write(id_dest->val,id_dest->width,id_src->val);
  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
