#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum { //Token类型枚举
  TK_NOTYPE = 256, TK_EQ,TK_HEX_NUM,TK_NUM,
  TK_REG
  /* TODO: Add more token types */

};
//Mission 1.1 编写规则
/*
十进制数字、十六进制数字，如 0x1234，567；
现阶段所定义的 9 个寄存器，如 $eax, $ebx；
左括号、右括号；
加号、减号、乘号、除号；
空格串（一个或多个空格）;
*/
static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  //2020-4-13 日 Version 1.00
  {"0x[0-9]+",TK_HEX_NUM}, //Hex number 
  {"[0-9]+",TK_NUM},
  {"-",'-'},
  {"\\*",'*'},
  {"/",'/'},
  {"\\(",'('},
  {"\\)",')'},

  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)",TK_REG}

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token { //词法分析 单元
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          // default: TODO();
          default:
            tokens[nr_token].type = rules[i].token_type;
            //FIXME:  Test Right? Or Not?
            if(substr_len > 31) substr_len = 31;
            for(int cnt = 0 ; cnt < substr_len ; ++cnt){
              tokens[nr_token].str[cnt] = substr_start[cnt];
            }
            tokens[nr_token].str[substr_len] = '\0';
            #ifdef DEBUG
              Log("New Token: %s",tokens[nr_token].str);
            #endif
            nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false; //没有搜索到合法的表达式
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
