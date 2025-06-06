
#include <libu.h>
#include <stdLibrary.h>
#include <sys/types.h>

int process_b(int argc, char *argv[]);

int pipes_test_main(int argc, char *argv[]) {
  const char *pipe_id = "testpipe";

  // Crea y escribe en el pipe
  int res = pipe_create(pipe_id);
  if (res < 0) {
    printf("Error creating pipe: %d\n", res);
    return 1;
  }

  int write_fd = pipe_open(pipe_id, 1); // 4
  if (write_fd < 0) {
    printf("Error opening pipe for writing: %d\n", write_fd);
    return 1;
  }

  const char *msg = "Hola desde el proceso main\n";
  write(write_fd, msg, str_len(msg));

  // load_program("process_b", (uint64_t)process_b);

  // spawn_process("process_b", 0, NULL);

  int write_fd2 = pipe_open(pipe_id, 1); // 5

  int read_fd = pipe_open(pipe_id, 0); // 6

  if (read_fd < 0 || write_fd2 < 0) {
    return 1;
  }

  dup2_fd(read_fd, write_fd2); // Queremos copiar el fd de lectura al fd de
                               // escritura (el 6 al 5)

  close_fd(read_fd); // Cerramos el fd original (6)

  return 0;
}

int process_b(int argc, char *argv[]) {
  const char *pipe_id = "testpipe";

  int read_fd = pipe_open(pipe_id, 0);
  if (read_fd < 0) {
    printf("Error opening pipe for reading: %d\n", read_fd);
    return 1;
  }
  char buf[64] = {0};
  ssize_t n = read(read_fd, buf, sizeof(buf));

  const char *prefix = "Hello from process B: \n";
  write(1, prefix, str_len(prefix));
  write(1, buf, n);

  pipe_close(pipe_id);

  return 0;
}
