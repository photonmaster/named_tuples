cmake_minimum_required(VERSION 3.0)
PROJECT(experimental)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
set(dir .)
file(GLOB src "${dir}/*.h" "${dir}/*.cpp" "${dir}/*.hpp")
set(files ${src})
add_library(${PROJECT_NAME} ${files})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} PUBLIC .)
add_subdirectory(tests)
