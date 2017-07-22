cmake_minimum_required(VERSION 3.7)
project(wyel)
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_STUFF_DIR share/games/wyel-sdl CACHE PATH "Installation directory for game stuff like images")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_library(M_LIB m)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${SDL2_INCLUDE_DIR}")

add_subdirectory(images)

add_executable(wyel-sdl average.cxx config.cxx error.cxx font.cxx fs.cxx img.cxx lock.cxx main.cxx menu.cxx perlin_noise.cxx pos_guard.cxx rand.cxx rect.cxx ship.cxx shot.cxx sprite.cxx)
target_link_libraries(wyel-sdl ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${M_LIB})

install(TARGETS wyel-sdl DESTINATION "${INSTALL_BIN_DIR}")
