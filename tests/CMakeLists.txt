cmake_minimum_required(VERSION 3.0)
project (unit_test)
add_executable(${PROJECT_NAME} unit.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ..)
