cmake_minimum_required(VERSION 3.7)
project(wyel)
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_STUFF_DIR share/games/wyel-sdl CACHE PATH "Installation directory for game stuff like images")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_library(M_LIB m)

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/ThreadPool" "${SDL2_INCLUDE_DIR}" "${EIGEN3_INCLUDE_DIR}")

add_subdirectory(images)
add_subdirectory(src)
