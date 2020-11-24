set(SERVER_EXEC_NAME server)

add_executable(${SERVER_EXEC_NAME}
    main.cc
    server.cc
)

target_link_libraries(${SERVER_EXEC_NAME}
    game_model
    message
)

