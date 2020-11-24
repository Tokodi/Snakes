#include "game.h"

#include <chrono>
#include <iostream>
#include <random>

using position_t = std::pair<uint32_t, uint32_t>;

namespace common {
namespace game_model {

game_t::game_t() {
    _isGameOver = false;
    createTable(TABLE_WIDTH, TABLE_HEIGHT);
}

position_t game_t::placePlayerOnTable(uint32_t id, std::string username) {
    position_t randomSnakePosition;
    do {
        randomSnakePosition = getRandomPosition();
    } while (_table->getField(randomSnakePosition) != '0');
    _snakes.push_back(std::make_unique<snake_t>(randomSnakePosition, id, username));

    try {
        _table->setField(_snakes.back()->getHeadPosition(), id);
        _table->setField(_snakes.back()->getTailPosition(), id);
    } catch (std::exception& e) {
        std::cerr << "[" << __func__ << "] Exception caught: " << e.what() << std::endl;
    }

    return randomSnakePosition;
}

const std::shared_ptr<table_t> game_t::getTable() const {
    return _table;
}

const std::shared_ptr<food_t> game_t::getFood() const {
    return _food;
}

// TODO: this looks like n ugly hack. Return iterators mb?
std::vector<std::unique_ptr<snake_t>> const& game_t::getSnakes() const {
    return _snakes;
}

void game_t::setTableField(position_t position, char fieldType) {
    try {
        _table->setField(position_t(position.first, position.second), fieldType);
    } catch (std::exception& e) {
        std::cerr << "[" << __func__ << "] Exception caught: " << e.what() << std::endl;
    }
}

void game_t::createTable(const uint32_t width, const uint32_t height) {
    _table = std::make_shared<table_t>(width, height);
}

void game_t::createFood() {
    position_t randomFoodPosition;
    do {
        randomFoodPosition = getRandomPosition();
    } while (_table->getField(randomFoodPosition) != '0');
    _food = std::make_unique<food_t>(randomFoodPosition);

    try {
        _table->setField(_food->getPosition(), 'F');
    } catch (std::exception& e) {
        std::cerr << "[" << __func__ << "] Exception caught: " << e.what() << std::endl;
    }
}

position_t game_t::getRandomPosition() const {
    const uint32_t posX = getRandomNumber(0, _table->getWidth() - 1);
    const uint32_t posY = getRandomNumber(0, _table->getHeight() - 1);

    return position_t(posX, posY);
}

const uint32_t game_t::getRandomNumber(uint32_t min, uint32_t max) const {
   static thread_local std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
   std::uniform_int_distribution<uint32_t> distribution(min, max);
   return distribution(generator);
}

} // ns game_model
} // ns common
