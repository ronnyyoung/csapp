cmake_minimum_required(VERSION 2.8)
project(csapp C CXX)

#SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99")

add_subdirectory(ch10_unix_io)
add_subdirectory(ch11_network)
add_subdirectory(ch12_concurrency)