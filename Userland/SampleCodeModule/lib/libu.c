#include <libu.h>

// DEFINE_WRAPPER(read, (char *buffer, size_t count), (buffer, count))
// DEFINE_WRAPPER(write_stdin, (const char *str, size_t count), (str, count))
// DEFINE_WRAPPER(get_date_time, (const void *dt), (dt))
// DEFINE_WRAPPER(screen_clear, (void), ())
// DEFINE_WRAPPER(change_color, (uint8_t background), (background))
// DEFINE_WRAPPER(opCodeExc, (void), ())
// DEFINE_WRAPPER(draw_sqr, (int64_t x, int64_t y, uint64_t col, uint64_t size),
// (x, y, col, size)) DEFINE_WRAPPER(sleep_time, (int64_t time), (time))
// DEFINE_WRAPPER(zoom, (int in), (in))
// DEFINE_WRAPPER(free_drawing, (int64_t x, int64_t y, int64_t **drawing, void
// *colors, int64_t size), (x, y, drawing, colors, size))
// DEFINE_WRAPPER(get_regist, (uint64_t *regs), (regs))
// DEFINE_WRAPPER(make_sound, (int64_t time, int64_t nFrequence), (time,
// nFrequence))
DEFINE_WRAPPER(read_kmsg, (char *log, size_t size), (log, size))
DEFINE_WRAPPER(get_procs, (proc_info_t * procs, size_t size), (procs, size))
DEFINE_WRAPPER(load_program, (char *name, int entry), (name, entry))
DEFINE_WRAPPER(rm_program, (char *name), (name))
DEFINE_WRAPPER(get_programs, (char **buffer, int max_count),
               (buffer, max_count))
DEFINE_WRAPPER(spawn_process, (char *name, int argc, char **argv),
               (name, argc, argv))
DEFINE_WRAPPER(proc_exit, (int code), (code))
