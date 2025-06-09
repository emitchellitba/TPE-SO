#include <display.h>

#include <stdLibrary.h>

#include <libu.h>
#include <stdlib.h>

int clear_cmd() {
  screen_clear();
  return 0;
}

int zoom_cmd(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: zoom <in|out>\n");
    return -1;
  }
  if (str_cmp(argv[1], "in") == 0) {
    zoom(1);
  } else if (str_cmp(argv[1], "out") == 0) {
    zoom(0);
  } else {
    printf("Unknown zoom parameter: %s\n", argv[1]);
    return -1;
  }
  return 0;
}

int color_cmd(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: color <font|background>\n");
    return -1;
  }
  if (str_cmp(argv[1], "font") == 0) {
    change_color(0);
  } else if (str_cmp(argv[1], "background") == 0) {
    change_color(1);
  } else {
    printf("Unknown color parameter: %s\n", argv[1]);
    return -1;
  }
  return 0;
}
