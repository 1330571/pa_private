#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  //expr old_nal new_val @author: xys @time 2020-4-17
  char name[32];
  uint32_t old_val,new_val;
} WP;

int set_watchpoint(char *e);
void list_watchpoint(void);
bool delete_watchpoint(int NO);
WP* scan_watchpoint(void);
void update_value(void);
bool check(void);

#endif
