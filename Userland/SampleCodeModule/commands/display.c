#include <display.h>

int clear_cmd() {
  screen_clear();
  return 0;
}

int zoom_in_cmd() {
  zoom(1);
  return 0;
}

int zoom_out_cmd() {
  zoom(0);
  return 0;
}

int change_font_color_cmd() {
  change_color(0);
  return 0;
}

int change_bg_color_cmd() {
  change_color(1);
  return 0;
}
