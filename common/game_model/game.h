#pragma once

#include <memory>
#include <vector>

#include "food.h"
#include "snake.h"
#include "table.h"

namespace common {
namespace game_model {

class game_t {
  public:
    game_t();

    void createFood();

    const std::shared_ptr<table_t> getTable() const;
    const std::shared_ptr<food_t> getFood() const;
    std::vector<std::unique_ptr<snake_t>> const& getSnakes() const;

    std::pair<uint32_t, uint32_t> placePlayerOnTable(uint32_t id, std::string username);
    void drawFoodOnTable(std::pair<uint32_t, uint32_t> food);

  private:
    void createTable(const uint32_t width, const uint32_t height);

    std::pair<uint32_t, uint32_t> getRandomPosition() const;
    const uint32_t getRandomNumber(uint32_t min, uint32_t max) const;

  private:
    bool _isGameOver;

    std::vector<std::unique_ptr<snake_t>> _snakes;
    std::shared_ptr<table_t> _table;
    std::shared_ptr<food_t> _food;

  private:
    static constexpr uint32_t TABLE_WIDTH = 30;
    static constexpr uint32_t TABLE_HEIGHT = 30;
};

} // ns game_model
} // ns common
