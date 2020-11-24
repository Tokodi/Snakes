cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME Snakes)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# TODO: set this with cmake parameters
set(CMAKE_BUILD_TYPE Debug)

# Disabled because of protobuf
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")

include_directories(${generated_protobuf_include_path})

include_directories(.)
include_directories(net)
# Needed because of boost location on mac
include_directories(/usr/local/include/)


add_subdirectory(client)
add_subdirectory(common)
add_subdirectory(net)
add_subdirectory(server)
