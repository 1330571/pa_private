#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
#ifdef DEBUG
  Log("parameters: %s", args);
#endif
  int num = 1;
  if (args)
    num = atoi(args);
  cpu_exec(num);
  return 0;
}

static int cmd_info(char *args)
{
#ifdef DEBUG
  Log("parameters: %s", args);
#endif
  if (args)
  {
    if (args[0] == 'r')
    {
      for (int i = 0; i < 8; ++i)
        printf("%s:    0X%08X    %d\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
      printf("%s:    0x%08X    %d\n", "eip", cpu.eip, cpu.eip);
    }
    else if (args[0] == 'w')
    {
      //TODO work that in PA1.3
    }
    else
    {
      printf("Unknown parameter\n");
    }
  }
  else
    printf("No parameters\n"); //没有键入指令
  return 0;
}

static int cmd_x(char *args)
{
//split the string
char *num = strtok(NULL," ");
char *start = strtok(NULL," ");
#ifdef DEBUG
  Log("parameters: %s %s",num,start);
  Log("parameters: %s", args);
#endif
  if(num && start){
    printf("Address    Dword block ... Byte sequence\n");
    vaddr_t startAddr;
    sscanf(start,"%x",&startAddr);
    int len = atoi(num);
    for(int i = 0 ; i < len ; ++i){
      uint32_t mem = vaddr_read(startAddr,4);
      printf("0x%08x  0x%08x ... %02x %02x %02x %02x\n",startAddr,mem,mem & 0xff,(mem & 0xff00)>>8 ,(mem & 0xff0000)>>16,(mem& 0xff000000) >> 24);
      startAddr += 4;
  }
  }else {
    printf("not enough parameters, if confused, type help to see more\n");
  }
return 0;
}

static int cmd_p(char *args){
  bool success;
  expr(args,success);
  return 0;
  //FIXME: return ?
}

static struct
{
  char *name;
  char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "[num] Step forward", cmd_si},
    {"info", "[r,w] Display informations about regs", cmd_info},
    {"x", "(length) (start_addr) Display address from start_addr to start_addr + len", cmd_x},
    {"p", "calculate the expression supporting regs and pointers,cmd_p"}
    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode)
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  while (1)
  {
    char *str = rl_gets();             //读入命令
    char *str_end = str + strlen(str); //命令尾的地址

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1; //args的位置从 指向cmd后第一个字符
    if (args >= str_end)
    {
      args = NULL; //没有参数
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        } //执行
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}
