#pragma once

#include "window.h"

#include <cstdint>
#include <memory>

#include "common/game_model/table.h"
#include "common/game_model/snake.h" // TODO: remove. only here because of direction_e

namespace client {
namespace ui {

class ncurses_view {
public:
    ncurses_view(uint32_t width, uint32_t height);

    common::game_model::direction_e getUserInputNonBlocking() const;

    void draw(const std::shared_ptr<const common::game_model::table_t> table);

    uint32_t getWidth() const { return _width; }
    uint32_t getHeight() const { return _height; }

    void show();
    void hide();

    int getchar() const;

private:
    void initialize();

private:
    const uint32_t _width;
    const uint32_t _height;

    std::unique_ptr<window> _gameWindow;
};

} // ns ui
} // ns common
