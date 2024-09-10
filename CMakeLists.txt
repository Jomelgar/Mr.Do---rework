cmake_minimum_required (VERSION 3.0)
project (Mr.Do)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Game)
include_directories(${CMAKE_SOURCE_DIR}/TextConsole)
include_directories(${CMAKE_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME}  TextConsole/TextConsole.cpp
                                Game/MyGame.cpp
                                Game/Main.cpp
                                cmake/Map.hpp  
              )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} -lpthread)
target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)