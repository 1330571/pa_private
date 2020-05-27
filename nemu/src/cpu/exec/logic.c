#include "cpu/exec.h"

make_EHelper(test) {
  TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
/*
DEST <- DEST AND SRC
CF <- 0
OF <- 0
PA2.2 Add 
*/
  int res;
  rtl_and(&res,&id_dest->val,&id_src->val);
  operand_write(id_dest,&res);
  int zeroValue = 0;
  rtl_set_CF(&zeroValue);
  rtl_set_ZF(&zeroValue);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t3,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t3);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  TODO();

  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}

make_EHelper(rol) {
  TODO();
  
  print_asm_template2(rol);
}
