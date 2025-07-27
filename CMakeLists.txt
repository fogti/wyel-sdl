cmake_minimum_required(VERSION 3.12)
project(wyel)
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_STUFF_DIR share/games/wyel-sdl CACHE PATH "Installation directory for game stuff like images")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_library(M_LIB m)

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/ThreadPool" "${SDL2_INCLUDE_DIR}")

add_subdirectory(images)

add_executable(wyel-sdl average.cxx config.cxx error.cxx font.cxx fs.cxx img.cxx main.cxx menu.cxx perlin_noise.cxx rand.cxx rect.cxx ship.cxx shot.cxx sprite.cxx)
target_link_libraries(wyel-sdl Threads::Threads SDL2::SDL2 SDL2_ttf::SDL2_ttf ${M_LIB})

install(TARGETS wyel-sdl DESTINATION "${INSTALL_BIN_DIR}")
