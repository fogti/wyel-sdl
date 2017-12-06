#ifndef SHOT_HPP
# define SHOT_HPP 1
# include "ship.hpp"
void cleanup_shots();
void move_shots() noexcept;
void draw_shots(SDL_Renderer *target);
void fire_shot(int x, int y, direction_t);
bool is_ship_hit(const ship&) noexcept;
#endif
