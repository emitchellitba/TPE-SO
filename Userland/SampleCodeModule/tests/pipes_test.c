
#include <libu.h>
#include <stdLibrary.h>
#include <sys/types.h>

static const char *pipe_id = "testpipe";

int writer(int argc, char *argv[]);
int reader(int argc, char *argv[]);
int lonely_reader(int argc, char *argv[]);
int long_writer(int argc, char *argv[]);

int pipes_test_main(int argc, char *argv[]) {
  int res = pipe_create(pipe_id);
  if (res < 0) {
    printf("Error creating pipe: %d\n", res);
    return 1;
  }

  load_program("writer", writer);
  load_program("reader", reader);

  // Se instancia primero el writer para que se cree antes el fd de escritura y
  // no se retorne EOF inmediatamente
  char *writer_argv[] = {"writer", NULL};
  int writer_pid = spawn_process("writer", 1, writer_argv);
  if (writer_pid < 0) {
    printf("Error spawning writer process: %d\n", writer_pid);
    pipe_close(pipe_id);
    return 1;
  }

  char *reader_argv[] = {"reader", NULL};
  int reader_pid = spawn_process("reader", 1, reader_argv);
  if (reader_pid < 0) {
    printf("Error spawning reader process: %d\n", reader_pid);
    pipe_close(pipe_id);
    return 1;
  }

  load_program("lonely_reader", lonely_reader);
  char *lonely_reader_argv[] = {"lonely_reader", NULL};
  int lonely_reader_pid = spawn_process("lonely_reader", 1, lonely_reader_argv);
  if (lonely_reader_pid < 0) {
    printf("Error spawning lonely reader process: %d\n", lonely_reader_pid);
    return 1;
  }

  load_program("long_writer", long_writer);
  char *long_writer_argv[] = {"long_writer", NULL};
  int long_writer_pid = spawn_process("long_writer", 1, long_writer_argv);
  if (long_writer_pid < 0) {
    printf("Error spawning long writer process: %d\n", long_writer_pid);
    return 1;
  }

  return 0;
}

int writer(int argc, char *argv[]) {
  int write_fd = pipe_open(pipe_id, WRITE_PIPE);
  if (write_fd < 0) {
    printf("Error opening pipe for writing: %d\n", write_fd);
    return 1;
  }

  const char *message = "Hello from process writer: \n";
  ssize_t n = write(write_fd, message, str_len(message));
  if (n < 0) {
    printf("Error writing to pipe: %zd\n", n);
    pipe_close(pipe_id);
    return 1;
  }

  return 0;
}

int reader(int argc, char *argv[]) {
  int read_fd = pipe_open(pipe_id, READ_PIPE);
  if (read_fd < 0) {
    printf("Error opening pipe for reading: %d\n", read_fd);
    return 1;
  }
  char buf[64] = {0};
  ssize_t n = read(read_fd, buf, sizeof(buf));

  const char *prefix = "Hello from process reader: \n";
  write(1, prefix, str_len(prefix));
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

  const char *prefix = "Hello from lonely reader: \n";
  write(1, prefix, str_len(prefix));
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
  const char *prefix = "Hello from long writer: \n";
  write(1, prefix, str_len(prefix));

  if (n < 0) {
    printf("Error writing to pipe: %zd\n", n);
    pipe_close(pipe_id);
    return 1;
  }

  return 0;
}
