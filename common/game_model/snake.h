#pragma once

#include <deque>
#include <string>

namespace common {
namespace game_model {

enum class direction_e { UP, LEFT, RIGHT, DOWN };

class snake_t {
  public:
    snake_t(std::pair<uint32_t, uint32_t> startPosition, uint32_t id, std::string username);

    void move();
    void grow();
    void changeDirection(direction_e newDirection);

    const std::pair<uint32_t, uint32_t>& getHeadPosition() const;
    const std::pair<uint32_t, uint32_t>& getTailPosition() const;
    const std::pair<uint32_t, uint32_t>& getTrailPosition() const;

    size_t getLength() const;
    bool isAlive() const;

    uint32_t getId() const;
    const std::string& getName() const;

    void debugPrint() const;

  private:
    bool didSelfHarm();

  private:
    direction_e _currentDirection;
    bool _isAlive;

    const uint32_t _id;
    const std::string _username;

    std::deque<std::pair<uint32_t, uint32_t>> _body;
    std::pair<uint32_t, uint32_t> _trailPosition;
};

} // ns game_model
} // ns common
