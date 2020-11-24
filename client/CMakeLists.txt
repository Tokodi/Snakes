set(CLIENT_EXEC_NAME client)

add_executable(${CLIENT_EXEC_NAME}
    client.cc
    main.cc
)

target_link_libraries(${CLIENT_EXEC_NAME}
    game_model
    message
    ui
)

include_directories(.)

add_subdirectory(ui)
