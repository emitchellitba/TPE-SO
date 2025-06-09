// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>
#include <tests.h>

// --- TABLA DE TESTS ---

static test_entry_t test_table[] = {
    {"pipes", "pipes_test", "Prueba la creación y comunicación de pipes."},
    {"rd_wr", "rd_wr_test", "Prueba lecturas y escrituras bloqueantes."},
    {"scheduler", "sched_test",
     "Prueba la creacion y terminacion de procesos y su intercalado."},
    {"spawn", "spawn_test", "Prueba la creacion de procesos y su terminacion."},
    {"arguments", "args_test", "Prueba el paso de argumentos a los procesos."},
    {"memory", "test_mm", "Prueba la asignación y liberación de memoria."},
    {"priority", "test_prio", "Prueba el cambio de prioridad de procesos."},
    {"processes", "test_processes",
     "Prueba la creación y terminación de procesos."},
    {"sync", "test_sync", "Prueba la sincronización entre procesos."},
};

#define TOTAL_TESTS (sizeof(test_table) / sizeof(test_table[0]))

// --- FUNCIONES ---

void print_available_tests() {
  printf("\nAvailable tests (run with 'test <name>'):\n");
  for (int i = 0; i < TOTAL_TESTS; i++) {
    printf("  - %s: %s\n", test_table[i].display_name,
           test_table[i].description);
  }
}

int test_runner_cmd(int argc, char **argv) {
  if (argc < 2) {
    printf("\nUsage: test <test_name>\n");
    printf("Type 'help tests' to see a list of available tests.\n");
    return -1;
  }

  const char *display_name_to_run = argv[1];
  const char *program_to_run = NULL;

  for (int i = 0; i < TOTAL_TESTS; i++) {
    if (str_cmp(display_name_to_run, test_table[i].display_name) == 0) {
      program_to_run = test_table[i].program_name;
      break;
    }
  }

  if (program_to_run == NULL) {
    printf("Error: Test '%s' not found.\n", display_name_to_run);
    return -1;
  }

  printf("--- Running Test: %s (program: %s) ---\n", display_name_to_run,
         program_to_run);

  int test_argc = argc - 1;
  char *test_argv[test_argc + 1];
  test_argv[0] = (char *)program_to_run;
  for (int i = 2; i < argc; i++) {
    test_argv[i - 1] = argv[i];
  }
  test_argv[test_argc] = NULL;

  int pid = spawn_process(program_to_run, test_argc, test_argv, NULL);

  if (pid < 0) {
    printf("Error spawning test process '%s'\n", program_to_run);
    return -1;
  }

  int status;
  wait_pid(pid, &status);

  printf("--- Test Finished (status: %d) ---\n", status);

  return status;
}
