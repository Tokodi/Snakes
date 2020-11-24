set(LIB_NAME ui)

add_library(${LIB_NAME} STATIC
    ncurses_game_view.cc
    ui_element.cc
    utils.cc
    window.cc
)

target_link_libraries(${LIB_NAME}
    ncurses
)

target_include_directories(${LIB_NAME}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
