#include "utils.h"

#include <ncurses.h>
#include <iostream>

namespace client {
namespace ui {

void initialize() {
    initscr();
    cbreak();
    noecho();
    timeout(-1);
    curs_set(0);
    nodelay(stdscr, FALSE);
    keypad(stdscr, TRUE);

    if (!has_colors()) {
        endwin();
        std::cout << "Your terminal does not support colors!" << std::endl;
        exit(1);
    }

    start_color();
    init_pair(COLOR_BLACK_IDX, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLOR_RED_IDX, COLOR_RED, COLOR_RED);
    init_pair(COLOR_GREEN_IDX, COLOR_GREEN, COLOR_GREEN);
    init_pair(COLOR_BLINK_IDX, COLOR_WHITE, COLOR_BLACK);
}

void terminate() {
    endwin();
}

void clear() {
    clear();
    refresh();
}

} // ns ui
} // ns client
