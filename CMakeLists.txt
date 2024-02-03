cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(remote_target LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(proto)
add_subdirectory(client)
add_subdirectory(server)
