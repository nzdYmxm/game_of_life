#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#ifndef bool
#define true 1
#define false 0
#define bool int
#endif

#define FIELD_WIDTH 80
#define FIELD_HEIGHT 25

#define FILL_CHAR '#'
#define EMPTY_CHAR ' '

// Временная функция для автозаполнения поля
bool random_fill_field(int **field);

// Вводит состояние всего поля через stdin. Возвращает true в случае ошибки.
bool input_field(int **field);

// Один шаг игры. Возвращает true в случае ошибки.
bool update(int **field, int **new_field);

// Отрисовывает (выводит) поле в терминал. Возвращает true в случае ошибки.
bool render(int **field);

bool start_game(int **field, int **field_2);

bool run_game_loop(int **field_1, int **field_2, int fps);

bool create_matrix(int ***matrix, int rows, int cols);


int main() {
    srand(time(NULL));
    int **field_1;
    int **field_2;

    bool error = create_matrix(&field_1, FIELD_WIDTH, FIELD_HEIGHT);

    if (!error) {
        error = create_matrix(&field_2, FIELD_WIDTH, FIELD_HEIGHT);

        if (!error) {
            error = start_game(field_1, field_2);
            free(field_2);
        }
        free(field_1);
    }

    if (error)
        printf("Error has occured.");

    return 0;
}

bool render(int **field) {
    for (int y = 0; y < 20; y++)
        printf("\n");

    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            char symb;

            if (field[x][y])
                symb = FILL_CHAR;
            else
                symb = EMPTY_CHAR;

            printf("%c", symb);
        }
        printf("|\n");
    }

    fflush(stdout);
    return false;   // No errors
}

int get_neighbours_count(int **field, int x, int y) {
    int count = 0;

    for (int rel_x = -1; rel_x <= 1; rel_x++)
        for (int rel_y = -1; rel_y <= 1; rel_y++)
            if (rel_x != 0 || rel_y != 0) {
                int real_x = (x + rel_x + FIELD_WIDTH) % FIELD_WIDTH;
                int real_y = (y + rel_y + FIELD_HEIGHT) % FIELD_HEIGHT;

                if (field[real_x][real_y])
                    count++;
            }

    return count;
}

bool update(int **field, int **new_field) {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            int is_alive = field[x][y];
            int neighbours = get_neighbours_count(field, x, y);

            if (is_alive) {
                if (neighbours != 2 && neighbours != 3)
                    is_alive = false;
            } else {
                if (neighbours == 3)
                    is_alive = true;
            }

            new_field[x][y] = is_alive;
        }
    }
    return false;   // No errors
}

bool random_fill_field(int **field) {
    for (int y = 0; y < FIELD_HEIGHT; y++)
        for (int x = 0; x < FIELD_WIDTH; x++)
            field[x][y] = rand() & 1;

    return false;   // No errors
}


bool run_game_loop(int **field_1, int **field_2, int fps) {
    bool error = false;
    int **current_field = field_1;
    int **next_field = field_2;

    while (!error) {
        error = update(current_field, next_field);

        int **tmp = current_field;
        current_field = next_field;
        next_field = tmp;

        if (!error) {
            error = render(current_field);
            usleep(1000000 / fps);
        }
    }

    return error;
}

bool start_game(int **field_1, int **field_2) {
    bool error = false;
    int fps;
    int scan_result = scanf("%d", &fps);

    if (scan_result != 1 || fps < 1) {
        error = true;
    } else {
        error = input_field(field_1);
        // error = random_fill_field(field_1);

        if (!error)
            error = run_game_loop(field_1, field_2, fps);
    }

    return error;
}

bool create_matrix(int ***matrix, int rows, int cols) {
    bool error = false;

    (*matrix) = (int**) malloc(cols * rows * sizeof(int) + rows * sizeof(int*));
    int *ptr = (int*) ((*matrix) + rows);

    if ((*matrix) == 0) {
        error = true;
    } else {
        for (int i = 0; i < rows; i++)
            (*matrix)[i] = ptr + cols * i;
    }

    return error;
}

bool input_field(int **field) {
    bool error = false;
    for (int y = 0; y < FIELD_HEIGHT && !error; y++) {
        for (int x = 0; x < FIELD_WIDTH && !error; x++) {
            char symb;
            int result = scanf("%c", &symb);

            error = result != 1;

            if (!error) {
                if (symb == FILL_CHAR) {
                    field[x][y] = 1;
                } else if (symb == EMPTY_CHAR) {
                    field[x][y] = 0;
                } else if (symb == '\n' || symb == '|') {
                    x--;
                } else {
                    error = true;
                }
            }
        }
    }

    return error;
}
