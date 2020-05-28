#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  //the target address is calculated at the decode stage
  decoding.is_jmp = 1;
  rtl_push(eip);
  rtl_add(&decoding.jmp_eip,eip,&id_dest->val);
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  //PA2.1 Add
  // EIP ← Pop();
  decoding.is_jmp = 1;
  rtl_pop(&decoding.jmp_eip);
  print_asm("ret");
}

make_EHelper(call_rm) {
  // `Push(EIP);`
// `EIP ← [r/m32];`
  t3 = decoding.seq_eip;
  rtl_push(&t3); //PUSH 过程
  decoding.is_jmp = 1;
  decoding.jmp_eip = id_dest->val; //EIP赋值过程
  print_asm("call *%s", id_dest->str);
}
