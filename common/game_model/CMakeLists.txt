set(LIB_NAME game_model)

add_library(${LIB_NAME} STATIC
    food.cc
    game.cc
    snake.cc
    table.cc
)

target_include_directories(${LIB_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
