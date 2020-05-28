#include "cpu/exec.h"

make_EHelper(mov) {
  //for example 
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

/*
make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}
*/

make_EHelper(push) {
  //PA2.1 Add ,transfer the source  operand to the top of the stack
  rtl_sext(&t3, &id_dest->val,id_dest->width);
  rtl_push(&t3);
  // operand_write(id_dest,&t0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //PA2.1 Add
  rtl_pop(&t3);
  operand_write(id_dest,&t3);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
// ESP ← EBP;
// EBP ← Pop();
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) { //from baidu: 扩展位数
  rtlreg_t tmp;
  if (decoding.is_operand_size_16) {
    rtl_lr_w(&tmp, R_AX); // Load AX

    if((int8_t)tmp < 0)
      reg_w(R_DX) = 0xffff;
    else 
      reg_w(R_DX) = 0;
  }
  else {
    rtl_lr_l(&tmp,R_EAX); // Load EAX
    
    if((int16_t)tmp < 0)
      reg_l(R_EDX) = 0xffffffff;
    else
      reg_l(R_EDX) = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) { //from baidu:有符号数。高16位还需要额外处理
  rtlreg_t tmp;
  if (decoding.is_operand_size_16) {
    rtl_lr_b(&tmp,R_AL);
    rtl_sext(&t3,&tmp,1);
    rtl_sr_w(R_AX,&t3);
  }
  else {
    rtl_lr_w(&tmp,R_AX);
    rtl_sext(&t3,&tmp,2);
    rtl_sr_l(R_EAX,&t3);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
