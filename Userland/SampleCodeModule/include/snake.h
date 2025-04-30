
#ifndef _SNAKE_H_
#define _SNAKE_H_
#include<stdint.h>

typedef struct {
    int x;
    int y;
} Segment;

typedef enum {RED_SNAKE = 0x00FF5722, BLUE_SNAKE_2 = 0x0029B6F6, GREY_SNAKE = 0x001A1F33, LIGHT_BLACK_SNAKE = 0x000D1B2A, BLACK = 0x00000000, CYAN = 0x0000FFFF, RED = 0x00FF0000, WHITE = 0xFFFFFFFF} snake_colors;

typedef enum moves{LEFT = 1, UP, DOWN, RIGHT};

void start_snake();
void game(int x);
void move_snake1(int current);
void move_snake2(int current);
void draw_back();
void draw_snake(int player);
int check_limits(int pos, int player);
void generate_apple();
void draw_apple(int x, int y);
void draw_snake_score(int n);
void restart_game();
void end_song();
extern void draw_rec(int x, int y, uint64_t color, uint64_t size);
extern void sleep_time(uint32_t t);
extern void free_drawing(int x, int y, int** drawing, uint64_t col, int size);
extern void make_sound(int ticks, int nFrequence);

#endif