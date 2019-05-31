#ifndef RECT_HPP
# define RECT_HPP 1
# include <SDL.h>
bool operator==(const SDL_Rect& a, const SDL_Rect& b) noexcept;
bool operator!=(const SDL_Rect& a, const SDL_Rect& b) noexcept;
bool is_hit(const SDL_Rect &a, const SDL_Rect &b) noexcept;
#endif
