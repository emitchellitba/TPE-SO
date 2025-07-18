// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>
#include <sys/types.h>

static char *pipe_id = "testpipe";

int writer(int argc, char *argv[]);
int reader(int argc, char *argv[]);
int lonely_reader(int argc, char *argv[]);
int long_writer(int argc, char *argv[]);

int pipes_test_main(int argc, char *argv[]) {
  /* Se cargan los procesos */
  load_program("writer", (uint64_t)&writer);
  load_program("reader", (uint64_t)&reader);
  load_program("lonely_reader", (uint64_t)&lonely_reader);
  load_program("long_writer", (uint64_t)&long_writer);

  /* Se crean los pipes de lectura y escritura para reader y writer */
  int res = pipe_create(pipe_id);
  if (res < 0) {
    printf("Error creating pipe: %d\n", res);
    return 1;
  }

  int writer_fd = pipe_open(pipe_id, WRITE_PIPE);
  if (writer_fd < 0) {
    printf("Error opening pipe for writing: %d\n", writer_fd);
    pipe_close(pipe_id);
    return 1;
  }
  int reader_fd = pipe_open(pipe_id, READ_PIPE);
  if (reader_fd < 0) {
    printf("Error opening pipe for reading: %d\n", reader_fd);
    pipe_close(pipe_id);
    return 1;
  }

  char *reader_argv[] = {"reader", NULL};
  int reader_fds[] = {reader_fd, 1, 2, -1};
  int reader_pid = spawn_process_bg("reader", 1, reader_argv, reader_fds);
  if (reader_pid < 0) {
    printf("Error spawning reader process: %d\n", reader_pid);
    pipe_close(pipe_id);
    return 1;
  }

  char *writer_argv[] = {"writer", NULL};
  int writer_fds[] = {0, writer_fd, 2, -1};
  int writer_pid = spawn_process_bg("writer", 1, writer_argv, writer_fds);
  if (writer_pid < 0) {
    printf("Error spawning writer process: %d\n", writer_pid);
    pipe_close(pipe_id);
    return 1;
  }

  char *lonely_reader_argv[] = {"lonely_reader", NULL};
  int lonely_reader_pid =
      spawn_process_bg("lonely_reader", 1, lonely_reader_argv, NULL);
  if (lonely_reader_pid < 0) {
    printf("Error spawning lonely reader process: %d\n", lonely_reader_pid);
    return 1;
  }

  char *long_writer_argv[] = {"long_writer", NULL};
  int long_writer_pid =
      spawn_process_bg("long_writer", 1, long_writer_argv, NULL);
  if (long_writer_pid < 0) {
    printf("Error spawning long writer process: %d\n", long_writer_pid);
    return 1;
  }

  int status;
  if (wait_pid(writer_pid, &status) < 0) {
    printf("Error waiting for writer process\n");
  }
  if (wait_pid(reader_pid, &status) < 0) {
    printf("Error waiting for reader process\n");
  }
  if (wait_pid(lonely_reader_pid, &status) < 0) {
    printf("Error waiting for lonely reader process\n");
  }

  rm_program("writer");
  rm_program("reader");
  rm_program("lonely_reader");
  rm_program("long_writer");

  return 0;
}

int writer(int argc, char *argv[]) {
  const char *message = "Hello from process writer: \n";
  ssize_t n = write(1, message, str_len(message));
  if (n < 0) {
    printf("Error writing to pipe: %d\n", n);
    pipe_close(pipe_id);
    return 1;
  }

  usleep_time(500 * 1000);
  return 0;
}

int reader(int argc, char *argv[]) {
  char buf[64] = {0};
  ssize_t n = read(0, buf, sizeof(buf));

  const char *prefix = "Reader writing: ";
  write(1, prefix, str_len(prefix));
  if (n > 0)
    write(1, buf, n);

  return 0;
}

/** Este proceso deberia retornar debido a que no hay writers y recibe EOF */
int lonely_reader(int argc, char *argv[]) {
  int read_fd = pipe_open("lonely_reader_pipe", READ_PIPE);
  if (read_fd < 0) {
    printf("Error opening pipe for reading: %d\n", read_fd);
    return 1;
  }
  char buf[64] = {0};
  ssize_t n = read(read_fd, buf, sizeof(buf));

  const char *prefix = "\nLonely reader is writing this \n";
  write(1, prefix, str_len(prefix));
  if (n > 0)
    write(1, buf, n);

  return 0;
}

/** Este proceso deberia escribir tanto como para llenar el pipe */
int long_writer(int argc, char *argv[]) {
  int write_fd = pipe_open("long_writer_pipe", WRITE_PIPE);
  if (write_fd < 0) {
    printf("Error opening pipe for writing: %d\n", write_fd);
    return 1;
  }

  char message[1100];
  for (int i = 0; i < sizeof(message) - 1; i++) {
    message[i] = 'a';
  }

  ssize_t n = write(write_fd, message, sizeof(message));

  // Codigo no alcanzable
  const char *prefix = "\nLong writer is writing this \n";
  write(1, prefix, str_len(prefix));

  if (n < 0) {
    printf("Error writing to pipe: %d\n", n);
    pipe_close(pipe_id);
    return 1;
  }

  return 0;
}
