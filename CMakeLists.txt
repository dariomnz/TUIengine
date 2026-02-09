cmake_minimum_required(VERSION 3.28)
project(TUIengine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)

add_library(TUIengine STATIC ${SOURCES})

target_include_directories(TUIengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(examples)
