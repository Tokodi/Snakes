#include "ui_element.h"

using position_t = std::pair<uint32_t, uint32_t>;

namespace client {
namespace ui {

ui_element::ui_element(position_t position,
                       uint32_t width,
                       uint32_t height)
    : _position(position),
      _width(width),
      _height(height),
      _isVisible(false) {
    _win = newwin(_height, _width, _position.second, _position.first);
}

ui_element::~ui_element() {
    if (_win)
        delwin(_win);
}

void ui_element::refresh() {
    if (!_win)
        return;

    box(_win, 0, 0);
    wrefresh(_win);
}

uint32_t ui_element::getWidth() const {
    return _width;
}

uint32_t ui_element::getHeight() const {
    return _height;
}

} // ns ui
} // ns client
