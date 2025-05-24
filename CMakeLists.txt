cmake_minimum_required(VERSION 3.5)
project(RPGGame)

set(CMAKE_CXX_STANDARD 11)

find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

add_executable(RPGGame
    main.cpp
    GameEngine.cpp
    Hero.cpp
    Enemy.cpp
    grotte.cpp
    Weapon.cpp
    DBManager.cpp
)

target_link_libraries(RPGGame ${SQLite3_LIBRARIES})

