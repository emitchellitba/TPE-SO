#include <info.h>

#include <shell.h>

extern command_entry_t command_table[];

char *reg_strings[] = {
    " RAX: ", " RBX: ", " RCX: ", " RDX: ", " RSI: ", " RDI: ",
    " RBP: ", " RSP: ", " R8: ",  " R9: ",  " R10: ", " R11: ",
    " R12: ", " R13: ", " R14: ", " R15: ", " RIP: "};

#define NUM_REGISTERS (sizeof(reg_strings) / sizeof(reg_strings[0]))

int get_registers_cmd() {
  printf(" Registers: \n");
  uint64_t regsStatus[17] = {0};
  get_regist(regsStatus);
  for (int i = 0; i < 17; i++) {
    printf("\n%s %x", reg_strings[i], regsStatus[i]);
  }
  printf("\n");

  return 0;
}

int help_cmd(int argc, char *argv[]) {
  if (argc >= 2 && str_cmp(argv[1], "tests") == 0) {
    print_available_tests();
  } else {
    printf("Command list:\n");
    for (int i = 0; i < get_total_commands(); i++) {
      printf("\t- %s\n", command_table[i].name);
    }
    printf("\nTo see the available tests, type 'help tests'.\n");
  }
  return 0;
}

/* Funcion no utilizada en comandos todavia */
int print_global_date_time() {
  date_time *dt = {0};
  load_date_time(dt);
  printf("%d/%d/%d %d:%d:%d\n", dt->day, dt->month, dt->year, dt->hour, dt->min,
         dt->sec);

  return 0;
}

int print_local_date_time_cmd() {
  date_time *dt = {0};
  load_date_time(dt);
  printf("%d/%d/%d %d:%d:%d\n", dt->day, dt->month, dt->year, dt->hour, dt->min,
         dt->sec);

  return 0;
}
