#include <shell.h>

#include <tests.h>

static void execute_pipe_commands(parsed_input_t *parsed_left,
                                  parsed_input_t *parsed_right);

char input_buffer[BUFF_SIZE];

command_entry_t command_table[] = {
    // --- Comandos Built-in ---
    {"help", CMD_BUILTIN, .data.func = help_cmd},
    {"echo", CMD_BUILTIN, .data.func = echo_cmd},
    {"date", CMD_BUILTIN, .data.func = print_local_date_time_cmd},
    {"zoom", CMD_BUILTIN, .data.func = zoom_cmd},
    {"color", CMD_BUILTIN, .data.func = color_cmd},
    {"clear", CMD_BUILTIN, .data.func = clear_cmd},
    {"registers", CMD_BUILTIN, .data.func = get_registers_cmd},
    {"kill", CMD_BUILTIN, .data.func = kill_cmd},
    {"test", CMD_BUILTIN, .data.func = test_runner_cmd},

    // --- Comandos Externos (Programas) ---
    {"ps", CMD_SPAWN, .data.program_name = "ps"},
    {"programs", CMD_SPAWN, .data.program_name = "ls"},
    {"cat", CMD_SPAWN, .data.program_name = "cat"},
    {"loop", CMD_SPAWN, .data.program_name = "loop"},
    {"chprio", CMD_SPAWN, .data.program_name = "chprio"},
    {"block", CMD_SPAWN, .data.program_name = "block"},
    {"lazy", CMD_SPAWN, .data.program_name = "lazy"},
    {"sleep", CMD_SPAWN, .data.program_name = "sleep"},
};

#define TOTAL_CMDS (sizeof(command_table) / sizeof(command_table[0]))

static void execute_command(command_entry_t *entry, int argc, char **argv,
                            int is_background) {
  switch (entry->type) {
  case CMD_BUILTIN:
    entry->data.func(argc, argv);
    break;

  case CMD_SPAWN: {
    if (!is_background) {
      int pid = spawn_process(entry->data.program_name, argc, argv, NULL);

      if (pid < 0) {
        printf("Error starting process '%s'\n", entry->data.program_name);
        return;
      }

      int status;
      wait_pid(pid, &status);
    } else {
      int pid = spawn_process_bg(entry->data.program_name, argc, argv, NULL);

      if (pid < 0) {
        printf("Error starting process '%s'\n", entry->data.program_name);
        return;
      }

      printf("Process started in background with PID: %d\n", pid);
    }
    break;
  }
  }
}

// --- Funciones de utilidad ---
static void show_prompt() { printf("user@itba:> "); }

static void show_command_not_found() {
  printf("\n");
  printf("Command not found\n");
  printf("Type 'help' for a list of available commands\n");
}

static int get_input() {
  int n = read(STDIN, input_buffer, BUFF_SIZE);

  if (n < 0) {
    printf("Error reading input\n");
    while (1)
      ;
  }

  if (n == 0) { // EOF
    printf("\n");
    return 0;
  }

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

static void parse_command(char *input, parsed_input_t *parsed) {
  parsed->param_count = 0;
  parsed->cmd[0] = '\0';

  input = trim(input);

  if (*input == '\0') {
    return;
  }

  char *token_start = input;
  while (*input != ' ' && *input != '\0') {
    input++;
  }

  if (*input != '\0') {
    *input = '\0';
    input++;
  }

  str_ncpy(parsed->cmd, token_start, MAX_PARAM_LEN - 1);
  parsed->cmd[MAX_PARAM_LEN - 1] = '\0';

  while (*input != '\0' && parsed->param_count < MAX_PARAMS) {

    while (*input == ' ') {
      input++;
    }

    if (*input == '\0') {
      break;
    }

    token_start = input;
    while (*input != ' ' && *input != '\0') {
      input++;
    }

    if (*input != '\0') {
      *input = '\0';
      input++;
    }

    str_ncpy(parsed->params[parsed->param_count], token_start,
             MAX_PARAM_LEN - 1);
    parsed->params[parsed->param_count][MAX_PARAM_LEN - 1] = '\0';
    parsed->param_count++;
  }
}

// --- Lógica principal del Shell ---
int shell_main(int argc, char *argv[]) {
  while (1) {
    show_prompt();

    if (!get_input())
      continue;

    char *line = trim(input_buffer);
    int is_background = 0;

    int len = str_len(line);
    if (len > 0 && line[len - 1] == '&') {
      is_background = 1;
      line[len - 1] = '\0';
      line = trim(line);
    }

    char *pipe_pos = str_chr(line, '|');

    if (pipe_pos == NULL) {
      parsed_input_t parsed_cmd;
      parse_command(line, &parsed_cmd);

      if (*parsed_cmd.cmd == '\0') {
        continue;
      }

      int command_idx = find_command_index(&parsed_cmd);
      if (command_idx != -1) { /* Comando encontrado */
        char *final_argv[MAX_PARAMS + 2];

        final_argv[0] = parsed_cmd.cmd;

        for (int i = 0; i < parsed_cmd.param_count; i++) {
          final_argv[i + 1] = parsed_cmd.params[i];
        }

        final_argv[parsed_cmd.param_count + 1] = NULL;

        int final_argc = parsed_cmd.param_count + 1;

        execute_command(&command_table[command_idx], final_argc, final_argv,
                        is_background);
      } else { /* Comando no encontrado */
        show_command_not_found();
      }
    } else { /* Se detecto Pipe */
      *pipe_pos = '\0';

      char *left_str = line;
      char *right_str = pipe_pos + 1;

      parsed_input_t parsed_left, parsed_right;
      parse_command(left_str, &parsed_left);
      parse_command(right_str, &parsed_right);

      execute_pipe_commands(&parsed_left, &parsed_right);
    }
  }
  return 0;
}

static void execute_pipe_commands(parsed_input_t *parsed_left,
                                  parsed_input_t *parsed_right) {
  pipe_create("pipe_cmd");

  int read_fd = pipe_open("pipe_cmd", READ_PIPE);
  int write_fd = pipe_open("pipe_cmd", WRITE_PIPE);

  char *argv_left[] = {parsed_left->cmd, NULL};
  int argc_left = 1;

  int fds_left[3] = {STDIN, write_fd, -1};
  int pid_left =
      spawn_process(parsed_left->cmd, argc_left, argv_left, fds_left);
  if (pid_left < 0) {
    printf("Error spawning process '%s'\n", parsed_left->cmd);
    return;
  }

  char *argv_right[] = {parsed_right->cmd, NULL};
  int argc_right = 1;

  int fds_right[2] = {read_fd, STDOUT, -1};
  int pid_right =
      spawn_process_bg(parsed_right->cmd, argc_right, argv_right, fds_right);
  if (pid_right < 0) {
    printf("Error spawning process '%s'\n", parsed_right->cmd);
    return;
  }

  close_fd(write_fd);
  close_fd(read_fd);

  wait_pid(pid_left, NULL);
  wait_pid(pid_right, NULL);

  pipe_close("pipe_cmd");
}

int get_total_commands() { return TOTAL_CMDS; }
