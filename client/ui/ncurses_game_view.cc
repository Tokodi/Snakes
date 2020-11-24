#include "ncurses_game_view.h"
#include "utils.h"

#include <string>

using position_t = std::pair<uint32_t, uint32_t>;

namespace client {
namespace ui {

ncurses_view::ncurses_view(uint32_t width, uint32_t height)
    : _width(width)
    , _height(height) {
    initialize();
}

common::game_model::direction_e ncurses_view::getUserInputNonBlocking() const {
    int key = getch();
    flushinp();
    switch (key) {
        case KEY_LEFT:
            return common::game_model::direction_e::LEFT;
        case KEY_RIGHT:
            return common::game_model::direction_e::RIGHT;
        case KEY_UP:
            return common::game_model::direction_e::UP;
        case KEY_DOWN:
            return common::game_model::direction_e::DOWN;
        case 'a':
            return common::game_model::direction_e::LEFT;
        case 'd':
            return common::game_model::direction_e::RIGHT;
        case 'w':
            return common::game_model::direction_e::UP;
        case 's':
            return common::game_model::direction_e::DOWN;
        default:
            throw; // TODO
    }
}

void ncurses_view::draw(const std::shared_ptr<const common::game_model::table_t> table) {
    for (unsigned int x = 0; x < table->getWidth(); ++x) {
        for (unsigned int y = 0; y < table->getHeight(); ++y) {
            switch (table->getField(position_t(x, y))) {
                case 'F':
                    _gameWindow->drawPixel(position_t(x + x + 1, y + 1), COLOR_BLACK_IDX);
                    break;
                case 0:
                    _gameWindow->drawPixel(position_t(x + x + 1, y + 1), COLOR_RED_IDX);
                    break;
                default:
                    // TODO: Different color for different ids
                    _gameWindow->drawPixel(position_t(x + x + 1, y + 1), COLOR_GREEN_IDX);
                    break;
            }
        }
    }
    _gameWindow->refresh();
}

void ncurses_view::show() {
    _gameWindow->show();
}

void ncurses_view::hide() {
    _gameWindow->hide();
}

void ncurses_view::initialize() {
    int posX = COLS/2 - _width - 1;
    int posY = LINES/2 - _height/2 - 1;
    _gameWindow = std::make_unique<window>(position_t(posX, posY), 2 * _width + 2, _height + 2);
}

} // ns ui
} // ns client
