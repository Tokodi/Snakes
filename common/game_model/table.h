#pragma once

#include <memory>
#include <vector>

namespace common {
namespace game_model {

class table_t {
  public:
    table_t(const uint32_t width, const uint32_t height);

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    void setField(std::pair<uint32_t, uint32_t> position, char fieldType);
    char getField(std::pair<uint32_t, uint32_t> position) const;

    bool isInside(std::pair<uint32_t, uint32_t> position) const;

    void debugPrint() const;

  private:
    const uint32_t _width;
    const uint32_t _height;

    std::unique_ptr<std::vector<std::unique_ptr<std::vector<char>>>> _table;
};

} // ns game_model
} // ns common
