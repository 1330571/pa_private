int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

<<<<<<< HEAD
  //GO FOR IT ! START PA1.1 COMMENT
=======

  //print test!!
>>>>>>> 23e2df4bf7672c796447dbec271880b59a0d29ce
  return 0;
}
