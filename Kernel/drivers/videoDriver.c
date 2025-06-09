#include <videoDriver.h>

#include <fonts.h>

#define CHAR_WIDTH (8 * size)
#define CHAR_HEIGHT (16 * size)
#define CANT_COLORS 6
#define HEXA_MAX 16
#define SPECIAL_CHARACTER 6
#define SCREEN_LOG_WIDTH SCREEN_WIDTH_PIXELS / 8
#define SCREEN_LOG_HEIGHT SCREEN_HEIGHT_PIXELS / 16
#define MAX_WIDTH_LOG SCREEN_WIDTH_PIXELS / CHAR_WIDTH
#define MAX_HEIGHT_LOG SCREEN_HEIGHT_PIXELS / CHAR_HEIGHT
#define MAX_SQUARE 32

color font_color = WHITE, background_color = BLACK;
unsigned int bcolor_iterator = 1;
unsigned int fcolor_iterator = 0;
static color colors[6] = {WHITE, BLACK, RED, GREEN, BLUE, YELLOW};
unsigned int posX = 0, posY = 0, size = 1;
char screen_text_log[SCREEN_LOG_HEIGHT][SCREEN_LOG_WIDTH];
int top_left_screen = 0, currentX = 0, currentY = 0, mooving_matrix = 0;

// 48 filas y 128 cols
struct vbe_mode_info_structure {
  uint16_t
      attributes; // deprecated, only bit 7 should be of interest to you, and it
                  // indicates the mode supports a linear frame buffer.
  uint8_t window_a;     // deprecated
  uint8_t window_b;     // deprecated
  uint16_t granularity; // deprecated; used while calculating bank numbers
  uint16_t window_size;
  uint16_t segment_a;
  uint16_t segment_b;
  uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode
                         // without returning to real mode
  uint16_t pitch;        // number of bytes per horizontal line
  uint16_t width;        // width in pixels
  uint16_t height;       // height in pixels
  uint8_t w_char;        // unused...
  uint8_t y_char;        // ...
  uint8_t planes;
  uint8_t bpp;   // bits per pixel in this mode
  uint8_t banks; // deprecated; total number of banks in this mode
  uint8_t memory_model;
  uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may
                     // be 16 KB...
  uint8_t image_pages;
  uint8_t reserved0;

  uint8_t red_mask;
  uint8_t red_position;
  uint8_t green_mask;
  uint8_t green_position;
  uint8_t blue_mask;
  uint8_t blue_position;
  uint8_t reserved_mask;
  uint8_t reserved_position;
  uint8_t direct_color_attributes;

  uint32_t framebuffer; // physical address of the linear frame buffer; write
                        // here to draw to the screen
  uint32_t off_screen_mem_off;
  uint16_t off_screen_mem_size; // size of memory in the framebuffer but not
                                // being displayed on the screen
  uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

static void clear_screen_text_log() {
  currentX = 0;
  currentY = 0;
  top_left_screen = 0;
  mooving_matrix = 0;
}

static void deleteChar() {
  if (posX == 0 && posY != 0) {
    posX = SCREEN_WIDTH_PIXELS - CHAR_WIDTH;
    posY -= CHAR_HEIGHT;
    drawcharSize(' ', font_color, background_color);
    posX = SCREEN_WIDTH_PIXELS - CHAR_WIDTH;
    posY -= CHAR_HEIGHT;
  } else {
    posX -= CHAR_WIDTH;
    drawcharSize(' ', font_color, background_color);
    posX -= CHAR_WIDTH;
  }
}

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
  uint8_t *framebuffer = (uint8_t *)(uint64_t)VBE_mode_info->framebuffer;
  uint64_t offset =
      (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
  framebuffer[offset] = (hexColor)&0xFF;
  framebuffer[offset + 1] = (hexColor >> 8) & 0xFF;
  framebuffer[offset + 2] = (hexColor >> 16) & 0xFF;
}

void putPixelRGB(char R, char G, char B, uint64_t x, uint64_t y) {
  uint8_t *framebuffer = (uint8_t *)(uint64_t)VBE_mode_info->framebuffer;
  uint64_t offset =
      (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
  framebuffer[offset] = R;
  framebuffer[offset + 1] = G;
  framebuffer[offset + 2] = B;
}

void set_color(char r, char g, char b) {
  font_color = (r << 16) | (g << 8) | b;
}

int print_str(char *string, int length, int isErr) {
  if (isErr) {
    printErr(string, length);
  } else {
    printStd(string, length);
  }
  return length;
}

void printStd(const char *buffer, size_t count) { print(buffer, count, 1); }

void printErr(const char *buffer, size_t count) { print(buffer, count, 0); }

void clear_screen() {
  for (int i = 0; i < SCREEN_WIDTH_PIXELS; i++) {
    for (int j = 0; j < SCREEN_HEIGHT_PIXELS; j++) {
      putPixel(background_color, i, j);
    }
  }
  posX = posY = 0;
  clear_screen_text_log();
}

void zoom_in() {
  switch (size) {
  case 1:
    size++;
    break;
  case 2:
    size += 2;
    break;
  default:
    size = 4;
    break;
  }
  clear_screen();
}

void zoom_out() {
  switch (size) {
  case 4:
    size -= 2;
    break;
  case 2:
    size--;
    break;
  default:
    size = 1;
    break;
  }
  clear_screen();
}

void scroll_down() {
  posX = 0;
  posY = 0;
  int first = 1;
  // limpiamos la pantalla sin modificar currentX y currentY
  for (int y = top_left_screen; first || y != top_left_screen;
       y++, y %= MAX_HEIGHT_LOG) {
    if (first)
      first = 0;
    for (int x = 0; x < MAX_WIDTH_LOG; x++) {
      if (screen_text_log[y][x] == '\n') {
        break;
      }
      drawcharSize(' ', font_color, background_color);
    }
  }
  posX = 0;
  posY = 0;
  first = 1;
  for (int y = top_left_screen;
       first ||
       y != (top_left_screen ? top_left_screen - 1 : MAX_HEIGHT_LOG - 1);
       y++, y %= MAX_HEIGHT_LOG) {
    if (first)
      first = 0;
    for (int x = 0; x < MAX_WIDTH_LOG; x++) {
      if (screen_text_log[y][x] == '\n') {
        while (x < MAX_WIDTH_LOG) {
          drawcharSize(' ', font_color, background_color);
          x++;
        }
        break;
      }
      drawcharSize(screen_text_log[y][x], font_color, background_color);
    }
  }
  posY = SCREEN_HEIGHT_PIXELS - CHAR_HEIGHT;
  for (int x = 0; x < MAX_WIDTH_LOG; x++) {
    drawcharSize(' ', font_color, background_color);
  }
  posY -= CHAR_HEIGHT;
}

int can_move_top_left_pointer() { // esta funcion corrige posibles errores/bugs
  // que tenga el borrado de caracteres con
  // respecto a las posiciones de
  // top_left_screen y currentY
  return currentY == MAX_HEIGHT_LOG ||
         (mooving_matrix &&
          (abs(currentY - top_left_screen) <= 1 ||
           (currentY == MAX_HEIGHT_LOG - 1 && top_left_screen == 0)));
}

void update_screen_text_log(const char c) {
  if (currentX >= MAX_WIDTH_LOG) {
    currentX = 0;
    currentY++;
    currentY %= MAX_HEIGHT_LOG;
    if (can_move_top_left_pointer()) {
      top_left_screen++;
      top_left_screen %= MAX_HEIGHT_LOG;
    }
  }
  switch (c) {
  case '\n': {
    screen_text_log[currentY][currentX] = c;
    currentX = 0;
    currentY++;
    if (currentY >= MAX_HEIGHT_LOG) {
      currentY = 0;
      top_left_screen++;
    } else if (mooving_matrix) {
      top_left_screen++;
    }
    currentY %= MAX_HEIGHT_LOG;
    top_left_screen %=
        MAX_HEIGHT_LOG; // si se me pasó de la pantalla, lo vuelvo al principio
    break;
  }
  case '\b': {
    if (currentX == 0) {
      if (currentY == 0 && mooving_matrix) {
        currentX = MAX_WIDTH_LOG - 1;
        currentY = MAX_HEIGHT_LOG - 1;
        screen_text_log[currentY][currentX] = ' ';
      } else if (currentY != 0) {
        currentY--; // nos movemos al lugar previo al borrado
        currentX = MAX_WIDTH_LOG - 1;
        screen_text_log[currentY][currentX] =
            ' '; // borramos el útlimo caracter guardado
      }
    } else {
      currentX--;
      screen_text_log[currentY][currentX] = ' ';
    }
    break;
  }
  case '\t': {
    for (int i = 0; i < 4; i++, currentX++) {
      if (currentX >= MAX_WIDTH_LOG) {
        currentX = 0;
        currentY++;
        if (currentY >= MAX_HEIGHT_LOG) {
          currentY = 0;
          top_left_screen++;
        } else if (top_left_screen || mooving_matrix) {
          top_left_screen++;
        }
        top_left_screen %= MAX_HEIGHT_LOG;
        currentY %= MAX_HEIGHT_LOG;
      }
      screen_text_log[currentY][currentX] = ' ';
    }
    break;
  }
  default: {
    screen_text_log[currentY][currentX] = c;
    currentX++;
    break;
  }
  }
  if (top_left_screen) // una vez corres el principio de la pantalla, se va a
                       // seguir corriendo cada vez que aumente *y*
    mooving_matrix = 1;
}

int special_char(char c) { return c < SPECIAL_CHARACTER; }

void print(const char *s, size_t count, FDS stdout) {
  for (int i = 0; i < count; i++) {
    if (!special_char(*s)) {
      update_screen_text_log(*s);
      if ((*s) == '\n') {
        posY += CHAR_HEIGHT;
        posX = 0;
        if (posY > SCREEN_HEIGHT_PIXELS - CHAR_HEIGHT)
          scroll_down();
      } else if ((*s) == '\b') {
        deleteChar();
      } else if ((*s) == '\t') {
        for (int i = 0; i < 4; i++)
          drawcharSize(' ', font_color, background_color);
      } else {
        switch (stdout) {
        case 1:
          drawcharSize((*s), font_color, background_color);
          break;
        case 2:
          drawcharSize((*s), RED, background_color);
          break;
        default:
          break;
        }
      }
    }
    s++;
  }
}

void change_font_color() {
  fcolor_iterator++;
  fcolor_iterator %= CANT_COLORS;
  font_color = colors[fcolor_iterator];
  if (font_color == background_color)
    change_font_color();
  clear_screen();
}

void change_bg_color() {
  bcolor_iterator++;
  bcolor_iterator %= CANT_COLORS;
  background_color = colors[bcolor_iterator];
  if (background_color == font_color)
    change_bg_color();
  clear_screen();
}

void drawcharSize(unsigned char c, color fcolor, color bcolor) {
  // chequeo si entra el caracter, sino salto de linea
  if (posX > SCREEN_WIDTH_PIXELS - CHAR_WIDTH) {
    posX = 0;
    posY += CHAR_HEIGHT;
  }
  // Si llegamos al final de la pantalla, dejamos de dibujar
  if (posY > SCREEN_HEIGHT_PIXELS - CHAR_HEIGHT) {
    scroll_down();
  }

  int cx, cy;
  int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};
  const unsigned char *glyph = get_font() + ((int)(c - 32) * 16);

  // guardo valor original de x e y
  int x = posX;
  int y = posY;
  for (cy = 0; cy < 16; cy++) {
    for (int i = 0; i < size; i++) {
      int posX = x;
      for (cx = 0; cx < 8; cx++) {
        for (int j = 0; j < size; j++) {
          putPixel(glyph[cy] & mask[cx] ? font_color : background_color, posX,
                   posY);
          posX++;
        }
      }
      posY++;
    }
  }
  posX += CHAR_WIDTH;
  posY = y;
  if ((posX %= SCREEN_WIDTH_PIXELS) == 0)
    posY += CHAR_HEIGHT;
}
