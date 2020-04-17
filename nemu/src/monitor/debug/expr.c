#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum { //Token类型枚举
  TK_NOTYPE = 256, TK_EQ,TK_HEX_NUM,TK_NUM,
  TK_REG, //寄存器
  TK_AND,TK_OR,TK_NEQ,TK_NOT,TK_PTR,TK_NGA //NGA: 负数 PTR: 指针 NOT: 逻辑运算
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
  {"0x[0-9a-e]+",TK_HEX_NUM}, //Hex number 
  {"[0-9]+",TK_NUM},
  {"-",'-'},
  {"\\*",'*'},
  {"/",'/'},
  {"\\(",'('},
  {"\\)",')'},
  
  {"&&",TK_AND},
  {"\\|\\|",TK_OR},
  {"!=",TK_NEQ},

  {"\\*",TK_PTR},
  {"!",TK_NOT},

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
  int op = 1;///OP 表征 之前是否是符号 以此来区别 负数
  if(op){
    //pass
  }
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);
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
            //FIXME:  negative number
            if(substr_len > 31) substr_len = 31;
            for(int cnt = 0 ; cnt < substr_len ; ++cnt){
              tokens[nr_token].str[cnt] = substr_start[cnt];
            }
            tokens[nr_token].str[substr_len] = '\0';
            #ifdef DEBUG
              // Log("New Token: %s",tokens[nr_token].str);
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

bool check_parentheses(int p,int q){
  //Check if valid?
  if(tokens[p].type == '(' && tokens[q].type == ')'){
    int l_cnt = 0;

    while(p < q){
      if(tokens[p].type == '(') ++l_cnt;
      if(tokens[p].type == ')') --l_cnt;
      if(l_cnt <= 0){
        // printf("Fake parentheses\n");
        return false;
      }
      ++p;
    }

    if(tokens[q].type == ')')--l_cnt;
    if(l_cnt == 0){
      // printf("expression is surrounded by parentheses\n");
      return true;
    }else{
      printf("Bad Expression\n");
      return false;
    }
  }

  // printf("expression is not surrounded by parentheses\n");
  return false;// 没有被括号包围.
}

int getPriority(int type){

  if(type == '+' || type == '-') return 10;
  if(type == '*' || type == '/') return 20;
  if(type == TK_EQ || type == TK_NEQ) return 7;
  if(type == TK_AND)return 5;
  if(type == TK_OR) return 3;
  if(type == TK_PTR) return 55;
  if(type == TK_NOT) return 66;
  if(type == TK_NGA) return 67; //负号优先级高
  if(type == TK_NUM || type == TK_HEX_NUM) return 66666;
  printf("Type: %d Unknown Type\n",type);
  return 666666;//Error
}

uint32_t find_dominated_op(int p,int q,bool *success){
  //指导思想: 优先级最低 最右边,不在括号里
  /* + - 优先级 10
     x / 优先级 20
     ()  不记录优先级
  */
 *success = true;//FIXME: Didn't find sometime it will die
 int pos = p;
 int depth = 0;
 --p;
 while(p < q){
   ++p;

   if(tokens[p].type == TK_HEX_NUM || tokens[p].type == TK_NUM || tokens[p].type == TK_REG) continue;
   if(tokens[p].type == '(')++depth;
   else if(tokens[p].type == ')')--depth;
   else if(!depth){
     int priority1 = getPriority(tokens[pos].type);
     int priority2 = getPriority(tokens[p].type);
      if(priority2 < priority1){
        pos = p;//转移中心操作符号
        *success = true;
      }
   }
   if(depth < 0) { //Error
     *success = false;
     return 0;
   }
 }

 return pos;
}

uint32_t eval(int p,int q){
  // printf("eval ... %d To %d\n",p,q);
  if(p > q){
    //BAD 
    printf("Bad Expression, Something Went Wrong\n");
    return 0;
  }
  else if(p == q){
    //Single Token
    uint32_t res = 0;
    if(tokens[p].type == TK_REG){
      for(int i = 0 ; i < 8 ; ++i){
        if (strcmp(tokens[p].str+1,regsl[i]) == 0)
          res = cpu.gpr[i]._32;
      }
      if(strcmp(tokens[p].str,"$eip") == 0)
        res = cpu.eip;
    }
    else if(tokens[p].type==TK_NUM) sscanf(tokens[p].str,"%d",&res);//将str的内容赋值到res变量中
    else if(tokens[p].type==TK_HEX_NUM) sscanf(tokens[p].str,"%x",&res);//16进制
    return res;
  }
  else if (check_parentheses(p,q) == true){
    return eval(p+1,q-1);//脱掉括号
  }else{
    //此时要考虑一元运算符
    bool *success = (bool*)malloc(sizeof(bool));
    int pos = find_dominated_op(p,q,success);
    // printf("p= % d,q= % d,dominated = %d,value = %d\n",p,q,pos,tokens[pos].type);

    if(*success == true){
      free(success);
      int op = tokens[pos].type;
      
      if(pos == p){
        //此时考虑!问题 
        //第一个就是 主运算符
        switch(op){
          case TK_NOT:return (eval(p+1,q) != 0) ? 0 : 1;
          case TK_PTR:return vaddr_read(eval(p+1,q),4);
          case TK_NGA:return (-1 * eval(p+1,q));
          default:
            printf("Error Type...Check\n");
        }
      }
      
      uint32_t val1 = eval(p,pos-1);
      uint32_t val2 = eval(pos+1,q);

      switch(op){
        case '+':return val1+val2;
        case '-':return val1-val2;
        case '*':return val1*val2;
        case '/':return val1/val2;
        case TK_AND: return (val1 && val2) ? 1 : 0;
        case TK_OR: return (val1 || val2 != 0) ? 1: 0;
        case TK_EQ: return (val1 == val2) ? 1 : 0;
        case TK_NEQ: return (val1 != val2) ? 1 : 0;
        default:
          printf("Error Type\n");
      }

    }else{
      printf("Fatal Error In Eval Func No dominated op detected\n");
      free(success);
    }
  }
  printf("Fatal Error In Eval Func\n");
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    printf("illegal expression\n");
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  //此处用作处理 负数 和 取地址.
  bool may = true;
  for(int i = 0 ; i <  nr_token ; i++){
    //指导思想: 连续符号则判断为 二义 * 和 -
    if(tokens[i].type == TK_NUM || tokens[i].type == TK_HEX_NUM || tokens[i].type == TK_REG)
      may = false;
    else if (tokens[i].type == ')') may =false;
    else{
      if(!may)may = true;
      else{
        if(tokens[i].type == '-') tokens[i].type = TK_NGA;
        if(tokens[i].type == '*') tokens[i].type = TK_PTR;
      }
    }
  }
  //Time: 2020/4/13 PA 1.2 Version 1.0.0
  return eval(0,nr_token-1);

}
