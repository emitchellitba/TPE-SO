#include <shell.h>

char input_buffer[BUFF_SIZE];
parsed_input_t parsed;

command_entry_t command_table[] = {
    {"help", help_cmd},
    {"date", print_local_date_time_cmd},
    {"zoom in", zoom_in_cmd},
    {"zoom out", zoom_out_cmd},
    {"color font", change_font_color_cmd},
    {"color background", change_bg_color_cmd},
    {"clear", clear_cmd},
    {"exit", exit_cmd},
    {"registers", get_registers_cmd},
    {"kmsg", show_kmsg_cmd},
    {"ps", show_processes_cmd},
    {"programs", show_programs_cmd},
    {"myscr", my_script_cmd},
};

#define TOTAL_CMDS (sizeof(command_table) / sizeof(command_table[0]))

static int process_input() {
  for (int i = 0; i < TOTAL_CMDS; i++) {
    if (str_cmp(input_buffer, command_table[i].name) == 0)
      return i;
  }
  return -1;
}

static void parse_input() {
  char *input = input_buffer;

  parsed.cmd = input;
  parsed.param_count = 0;

  char *dash = strchr(input, '-');
  if (dash) {
    if (dash > input && *(dash - 1) == ' ')
      *(dash - 1) = '\0';
    else
      *dash = '\0';

    char *param = dash + 1;
    while (*param && parsed.param_count < MAX_PARAMS) {
      while (*param == ' ')
        param++;
      if (!*param)
        break;

      int len = 0;
      while (*param && *param != ' ' && len < MAX_PARAM_LEN - 1) {
        parsed.params[parsed.param_count][len++] = *param++;
      }
      parsed.params[parsed.param_count][len] = '\0';
      parsed.param_count++;
    }
  }
}

static void show_prompt() { printf("user@itba:> "); }

static void show_command_not_found() {
  printf("\n");
  printf("Command not found\n");
  printf("Type 'help' for a list of available commands\n");
}

int shell_main(int argc, char *argv[]) {
  while (1) {
    show_prompt();
    get_input();
    to_lower(input_buffer);

    parse_input();

    int command_idx = process_input(parsed.cmd);
    if (command_idx != -1) {
      command_table[command_idx].func(parsed.param_count, parsed.params);
    } else {
      show_command_not_found();
    }
  }
  return 0;
}

int get_total_commands() { return TOTAL_CMDS; }

void get_input() {
  char *buffer = input_buffer;

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
