#include <shell.h>

static void execute_pipe_commands(parsed_input_t *parsed_left,
                                  parsed_input_t *parsed_right);

char input_buffer[BUFF_SIZE];

command_entry_t command_table[] = {
    {"help", help_cmd},
    {"date", print_local_date_time_cmd},
    {"zoom", zoom_cmd},
    {"color", color_cmd},
    {"clear", clear_cmd},
    {"exit", exit_cmd},
    {"registers", get_registers_cmd},
    {"kmsg", show_kmsg_cmd},
    {"ps", show_processes_cmd},
    {"programs", show_programs_cmd},
    {"test", test_cmd},
    {"sleep", sleep_cmd}
};

#define TOTAL_CMDS (sizeof(command_table) / sizeof(command_table[0]))

// --- Funciones de utilidad ---
static void show_prompt() { printf("user@itba:> "); }

static void show_command_not_found() {
  printf("\n");
  printf("Command not found\n");
  printf("Type 'help' for a list of available commands\n");
}

static int get_input() {
  int n = read(STDIN, input_buffer, BUFF_SIZE);

  if (n < 0)
    return 0;

  input_buffer[n] = '\0';

  if (input_buffer[n - 1] == '\n') {
    input_buffer[n - 1] = '\0';
  }
  return 1;
}

// --- Lógica de procesamiento ---
static int find_command_index(parsed_input_t *parsed) {
  for (int i = 0; i < TOTAL_CMDS; i++) {
    if (str_cmp(parsed->cmd, command_table[i].name) == 0)
      return i;
  }
  return -1;
}

static void parse_single_command(char *input, parsed_input_t *parsed) {
  input = trim(input);
  parsed->param_count = 0;

  // Find the first space to separate command from params
  char *space = str_chr(input, ' ');
  if (space) {
    *space = '\0';
    str_ncpy(parsed->cmd, input, MAX_PARAM_LEN - 1);
    parsed->cmd[MAX_PARAM_LEN - 1] = '\0';

    char *param = space + 1;
    param = trim(param); // Remove leading spaces

    char *token = str_tok(param, " ");
    while (token && parsed->param_count < MAX_PARAMS) {
      str_ncpy(parsed->params[parsed->param_count], token, MAX_PARAM_LEN - 1);
      parsed->params[parsed->param_count][MAX_PARAM_LEN - 1] = '\0';
      parsed->param_count++;
      token = str_tok(NULL, " ");
    }
  } else {
    str_ncpy(parsed->cmd, input, MAX_PARAM_LEN - 1);
    parsed->cmd[MAX_PARAM_LEN - 1] = '\0';
  }
}

// --- Lógica principal del Shell ---
int shell_main(int argc, char *argv[]) {
  while (1) {
    show_prompt();

    if (!get_input())
      continue;

    char *line = trim(input_buffer);
    char *pipe_pos = str_chr(line, '|');

    if (pipe_pos == NULL) {
      parsed_input_t parsed_cmd;
      parse_single_command(line, &parsed_cmd);

      if (*parsed_cmd.cmd == '\0') {
        continue;
      }

      int command_idx = find_command_index(&parsed_cmd);
      if (command_idx != -1) {
        char *argv[MAX_PARAMS];
        for (int i = 0; i < parsed_cmd.param_count; i++) {
          argv[i] = parsed_cmd.params[i];
        }
        command_table[command_idx].func(parsed_cmd.param_count, argv);
      } else {
        show_command_not_found();
      }
    } else { /* Se detecto Pipe */
      *pipe_pos = '\0';

      char *left_str = line;
      char *right_str = pipe_pos + 1;

      parsed_input_t parsed_left, parsed_right;

      parse_single_command(left_str, &parsed_left);
      parse_single_command(right_str, &parsed_right);

      printf("\nPipe detectado!\n");
      printf("  Comando Izquierda: '%s'\n", parsed_left.cmd);
      printf("  Comando Derecha:   '%s'\n", parsed_right.cmd);

      execute_pipe_commands(&parsed_left, &parsed_right);
    }
  }
  return 0;
}

int get_total_commands() { return TOTAL_CMDS; }

static void execute_pipe_commands(parsed_input_t *parsed_left,
                                  parsed_input_t *parsed_right) {
  pipe_create("pipe_cmd");

  int read_fd = pipe_open("pipe_cmd", READ_PIPE);
  int write_fd = pipe_open("pipe_cmd", WRITE_PIPE);

  char *argv_left[] = {parsed_left->cmd, NULL};
  int argc_left = 1;

  int fds_left[2] = {STDIN, write_fd};
  int pid_left = spawn_process_redirect(parsed_left->cmd, argc_left, argv_left,
                                        1, fds_left);
  if (pid_left < 0) {
    printf("Error spawning process '%s'\n", parsed_left->cmd);
    return;
  }

  char *argv_right[] = {parsed_right->cmd, NULL};
  int argc_right = 1;

  int fds_right[2] = {read_fd, STDOUT};
  int pid_right = spawn_process_redirect(parsed_right->cmd, argc_right,
                                         argv_right, 1, fds_right);
  if (pid_right < 0) {
    printf("Error spawning process '%s'\n", parsed_right->cmd);
    return;
  }

  close_fd(write_fd);
  close_fd(read_fd);

  // Wait both processes
  // waitpid(pid_left);
  // waitpid(pid_right);

  pipe_close("pipe_cmd");
}
