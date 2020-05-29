#include "cpu/exec.h"

make_EHelper(test) {
  //LeftSrc AND RightSrc
  //CF <- 0
  //OF <- 0
  rtlreg_t tmpValue;
  rtl_and(&tmpValue,&id_dest->val,&id_src->val);
  t0 = 0;
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);

  rtl_update_ZFSF(&tmpValue,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
/*
DEST <- DEST AND SRC
CF <- 0
OF <- 0
PA2.2 Add 
*/
  rtlreg_t res;
  rtl_and(&res,&id_dest->val,&id_src->val);
  operand_write(id_dest,&res);

  rtlreg_t zeroValue = 0;
  rtl_set_CF(&zeroValue);
  rtl_set_OF(&zeroValue);

  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtlreg_t tmp;
  rtl_xor(&tmp,&id_dest->val,&id_src->val);
  operand_write(id_dest,&tmp);
  t2 = 0;
  rtl_set_CF(&t2);
  rtl_set_OF(&t2);
  rtl_update_ZFSF(&tmp,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtlreg_t res;
  rtl_or(&res,&id_dest->val,&id_src->val);
  operand_write(id_dest,&res);
  rtlreg_t zeroValue = 0;
  rtl_set_CF(&zeroValue);
  rtl_set_ZF(&zeroValue);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtlreg_t tmp,op1,op2;
  op1 = id_dest->val;
  op2 = id_src->val;
  rtl_sar(&tmp,&op1,&op2);
  operand_write(id_dest,&tmp);
  rtl_update_ZF(&tmp, id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtlreg_t tmp,op1,op2;
  op1 = id_dest->val;
  op2 = id_src->val;
  rtl_shl(&tmp,&op1,&op2);
  operand_write(id_dest,&tmp);
  rtl_update_ZF(&tmp, id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  rtlreg_t tmp,op1,op2;
  op1 = id_dest->val;
  op2 = id_src->val;
  rtl_shr(&tmp,&op1,&op2);
  operand_write(id_dest,&tmp);
  rtl_update_ZF(&tmp, id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtlreg_t tmp;
  rtl_li(&tmp,id_dest->val);
  rtl_not(&tmp);
  operand_write(id_dest,&tmp);
  print_asm_template1(not);
}

make_EHelper(rol) {
  //循环n次
  rtlreg_t T1,T2;
  for(int i = 0 ; i < id_src->val ; ++i){
    rtl_shri(&T1,&id_dest->val,id_dest->width * 8 - 1); //右移
    rtl_shli(&T1,&id_dest->val,1); //左移
    rtl_xori(&T2,&id_dest->val,T1);
  }
  rtl_set_CF(&T1);
  operand_write(id_dest,&T2);
  print_asm_template2(rol);
}
