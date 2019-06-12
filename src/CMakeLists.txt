cmake_minimum_required(VERSION 3.7)

add_executable(wyel-sdl average.cxx config.cxx error.cxx font.cxx fs.cxx img.cxx main.cxx menu.cxx perlin_noise.cxx rand.cxx rect.cxx ship.cxx shot.cxx sprite.cxx)
target_link_libraries(wyel-sdl Threads::Threads ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${M_LIB})

install(TARGETS wyel-sdl DESTINATION "${INSTALL_BIN_DIR}")
