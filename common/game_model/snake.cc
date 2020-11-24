#include "snake.h"

#include <iostream>

using position_t = std::pair<uint32_t, uint32_t>;

namespace common {
namespace game_model {

snake_t::snake_t(position_t startPosition, uint32_t id, std::string username)
    : _isAlive(true)
    , _id(id)
    , _username(username) {
    _body.push_front(startPosition);
    _trailPosition = _body.back();
    _currentDirection = direction_e::RIGHT;
}

void snake_t::move() {
    if (!_isAlive)
        return;

    position_t newHead(_body.front());
    switch(_currentDirection) {
        case direction_e::UP:
            --newHead.second;
            break;
        case direction_e::LEFT:
            --newHead.first;
            break;
        case direction_e::RIGHT:
            ++newHead.first;
            break;
        case direction_e::DOWN:
            ++newHead.second;
            break;
    }

    _body.emplace_front(newHead);

    auto tmpPosition = _body.back();
    _body.pop_back();

    if (_body.back() != tmpPosition)
        _trailPosition = tmpPosition;

    if (didSelfHarm()) {
        _isAlive = false;
    }
}

void snake_t::grow() {
    if (!_isAlive)
        return;

    _body.push_front(_body.front());
}

void snake_t::changeDirection(direction_e newDirection) {
    if (!_isAlive)
        return;

    //TODO:Magic
    if (static_cast<int>(_currentDirection) + static_cast<int>(newDirection) != 3) {
        _currentDirection = newDirection;
    }
}

const std::pair<uint32_t, uint32_t>& snake_t::getHeadPosition() const {
    return _body.front();
}

const std::pair<uint32_t, uint32_t>& snake_t::getTailPosition() const {
    return _body.back();
}

const std::pair<uint32_t, uint32_t>& snake_t::getTrailPosition() const {
    return _trailPosition;
}

size_t snake_t::getLength() const {
    return _body.size();
}

bool snake_t::isAlive() const {
    return _isAlive;
}

uint32_t snake_t::getId() const {
    return _id;
}

const std::string& snake_t::getName() const {
    return _username;
}

void snake_t::debugPrint() const {
    for (auto const& bodyPart : _body) {
        std::cout << "(" << bodyPart.first << ", " << bodyPart.second << ") ";
    }
    std::cout << std::endl;
}

bool snake_t::didSelfHarm() {
    int matchCounter = 0;
    const position_t& head = _body.front();
    for (auto const& bodyPart : _body) {
        if (bodyPart == head)
            ++matchCounter;
    }
    return matchCounter != 1;
}

} // ns game_model
} // ns common
