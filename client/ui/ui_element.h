#pragma once

#include <cstdint>
#include <ncurses.h>
#include <utility>

namespace client {
namespace ui {

class ui_element {
  public:
    ui_element(std::pair<uint32_t, uint32_t> position,
               uint32_t width,
               uint32_t height);

    virtual ~ui_element();

    virtual void show() = 0;
    virtual void hide() = 0;
    void refresh();

    uint32_t getWidth() const;
    uint32_t getHeight() const;

protected:
    WINDOW* _win;

    const std::pair<uint32_t, uint32_t> _position;

    const uint32_t _width;
    const uint32_t _height;

    bool _isVisible;
};

} // ns ui
} // ns client
