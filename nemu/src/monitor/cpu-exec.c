#include "nemu.h"
#include "monitor/monitor.h"
#include "watchpoint.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 100000

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  if (nemu_state == NEMU_END) {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;

  bool print_flag = n < MAX_INSTR_TO_PRINT;

  for (; n > 0; n --) {
    /* Execute one instruction, including instruction fetch,
     * instruction decode, and the actual execution. */
    exec_wrapper(print_flag);
    WP* diff = scan_watchpoint();
    if(diff != NULL){
      //hit
      printf("Hit wachtpoint %d at address %d\n",diff->NO,cpu.eip);
      printf("expr\t\t= %s\n",diff->name);
      printf("old value = 0x%08d\n",diff->old_val);
      printf("new value = 0x%08d\n",diff->new_val);
      printf("program paused");
      //set value 
      nemu_state = NEMU_STOP;
      update_value();
    }
    
#ifdef DEBUG
    /* TODO: check watchpoints here. */

#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) { return; }
  }

  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}
