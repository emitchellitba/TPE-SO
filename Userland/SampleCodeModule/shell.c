#include <shell.h>
#include <stdLibrary.h>

#define BUFF_SIZE 500
#define CANT_CMDS 9
#define SPECIAL_KEY_MAX_VALUE 5

char *reg_strings[] = {
    " RAX: ", " RBX: ", " RCX: ", " RDX: ", " RSI: ", " RDI: ",
    " RBP: ", " RSP: ", " R8: ",  " R9: ",  " R10: ", " R11: ",
    " R12: ", " R13: ", " R14: ", " R15: ", " RIP: "};
char *command_strings[] = {"help",     "date",       "zoom in",
                           "zoom out", "color font", "color background",
                           "clear",    "exit",       "registers"};
static uint8_t on = 1;

typedef void (*TerminalCommand)();
static TerminalCommand commands[] = {help,
                                     print_local_date_time,
                                     zoom_in,
                                     zoom_out,
                                     change_font_color,
                                     change_bg_color,
                                     clear,
                                     exit,
                                     get_registers};
void show_prompt();
extern void get_regist(uint64_t registers);

void start_shell() {
  char input_buffer[BUFF_SIZE];
  while (on) {
    show_prompt();
    get_input(input_buffer);
    to_lower(input_buffer);
    command_id command = process_input(input_buffer);
    if (command != -1) {
      commands[command]();
    } else {
      printf("Unknown Command\n");
    }
  }
}

void show_prompt() { printf("user@itba:> "); }

void get_input(char *buffer) {
  char *c = buffer;
  int limit_count = 0;

  do {
    *c = get_char();
    if (*c <= SPECIAL_KEY_MAX_VALUE) {
      c--;
    } else if (*c == '\b') {
      if (c > buffer) {
        put_char(*c);
        c--;
      }
      c--;
    } else {
      put_char(*c);
      limit_count++;
      if (limit_count > BUFF_SIZE)
        break;
    }
  } while ((*c++) != '\n');
  *(c - 1) = '\0';
}

command_id process_input(char *input) {
  int index = -1;
  for (int i = 0; i < CANT_CMDS && (index == -1); i++) {
    if (str_cmp(input, command_strings[i]) == 0)
      index = i;
  }
  return index;
}

void help() {
  printf("Command list:\n");
  for (int i = 0; i < CANT_CMDS; i++) {
    printf("\t- %s\n", command_strings[i]);
  }
}

void print_global_date_time() {
  date_time *dt = {0};
  load_date_time(dt);
  printf("%d/%d/%d %d:%d:%d\n", dt->day, dt->month, dt->year, dt->hour, dt->min,
         dt->sec);
}

void print_local_date_time() {
  date_time *dt = {0};
  load_date_time(dt);
  printf("%d/%d/%d %d:%d:%d\n", dt->day, dt->month, dt->year, dt->hour, dt->min,
         dt->sec);
}

void command_not_found() { printf("\tCommand not found\n"); }

void exit() {
  printf("\n");
  printf("Saving session...\n");
  printf("Session saved\n");
  printf("Exiting...\n");
  printf("\n");

  printf("\n[Process completed]\n");
  on = 0;
}

void get_registers() {
  printf(" REGISTROS: \n");
  uint64_t regsStatus[17] = {0};
  get_regist(regsStatus);
  for (int i = 0; i < 17; i++) {
    printf("\n%s %x", reg_strings[i], regsStatus[i]);
  }
  printf("\n");
}