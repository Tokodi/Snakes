#pragma once

#include "ui_element.h"

#include <cstdint>
#include <string>

namespace client {
namespace ui {

class window : public ui_element {
public:
    window(std::pair<uint32_t, uint32_t> position,
           uint32_t width,
           uint32_t height);

    virtual ~window() {}

    void show() override;
    void hide() override;

    void drawPixel(std::pair<uint32_t, uint32_t> position, uint32_t colorIndex);
    void printToCenter(std::string input, uint32_t row);

    int getchar() const;
};

} // ns ui
} // ns client
