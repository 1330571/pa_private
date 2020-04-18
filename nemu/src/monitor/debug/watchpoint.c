#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static int cnt = 0;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

//new_wp
//从free_链表中返回空闲监视点
//@parameter: idx : Index Of Cpu Regs
WP *new_wp()
{
  if (free_ == NULL)
  {
    printf("too many watchpoints\n");
    assert(0);
  }
  WP *tmp = free_;
  free_ = free_->next;
  return tmp;
}
//归还wp到free_链表
void free_wp(WP *wp)
{
  if (head == wp)
  {
    //是链表头
    head = head->next;
  }
  else
  {
    WP *it = head->next, *q = head;
    while (it != wp)
    {
      it = it->next;
      q = q->next;
    }
    //it == wp;
    q->next = wp->next;
  }
  WP *tmp = free_;
  free_ = wp;
  wp->next = tmp;
}

int set_watchpoint(char *e)
{
  if(e == NULL){
    printf("No arguments given\n");
    return 0;
  }

  int idx = 66;
  for (int i = 0; i < 8; ++i)
  {
    if (strcmp(e, regsl) == 0)
      idx = i;
  }

  WP *tmp = new_wp();
  strcpy(tmp->name, e);
  // if(idx == 66) tmp->old_val = 0;
  // else tmp->old_val = (idx == -1) ? cpu.eip : cpu.gpr[idx]._32;
  bool success;
  tmp->old_val = expr(e,&success);
  printf("Set watchpoint #%d\n", cnt++);
  printf("expr\t\t= $s\n", e);
  printf("old value = %x (%d)\n", tmp->old_val, tmp->old_val);
  return 1;
}

void list_watchpoint(void)
{
  WP *ptr = head;
  printf("No Expr\t\tOld Value\n");
  while (ptr != NULL)
  {
    printf("%3d %-16s 0x%08X",ptr->NO,ptr->name,ptr->old_val);
  }
}

bool delete_watchpoint(int NO){
  WP *ptr = head;
  while(ptr != NULL){
    if(ptr->NO == NO){
      free_wp(ptr);
      return true;
    }
  }
  printf("NO = %d doesn't exist\n",NO);
  return false;
}

WP* scan_watchpoint(void){
  WP *ptr = head;
  WP *first_diff = NULL;
  while(ptr != NULL){
    bool success;
    ptr->new_val = expr(ptr->name,&success);
    if(ptr->new_val != ptr->old_val){
      first_diff = ptr;
    }
  }
  return first_diff;
}

void update_value(void){
  WP* ptr = head;
    while(ptr != NULL){
      ptr->new_val = ptr->old_val;
  }
}

bool check(void){
    WP* diff = scan_watchpoint();
    if(diff != NULL){
      //hit
      printf("Hit wachtpoint %d at address %d\n",diff->NO,cpu.eip);
      printf("expr\t\t= %s\n",diff->name);
      printf("old value = 0x%08d\n",diff->old_val);
      printf("new value = 0x%08d\n",diff->new_val);
      printf("program paused");
      //set value 
      update_value();
      return true;
    }
    return false;
}