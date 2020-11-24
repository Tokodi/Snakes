#include "table.h"

#include <iostream>

using position_t = std::pair<uint32_t, uint32_t>;

namespace common {
namespace game_model {

table_t::table_t(const uint32_t width, const uint32_t height) : _width(width), _height(height) {
    _table = std::make_unique<std::vector<std::unique_ptr<std::vector<char>>>>(_height);
    for (auto& row : *_table) {
        row = std::make_unique<std::vector<char>>(_width, '0');
    }
}

uint32_t table_t::getWidth() const {
    return _width;
}

uint32_t table_t::getHeight() const {
    return _height;
}

//Note: Might throw exception
void table_t::setField(position_t position, char fieldType) {
    _table->at(position.second)->at(position.first) = fieldType;
}

//Note: Might throw exception
char table_t::getField(position_t position) const {
    return _table->at(position.second)->at(position.first);
}

bool table_t::isInside(position_t position) const {
    if (static_cast<unsigned>(position.first) < _width) {
        if (static_cast<unsigned>(position.second) < _height) {
            return true;
        }
    }
    return false;
}

void table_t::debugPrint() const {
    for (auto const& row : *_table) {
        for (char const& field : *row) {
            std::cout << field << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

} // ns game_model
} // ns common
