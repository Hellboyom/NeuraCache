cmake_minimum_required(VERSION 3.16)

project(NeuraCache LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

execute_process(
    COMMAND xcrun --show-sdk-path
    OUTPUT_VARIABLE MACOS_SDK
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_OSX_SYSROOT "${MACOS_SDK}")

include_directories(
    "${MACOS_SDK}/usr/include/c++/v1"
)

add_executable(
    neuracache

    src/main.cpp
    src/server/server.cpp
    src/protocol/resp.cpp
    src/storage/database.cpp
    src/commands/command_handler.cpp
    src/eviction/lru_cache.cpp
    src/persistence/snapshot.cpp
    src/metrics/metrics.cpp
)

add_executable(
    test_lru

    tests/test_lru.cpp
    src/eviction/lru_cache.cpp
)

add_executable(
    test_database

    tests/test_database.cpp
    src/storage/database.cpp
    src/eviction/lru_cache.cpp
    src/persistence/snapshot.cpp
    src/metrics/metrics.cpp
)
add_executable(
    test_commands

    tests/test_commands.cpp
    src/commands/command_handler.cpp
    src/storage/database.cpp
    src/eviction/lru_cache.cpp
    src/persistence/snapshot.cpp
    src/metrics/metrics.cpp
)
add_executable(
    test_resp

    tests/test_resp.cpp
    src/protocol/resp.cpp
)
add_executable(
    test_server

    tests/test_server.cpp
)
add_executable(
    test_concurrency

    tests/test_concurrency.cpp
)
add_executable(
    test_snapshot

    tests/test_snapshot.cpp
    src/persistence/snapshot.cpp
)
add_executable(
    test_metrics

    tests/test_metrics.cpp
    src/metrics/metrics.cpp
)
enable_testing()

add_test(NAME test_lru COMMAND test_lru)
add_test(NAME test_database COMMAND test_database)
add_test(NAME test_commands COMMAND test_commands)
add_test(NAME test_resp COMMAND test_resp)
# TODO: Re-enable server integration test after fixing test harness.
# add_test(NAME test_server COMMAND test_server)
add_test(NAME test_concurrency COMMAND test_concurrency)
add_test(NAME test_snapshot COMMAND test_snapshot)
add_test(NAME test_metrics COMMAND test_metrics)