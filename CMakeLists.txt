cmake_minimum_required(VERSION 3.16)

project(NeuraCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    neuracache

    src/main.cpp

    src/server/server.cpp

    src/protocol/resp.cpp

    src/storage/database.cpp

    src/commands/command_handler.cpp
)