#include <info.h>

extern command_entry_t command_table[];

char *reg_strings[] = {
    " RAX: ", " RBX: ", " RCX: ", " RDX: ", " RSI: ", " RDI: ",
    " RBP: ", " RSP: ", " R8: ",  " R9: ",  " R10: ", " R11: ",
    " R12: ", " R13: ", " R14: ", " R15: ", " RIP: "};

#define NUM_REGISTERS (sizeof(reg_strings) / sizeof(reg_strings[0]))

int get_registers_cmd() {
  printf(" REGISTROS: \n");
  uint64_t regsStatus[17] = {0};
  get_regist(regsStatus);
  for (int i = 0; i < 17; i++) {
    printf("\n%s %x", reg_strings[i], regsStatus[i]);
  }
  printf("\n");

  return 0;
}

int help_cmd() {
  printf("Command list:\n");
  for (int i = 0; i < get_total_commands(); i++) {
    printf("\t- %s\n", command_table[i].name);
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

int show_kmsg_cmd() {
  char log[KMSG_BUFF_SIZE];
  size_t size = KMSG_BUFF_SIZE;
  read_kmsg(log, size);
  printf("%s", log);

  return 0;
}

int show_processes_cmd() {
  int pid;
  char *argv[] = {"ps", NULL};
  int argc = 1;

  pid = spawn_process("ps", argc, argv);

  if (pid < 0) {
    printf("Error spawning process 'ps'\n");
    return -1;
  }

  int status;
  if (wait_pid(pid, &status) < 0) {
    printf("Error waiting for process 'ps'\n");
    return -1;
  }

  return 0;
}

int show_programs_cmd() {
  int pid;
  char *argv[] = {"ls", NULL};
  int argc = 1;

  pid = spawn_process("ls", argc, argv);

  if (pid < 0) {
    printf("Error spawning process 'ls'\n");
    return -1;
  }

  return 0;
}
