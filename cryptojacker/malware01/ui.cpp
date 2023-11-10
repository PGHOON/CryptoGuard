#include "ui.hpp"
#include "game.hpp"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void setWindow1(int height, int width, int x, int y) {
    WINDOW *my_win = newwin(height, width, y, x);
    refresh();

    box(my_win, 0, 0);

    print_exit();
    print_rank();
    print_explain();
    print_start();

    wrefresh(my_win);
}

void setWindow2(int height, int width, int x, int y) {
    // WINDOW *my_win = newwin(HEIGHT, WIDTH, y, x);
    WINDOW *my_win = newwin(height, width, y, x);
    refresh();

    box(my_win, 0, 0);

    wrefresh(my_win);
}

void setWindow3(const char *text, int height, int width, int start_x) {
    clear();
    setWindow2(UIHEIGHT, UIWIDTH, 0, 0);
    setWindow2(height, width, start_x, HEIGHT / 2 - 1);
    mvprintw(UIHEIGHT / 2, (UIWIDTH - strlen(text)) / 2, "%s", text);
}

int intlen(int num) {
    int len = num;
    int count = 0;
    do {
        num = int(num / 10);
        count++;
    } while (num > 0);
    return count;
}

void print_start() {
    mvprintw(UIHEIGHT / 2 - 3, (UIWIDTH - strlen("GAME START")) / 2,
             "GAME START");
}
void print_explain() {
    mvprintw(UIHEIGHT / 2 - 1, (UIWIDTH - strlen("GAME EXPLAIN")) / 2,
             "GAME EXPLAIN");
}
void print_rank() {
    mvprintw(UIHEIGHT / 2 + 1, (UIWIDTH - strlen("RANKING")) / 2, "RANKING");
}
void print_exit() {
    mvprintw(UIHEIGHT / 2 + 3, (UIWIDTH - strlen("GAME EXIT")) / 2,
             "GAME EXIT");
}
