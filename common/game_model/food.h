#pragma once

#include <cstdint>
#include <utility>

namespace common {
namespace game_model {

class food_t {
public:
    food_t(std::pair<uint32_t, uint32_t> position);

    const std::pair<uint32_t, uint32_t>& getPosition() const;

private:
    const std::pair<uint32_t, uint32_t> _position;
};

} // ns game_model
} // ns common
