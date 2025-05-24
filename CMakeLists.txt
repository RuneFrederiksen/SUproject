cmake_minimum_required(VERSION 3.10)
project(SUgame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SQLite3
find_package(SQLite3 REQUIRED)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Kildefiler 
set(SOURCES
    main.cpp
    Hero.cpp
    Enemy.cpp
    GameEngine.cpp   
    Grotte.cpp
    Shop.cpp
    Database.cpp     
)

add_executable(SUgame ${SOURCES})

# Link mod SQLite
target_link_libraries(SUgame PRIVATE SQLite::SQLite3)

set_target_properties(SUgame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

