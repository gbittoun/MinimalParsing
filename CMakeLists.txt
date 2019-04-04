cmake_minimum_required(VERSION 2.8)

project(Parser)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "PEGTL/include/tao")
