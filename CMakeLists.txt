cmake_minimum_required(VERSION 3.10)
project(SUgame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    main.cpp
    Hero.cpp
    Enemy.cpp
    Gameengine.cpp
    grotte.cpp
)


add_executable(SUgame ${SOURCES})

set_target_properties(SUgame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

