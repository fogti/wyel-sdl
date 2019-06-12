#pragma once
#include "ship.hpp"
void move_shots() noexcept;
void draw_shots(SDL_Renderer *target);
void reserve_shots(const size_t asc);
void fire_shot(int x, int y, direction_t);
bool is_ship_hit(const ship&) noexcept;
