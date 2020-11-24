#include "window.h"

using position_t = std::pair<uint32_t, uint32_t>;

namespace client {
namespace ui {

window::window(position_t position,
               uint32_t width,
               uint32_t height)
    : ui_element(position,
                 width,
                 height) {
    keypad(_win, TRUE);
}

void window::show() {
    if (!_win || _isVisible)
        return;

    refresh();
    _isVisible = true;
}

void window::hide() {
    if (!_win || !_isVisible)
        return;

    wclear(_win);
    _isVisible = false;
}

void window::drawPixel(position_t position, uint32_t colorIndex) {
    if (!_win)
        return;

    wattron(_win, COLOR_PAIR(colorIndex));
    mvwaddch(_win, position.second, position.first, 'X');
    mvwaddch(_win, position.second, position.first + 1, 'X');
    wattroff(_win, COLOR_PAIR(colorIndex));
}

void window::printToCenter(std::string input, uint32_t row) {
    int posX = _width/2 - input.length()/2;
    mvwprintw(_win, row, posX, input.c_str());
    refresh();
}

int window::getchar() const {
    return wgetch(_win);
}

} // ns ui
} // ns client
