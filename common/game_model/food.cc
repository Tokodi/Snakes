#include "food.h"

using position_t = std::pair<uint32_t, uint32_t>;

namespace common {
namespace game_model {

food_t::food_t(position_t position)
    : _position(position) {
}

const position_t& food_t::getPosition() const {
    return _position;
}

} // ns game_model
} // ns common
