#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define LIVE 1
#define DEAD 0
#define MAX_SPEED 50000
#define MIN_SPEED 500000

void draw_field(int field[HEIGHT][WIDTH], int is_end);
int change(int last[HEIGHT][WIDTH], int curr[HEIGHT][WIDTH]);
void input(int field[HEIGHT][WIDTH]);
void copy(int curr[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]);
int get_number_of_neighbors(int field[HEIGHT][WIDTH], int x, int y);
int get_changed_coord(int i, int coord, int max_coord);
void speed(int* n, char ch);
void game(int field[HEIGHT][WIDTH]);
void pause_game();
void is_pause_or_quit(int ch);
void print_speed(int n, int entered_quit);
int is_loop(int last[HEIGHT][WIDTH], int curr[HEIGHT][WIDTH]);

int main() {
    int game_field[HEIGHT][WIDTH];
    game(game_field);
    return 0;
}

void game(int field[HEIGHT][WIDTH]) {
    int last_field[HEIGHT][WIDTH];
    copy(field, last_field);
    int sp = 100000;
    input(field);
    if (freopen("/dev/tty", "r", stdin)) initscr();
    noecho();
    curs_set(0);
    int flag = 1;
    int iterations = 0;
    while (flag) {
        clear();
        timeout(0);
        int ch = getch();
        if (ch == 'q') flag = 0;
        speed(&sp, ch);
        draw_field(field, !flag);
        print_speed(sp, !flag);
        if (iterations % 2 == 0) copy(field, last_field);
        if (!change(last_field, field)) flag = 0;
        if (ch == ' ') pause_game();
        refresh();
        usleep(10);
        iterations++;
    }
    clear();
    draw_field(field, !flag);
    refresh();
    usleep(-1);
    endwin();
}

void pause_game() {
    printw("GAME PAUSED. Press any key to continue\n");
    timeout(-1);
    getch();
    printw(" ");
}

void draw_field(int field[HEIGHT][WIDTH], int is_end) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if ((i == HEIGHT - 1 && j == WIDTH - 1) || (i == 0 && j == 0) || (i == 0 && j == WIDTH - 1) ||
                (i == HEIGHT - 1 && j == 0))
                printw("+");
            else if (j == WIDTH - 1 || j == 0)
                printw("|");
            else if (i == HEIGHT - 1 || i == 0)
                printw("-");
            else if (field[i][j] == LIVE)
                printw("*");
            else
                printw(" ");
        }
        printw("\n");
    }
    if (is_end) printw("END\n");
}

void print_speed(int n, int is_end) {
    if (is_end != 1) {
        if (n <= MAX_SPEED)
            printw("max speed!\n");
        else if (n >= MIN_SPEED)
            printw("min speed!\n");
        else
            printw("current speed: %d (less - faster)\n", n);
    } else
        printw("END\n");
}

int get_number_of_neighbors(int field[HEIGHT][WIDTH], int x, int y) {
    int count = 0;
    count += field[get_changed_coord(1, x, HEIGHT)][get_changed_coord(1, y, WIDTH)] == LIVE ? 1 : 0;
    count += field[get_changed_coord(-1, x, HEIGHT)][get_changed_coord(1, y, WIDTH)] == LIVE ? 1 : 0;
    count += field[get_changed_coord(1, x, HEIGHT)][get_changed_coord(-1, y, WIDTH)] == LIVE ? 1 : 0;
    count += field[get_changed_coord(-1, x, HEIGHT)][get_changed_coord(-1, y, WIDTH)] == LIVE ? 1 : 0;
    count += field[get_changed_coord(1, x, HEIGHT)][y] == LIVE ? 1 : 0;
    count += field[get_changed_coord(-1, x, HEIGHT)][y] == LIVE ? 1 : 0;
    count += field[x][get_changed_coord(1, y, WIDTH)] == LIVE ? 1 : 0;
    count += field[x][get_changed_coord(-1, y, WIDTH)] == LIVE ? 1 : 0;
    return count;
}

int get_changed_coord(int i, int coord, int max_coord) { return (i + coord + max_coord) % max_coord; }

int change(int last[HEIGHT][WIDTH], int curr[HEIGHT][WIDTH]) {
    int next[HEIGHT][WIDTH];
    copy(curr, next);

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int n = get_number_of_neighbors(next, i, j);
            if (n == 3)
                curr[i][j] = LIVE;
            else if (next[i][j] == LIVE && (n < 2 || n > 3))
                curr[i][j] = DEAD;
        }
    }
    return !is_loop(last, curr);
}

void copy(int curr[HEIGHT][WIDTH], int next[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            next[i][j] = curr[i][j];
        }
    }
}

void input(int field[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            scanf("%d", &field[i][j]);
        }
    }
}

void speed(int* n, char ch) {
    switch (ch) {
        case '2':
            *n = (*n - 10000) >= MAX_SPEED ? *n - 10000 : *n + 0;
            break;
        case '1':
            *n = (*n + 10000) <= MIN_SPEED ? *n + 10000 : *n - 0;
            break;
        default:
            *n += 0;
    }
}

int is_loop(int last[HEIGHT][WIDTH], int curr[HEIGHT][WIDTH]) {
    int flag = 1;
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            if (last[i][j] != curr[i][j]) flag = 0;
    return flag;
}
