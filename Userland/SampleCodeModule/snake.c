#include <drawings.h>
#include <snake.h>
#include <stdint.h>
#include <stdLibrary.h>

#define MAX_LENGHT 660
#define MAX_COL 30
#define MAX_ROWS 22
#define RELATIVE_POSITION 32
#define OFFSET 32
#define LINE_WIDTH 2
#define SQUARE_SIZE 28
#define SNAKE_1 1
#define SNAKE_2 2
#define APPLE 3

char speed = 0;
int win = 0;
int win1 = 0;
int win2 = 0;
int draw = 0;
int score1 = 0;
int score2 = 0;
int lenght1 = 3;
int lenght2 = 3;
int current_direction1 = 0;
int current_direction2 = 0;
unsigned int gameIsRunning = 0;
int color_back1 = 0;
int color_back2 = 0;
int table[MAX_ROWS][MAX_COL] = {0};
Segment snake1[MAX_LENGHT] = {0};
Segment snake2[MAX_LENGHT] = {0};

void start_snake(){
    int cont = 1;
    char players;
    char level;
    clear();   
    printf("Bienvenido a Snake\n");
    while (cont){
    printf("\n");
    printf("Elija la cantidad de jugadores:\n");
    printf("\n");
    printf(" (1) Un jugador\n");  
    printf(" (2) Dos jugadores\n");
    printf("\n");
    do {
        players = get_char();
    }while (players != '1' && players != '2');
    printf("Seleccione la dificultad:\n");
    printf("\n");
    printf(" (1) Easy\n");
    printf(" (2) Medium\n");
    printf(" (3) Hard\n");
    printf("\n");
    do {
        level = get_char();
    } while(level != '1' && level != '2' && level != '3');
    if (level == '1'){
        speed = 3;
    } else if (level == '2'){
        speed = 2;
    } else {
        speed = 1;
    }
    if (players == '1'){
        clear();
        gameIsRunning = 1;
        game(1);
    } else{
        clear();
        gameIsRunning = 1;
        game(2);
    }
    printf("\n");
    printf("Desea jugar de nuevo? (y/n)\n");

    do {
        cont = get_char();
    } while (cont != 'y' && cont != 'n');
    if (cont == 'n'){
        cont = 0;
    } else {
        cont = 1;
    }

    clear();

    }

}


void game(int x){
    if (x == 1){
        int position = 0;
        inicio_snake();
        while (gameIsRunning){
            position = get_entry();
            sleep_time(speed);
            if (position == 0){
                position = current_direction1;
            }
            move_snake1(position);
            draw_snake_position(current_direction1, 1);
            draw_snake_score(1);
        }
        end_song();
    } else {
        int pos = 0;
        int position1 = 0;
        int position2 = 0;
        inicio_snake2();
        while (gameIsRunning){
            pos = get_entry();
            sleep_time(speed);
            if (pos == 0){
                position1 = current_direction1;
                position2 = current_direction2;
            }
            if ((pos != 'a') && (pos != 'w') && (pos != 's') && (pos != 'd')){
                position1 = current_direction1;
            } else {
                position1 = pos;
                if (current_direction2 == 0){
                    position2 = LEFT;
                }
            }
            if (pos != LEFT && pos != UP && pos != DOWN && pos != RIGHT){
                if (current_direction2 != 0){
                    position2 = current_direction2;
                }
            } else {
                position2 = pos;
                if (current_direction1 == 0){
                    position1 = 'd';
                }
            }
            move_snake1(position1);
            move_snake2(position2);
            draw_snake_position(current_direction1, 1);
            draw_snake_position(current_direction2, 2);
            draw_snake_score(1);
            draw_snake_score(2);
        }
        end_song();
    }

    clear();

    check_winner(x);

    restart_game();

}   


void inicio_snake(){
    snake1[0].x = 3;
    snake1[0].y = 11;
    table[11][3] = SNAKE_1;
    snake1[1].x = 4;
    snake1[1].y = 11;
    table[11][4] = SNAKE_1;
    snake1[lenght1 - 1].x = 5;
    snake1[lenght1 - 1].y = 11;
    table[11][5] = SNAKE_1;
    
    draw_back();
    draw_snake(1);
    generate_apple();
}

void inicio_snake2(){
    snake1[0].x = 3;
    snake1[0].y = 11;
    table[11][3] = SNAKE_1;
    snake1[1].x = 4;
    snake1[1].y = 11;
    table[11][4] = SNAKE_1;
    snake1[lenght1 - 1].x = 5;
    snake1[lenght1 - 1].y = 11;
    table[11][5] = SNAKE_1;
    
    snake2[0].x = 26;
    snake2[0].y = 11;
    table[11][26] = SNAKE_2;
    snake2[1].x = 25;
    snake2[1].y = 11;
    table[11][25] = SNAKE_2;
    snake2[lenght2 - 1].x = 24;
    snake2[lenght2 - 1].y = 11;
    table[11][24] = SNAKE_2;
    
    draw_back();
    draw_snake(1);
    draw_snake(2);
    generate_apple();

}

void end_song() {
    make_sound(2, 523);
    make_sound(2, 392);
    make_sound(2, 370);
    make_sound(2, 349);
    make_sound(2, 329);
    make_sound(2, 293);
    make_sound(2, 261);
}

void draw_back(){
    int oddColumn = 0;
	for(int x = 0; x < MAX_COL; x++){
		for (int y = 0; y < MAX_ROWS; y++){
			if (oddColumn % 2 == 0){
				draw_sqr(x * RELATIVE_POSITION + OFFSET + LINE_WIDTH, y * RELATIVE_POSITION + OFFSET + LINE_WIDTH,  GREY_SNAKE, SQUARE_SIZE);
				y++;
				draw_sqr(x * RELATIVE_POSITION + OFFSET + LINE_WIDTH, y * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
			}else{
				draw_sqr(x * RELATIVE_POSITION + OFFSET + LINE_WIDTH, y * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
				y++;
				draw_sqr(x * RELATIVE_POSITION + OFFSET + LINE_WIDTH, y * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
			}
		}
		oddColumn++;
	}
}

void draw_snake(int player){
    if(player == 1){
        for (int i = 0; i < lenght1; i++){
            draw_sqr((snake1[i].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[i].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, RED_SNAKE, SQUARE_SIZE);
        }
    } else {
        for (int i = 0; i < lenght2; i++){
            draw_sqr((snake2[i].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[i].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, BLUE_SNAKE_2, SQUARE_SIZE);
        }
    }
}

void move_snake1(int current){
    switch (current)
    {
    case 'a':
    case 'A':
        if ((current_direction1 != 'd' && current_direction1 != 'D') && (current_direction1 != 0)){
            current_direction1 = 'a';
        } else if (current_direction1 == 0){
            current_direction1 = 'd';
        }
        break;
    case 'w':
    case 'W':
        if (current_direction1 != 's' && current_direction1 != 'S'){
            current_direction1 = 'w';
        }
        break;
    case 's':
    case 'S':
        if (current_direction1 != 'w' && current_direction1 != 'W'){
            current_direction1 = 's';
            
        }
        break;
    case 'd':
    case 'D':
        if (current_direction1 != 'a' && current_direction1 != 'A'){
            current_direction1 = 'd';
        }
        break;
    default:
        break;
    }

}


void move_snake2(int current){
    switch (current)
    {
    case LEFT:
        if (current_direction2 != RIGHT){
            current_direction2 = LEFT;
        }
        break;
    case UP:
        if (current_direction2 != DOWN){
            current_direction2 = UP;
        }
        break;
    case DOWN:
        if (current_direction2 != UP){
            current_direction2 = DOWN;
        }
        break;
    case RIGHT:
        if (current_direction2 != LEFT && (current_direction2 != 0)){
            current_direction2 = RIGHT;
        } else if (current_direction2 == 0){
            current_direction2 = LEFT;
        }
        break;
    default:
        break;
    }
}

int check_limits(int pos, int player){

    switch (pos)
    {
    case LEFT:
        if(player == 1){
            if (snake1[lenght1 - 1].x - 1 < 0 || table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x - 1] == SNAKE_1 || table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x - 1] == SNAKE_2){
                if (snake1[lenght1 - 1].x - 1 == snake2[lenght2 -1].x && snake1[lenght1 - 1].y == snake2[lenght2 - 1].y){
                    draw = 1;
                } else {
                    win2 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        } else {
            if (snake2[lenght2 - 1].x - 1 < 0 || table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x - 1] == SNAKE_2 || table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x - 1] == SNAKE_1){
                if ((snake2[lenght2 - 1].x - 1 == snake1[lenght1 - 1].x) && (snake2[lenght2 - 1].y == snake1[lenght1 - 1].y)){
                    draw = 1;
                } else {
                    win1 = 1; 
                }
                gameIsRunning = 0;
                return 0;
            }
        }
        break;
    case UP:
        if(player == 1){
            if (snake1[lenght1 - 1].y - 1 < 0 || table[snake1[lenght1 - 1].y - 1][snake1[lenght1 - 1].x] == SNAKE_1 || table[snake1[lenght1 - 1].y - 1][snake1[lenght1 - 1].x] == SNAKE_2){
                if (snake1[lenght1 - 1].x == snake2[lenght2 -1].x && snake1[lenght1 - 1].y - 1 == snake2[lenght2 - 1].y){
                    draw = 1;
                } else {
                    win2 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        } else {
            if (snake2[lenght2 - 1].y - 1 < 0 || table[snake2[lenght2 - 1].y - 1][snake2[lenght2 - 1].x] == SNAKE_2 || table[snake2[lenght2 - 1].y - 1][snake2[lenght2 - 1].x] == SNAKE_1){
                if(snake2[lenght2 - 1].x == snake1[lenght1 -1].x && snake2[lenght2 - 1].y - 1 == snake1[lenght1 - 1].y){
                    draw = 1;
                } else {
                    win1 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        }
        break;
    case DOWN:
        if(player == 1){
            if (snake1[lenght1 - 1].y + 1 >= MAX_ROWS || table[snake1[lenght1 - 1].y + 1][snake1[lenght1 - 1].x] == SNAKE_1 || table[snake1[lenght1 - 1].y + 1][snake1[lenght1 - 1].x] == SNAKE_2){
                if (snake1[lenght1 - 1].x == snake2[lenght2 -1].x && snake1[lenght1 - 1].y + 1 == snake2[lenght2 - 1].y){
                    draw = 1;
                } else {
                    win2 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        } else {
            if (snake2[lenght2 - 1].y + 1 >= MAX_ROWS || table[snake2[lenght2 - 1].y + 1][snake2[lenght2 - 1].x] == SNAKE_2 || table[snake2[lenght2 - 1].y + 1][snake2[lenght2 - 1].x] == SNAKE_1){
                if (snake2[lenght2 - 1].x == snake1[lenght1 -1].x && snake2[lenght2 - 1].y + 1 == snake1[lenght1 - 1].y){
                    draw = 1;
                } else {
                    win1 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        }
        break;
    case RIGHT:
        if(player == 1){
            if (snake1[lenght1 - 1].x + 1 >= MAX_COL || table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x + 1] == SNAKE_1 || table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x + 1] == SNAKE_2){
                if (snake1[lenght1 - 1].x + 1 == snake2[lenght2 -1].x && snake1[lenght1 - 1].y == snake2[lenght2 - 1].y){
                    draw = 1;
                } else {
                    win2 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        } else {
            if (snake2[lenght2 - 1].x + 1 >= MAX_COL || table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x + 1] == SNAKE_2 || table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x + 1] == SNAKE_1){
                if (snake2[lenght2 - 1].x + 1 == snake1[lenght1 -1].x && snake2[lenght2 - 1].y == snake1[lenght1 - 1].y){
                    draw = 1;
                } else {
                    win1 = 1;
                }
                gameIsRunning = 0;
                return 0;
            }
        }
        break;
    default:
        break;
    }

    return gameIsRunning;

}


void draw_snake_position(int pos, int player){
    int crecio = 0;
    int headX = 0;
    int headY = 0; 
    switch (pos){
    case 'a':
    case LEFT: {
        if(player == 1){
            headX = snake1[lenght1 - 1].x;
            headY = snake1[lenght1 - 1].y;
            if (check_limits(LEFT, 1)){
                if (table[headY][headX - 1] == APPLE){
                    score1++;
                    lenght1++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }

                if (color_back1 % 2 == 0  && crecio == 0){
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                } else if (crecio == 0){ 
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                }

                if (crecio == 0){
                for (int i = 0; i < lenght1 - 1; i++){
                    snake1[i].x = snake1[i + 1].x;
                    snake1[i].y = snake1[i + 1].y;
                }
                }

                snake1[lenght1 - 1].x = headX - 1;
                snake1[lenght1 - 1].y = headY;
                
                draw_sqr((snake1[lenght1 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[lenght1 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, RED_SNAKE, SQUARE_SIZE);
                table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x] = SNAKE_1;
                if (crecio == 1){
                    crecio = 0;
                }
            }
        } else {
            headX = snake2[lenght2 - 1].x;
            headY = snake2[lenght2 - 1].y;
            if (check_limits(LEFT, 2)){
                if (table[headY][headX - 1] == APPLE){
                    score2++;
                    lenght2++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back2 % 2 == 0  && crecio == 0){
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                } else if (crecio == 0){ 
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                }
                if (crecio == 0){
                for (int i = 0; i < lenght2 - 1; i++){
                    snake2[i].x = snake2[i + 1].x;
                    snake2[i].y = snake2[i + 1].y;
                }
                }
                snake2[lenght2 - 1].x = headX - 1;
                snake2[lenght2 - 1].y = headY;
                draw_sqr((snake2[lenght2 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[lenght2 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, BLUE_SNAKE_2, SQUARE_SIZE);
                table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x] = SNAKE_2;
                if (crecio == 1){
                    crecio = 0;
                }
            }
        }
        break;
    }
    case 'w':
    case UP:
        if(player == 1){
            headX = snake1[lenght1 - 1].x;
            headY = snake1[lenght1 - 1].y;
            if (check_limits(UP, 1)){
                if (table[headY - 1][headX] == APPLE){
                    score1++;
                    lenght1++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back1 % 2 == 0 && crecio == 0){
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                } else if (crecio == 0){ 
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                }
                if (crecio == 0){                   
                for (int i = 0; i < lenght1 - 1; i++){
                    snake1[i].x = snake1[i + 1].x;
                    snake1[i].y = snake1[i + 1].y;
                }
                }
                snake1[lenght1 - 1].x = headX;
                snake1[lenght1 - 1].y = headY - 1;
                draw_sqr((snake1[lenght1 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[lenght1 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, RED_SNAKE, SQUARE_SIZE);
                table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x] = SNAKE_1;
                if(crecio == 1){
                    crecio = 0;
                }
            }
        } else {
            headX = snake2[lenght2 - 1].x;
            headY = snake2[lenght2 - 1].y;
            if (check_limits(UP, 2)){
                if (table[headY - 1][headX] == APPLE){
                    score2++;
                    lenght2++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back2 % 2 == 0 && crecio == 0){
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                } else if (crecio == 0){ 
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                }
                if (crecio == 0){             
                for (int i = 0; i < lenght2 - 1; i++){
                    snake2[i].x = snake2[i + 1].x;
                    snake2[i].y = snake2[i + 1].y;
                }
                }
                snake2[lenght2 - 1].x = headX;
                snake2[lenght2 - 1].y = headY - 1;
                draw_sqr((snake2[lenght2 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[lenght2 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, BLUE_SNAKE_2, SQUARE_SIZE);
                table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x] = SNAKE_2;
                if(crecio == 1){
                    crecio = 0;
                }
            }
        }
        break;
    case 's':
    case DOWN:
        if(player == 1){
            headX = snake1[lenght1 - 1].x;
            headY = snake1[lenght1 - 1].y;
            if (check_limits(DOWN, 1)){
                if (table[headY + 1][headX] == APPLE){
                    score1++;
                    lenght1++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back1 % 2 == 0 && crecio == 0){
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                } else if(crecio == 0){ 
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                }
                if (crecio == 0){                   
                for (int i = 0; i < lenght1 - 1; i++){
                    snake1[i].x = snake1[i + 1].x;
                    snake1[i].y = snake1[i + 1].y;
                }
                }
                snake1[lenght1 - 1].x = headX;
                snake1[lenght1 - 1].y = headY + 1;
                draw_sqr((snake1[lenght1 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[lenght1 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, RED_SNAKE, SQUARE_SIZE);
                table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x] = SNAKE_1;
                if(crecio == 1){
                    crecio = 0;
                }
            }
        } else {
            headX = snake2[lenght2 - 1].x;
            headY = snake2[lenght2 - 1].y;
            if (check_limits(DOWN, 2)){
                if (table[headY + 1][headX] == APPLE){
                    score2++;
                    lenght2++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back2 % 2 == 0 && crecio == 0){
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                } else if(crecio == 0){ 
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                }
                if (crecio == 0){                   
                for (int i = 0; i < lenght2 - 1; i++){
                    snake2[i].x = snake2[i + 1].x;
                    snake2[i].y = snake2[i + 1].y;
                }
                }
                snake2[lenght2 - 1].x = headX;
                snake2[lenght2 - 1].y = headY + 1;
                draw_sqr((snake2[lenght2 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[lenght2 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, BLUE_SNAKE_2, SQUARE_SIZE);
                table[snake2[lenght2- 1].y][snake2[lenght2 - 1].x] = SNAKE_2;
                if(crecio == 1){
                    crecio = 0;
                }
            }
        }
        break;
    case 'd':
    case RIGHT:
        if (player == 1){
            headX = snake1[lenght1 - 1].x;
            headY = snake1[lenght1 - 1].y;
            if (check_limits(RIGHT, 1)){
                if (table[headY][headX + 1] == APPLE){
                    score1++;
                    lenght1++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back1 % 2 == 0 && crecio == 0){
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                } else if (crecio == 0){ 
                    draw_sqr((snake1[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake1[0].y][snake1[0].x] = 0;
                    color_back1++;
                }   
                if (crecio == 0){              
                for (int i = 0; i < lenght1 - 1; i++){
                    snake1[i].x = snake1[i + 1].x;
                    snake1[i].y = snake1[i + 1].y;
                }
                }
                snake1[lenght1 - 1].x = headX + 1;
                snake1[lenght1 - 1].y = headY;
                draw_sqr((snake1[lenght1 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake1[lenght1 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, RED_SNAKE, SQUARE_SIZE);
                table[snake1[lenght1 - 1].y][snake1[lenght1 - 1].x] = SNAKE_1;
                if (crecio == 1){
                    crecio = 0;
                }  
            }
        } else {
            headX = snake2[lenght2 - 1].x;
            headY = snake2[lenght2 - 1].y;
            if (check_limits(RIGHT, 2)){
                if (table[headY][headX + 1] == APPLE){
                    score2++;
                    lenght2++;
                    crecio = 1;
                    generate_apple();
                    make_sound(1, 130);
                }
                if (color_back2 % 2 == 0 && crecio == 0){
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, LIGHT_BLACK_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                } else if (crecio == 0){ 
                    draw_sqr((snake2[0].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[0].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, GREY_SNAKE, SQUARE_SIZE);
                    table[snake2[0].y][snake2[0].x] = 0;
                    color_back2++;
                }
                if (crecio == 0){               
                for (int i = 0; i < lenght2 - 1; i++){
                    snake2[i].x = snake2[i + 1].x;
                    snake2[i].y = snake2[i + 1].y;
                }
                }
                snake2[lenght2 - 1].x = headX + 1;
                snake2[lenght2 - 1].y = headY;
                draw_sqr((snake2[lenght2 - 1].x) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, (snake2[lenght2 - 1].y) * RELATIVE_POSITION + OFFSET + LINE_WIDTH, BLUE_SNAKE_2, SQUARE_SIZE);
                table[snake2[lenght2 - 1].y][snake2[lenght2 - 1].x] = SNAKE_2;
                if (crecio == 1){
                    crecio = 0;
                }  
            }
        }
        break;
    default:
        break;
    }
      
}   

void check_winner(int x){
    printf("Fin del juego!\n");
    if (x == 1){
        if (win == 1){
            printf("\nFelicidades, ganaste!\n");
        } else {
            printf("\nPerdiste, mejor suerte la proxima!\n");
        }
    } else {
        if (win1 == 1){
            printf("\nGanador: Jugador 1\n");
        } else if (win2 == 1){
            printf("\nGanador: Jugador 2\n");
        } else if (draw == 1){
            printf("\nEmpate\n");
        }
    }
}

void generate_apple() {
    int x, y;
    do {
        x = get_random() % MAX_COL;
        y = get_random() % MAX_ROWS;
    } while(table[y][x] == SNAKE_1 || table[y][x] == SNAKE_2);
    draw_apple(x, y);
    table[y][x] = APPLE;
}

void draw_apple(int x, int y) {
    // me fijo el color del cuadrado donde estoy parado usando paridades
    snake_colors background_color;
    if(y % 2 == 0) {
        if(x % 2 == 0) {
            background_color = GREY_SNAKE;
        } else {
            background_color = LIGHT_BLACK_SNAKE;
        }
    } else {
        if(x % 2 == 0) {
            background_color = LIGHT_BLACK_SNAKE;
        } else {
            background_color = GREY_SNAKE;
        }
    }
    apple_colors[0] = background_color;
    free_drawing(x * RELATIVE_POSITION + OFFSET + LINE_WIDTH, y * RELATIVE_POSITION + OFFSET + LINE_WIDTH, apple, apple_colors, SQUARE_SIZE);
}

void restart_game(){
    win = 0;
    win1 = 0;
    win2 = 0;
    draw = 0;   
    current_direction1 = 0;
    current_direction2 = 0;
    color_back1 = 0;
    color_back2 = 0;
    for (int i = 0; i < MAX_ROWS; i++){
        for (int j = 0; j < MAX_COL; j++){
            table[i][j] = 0;
        }
    }
    for (int i = 0; i < MAX_LENGHT; i++){
        snake1[i].x = 0;
        snake1[i].y = 0;
        snake2[i].x = 0;
        snake2[i].y = 0;
    }
    score1 = 0;
    score2 = 0;
    lenght1 = 3;
    lenght2 = 3;
}

void draw_score(int x, int y, int score) {
    switch(score) {
        case 0:
            free_drawing(x, y, zero, number_colors, SQUARE_SIZE);
            break;
        case 1:
            free_drawing(x, y, one, number_colors, SQUARE_SIZE);
            break;
        case 2:
            free_drawing(x, y, two, number_colors, SQUARE_SIZE);
            break;
        case 3:
            free_drawing(x, y, three, number_colors, SQUARE_SIZE);
            break;
        case 4:
            free_drawing(x, y, four, number_colors, SQUARE_SIZE);
            break;
        case 5:
            free_drawing(x, y, five, number_colors, SQUARE_SIZE);
            break;
        case 6:
            free_drawing(x, y, six, number_colors, SQUARE_SIZE);
            break;
        case 7:
            free_drawing(x, y, seven, number_colors, SQUARE_SIZE);
            break;
        case 8:
            free_drawing(x, y, eight, number_colors, SQUARE_SIZE);
            break;
        case 9:
            free_drawing(x, y, nine, number_colors, SQUARE_SIZE);
            break;
    }
}

void draw_snake_score(int n) {
    if(n == 1) {
        apple_colors[0] = BLACK;
        apple_colors[1] = RED;
        apple_colors[2] = RED_SNAKE;
        free_drawing(1 * OFFSET + LINE_WIDTH, LINE_WIDTH, apple, apple_colors, SQUARE_SIZE);

        int score = lenght1 - 3;
        for(int i = 0; i < 3; i++, score /= 10) {
              draw_score((4-i) * OFFSET + LINE_WIDTH, LINE_WIDTH, score % 10);
        }

    } else {
        apple_colors[1] = CYAN;
        apple_colors[2] = BLUE_SNAKE_2;
        free_drawing(MAX_COL * OFFSET + LINE_WIDTH, LINE_WIDTH, apple, apple_colors, SQUARE_SIZE);
        apple_colors[1] = RED;
        apple_colors[2] = RED_SNAKE;

        int score = lenght2 - 3;
        for(int  i = 0; i < 3; i++, score /= 10) {
            draw_score(MAX_COL * OFFSET + LINE_WIDTH - (i + 1) * OFFSET, LINE_WIDTH, score % 10);
        } 
    }
}