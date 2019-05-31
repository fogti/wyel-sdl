#include <SDL_thread.h>
#include <vector>
#include <algorithm>

#include <img.hpp>
#include <shot.hpp>

using namespace std;

struct shot : public sprite {
  shot(int x, int y, direction_t _d);
  void notify_destroyed() noexcept;
};

shot::shot(int x, int y, direction_t _d): sprite(x, y, wyel_images.i_shot, _d) { }

void shot::notify_destroyed() noexcept {
  sprite::notify_destroyed();
  set_image(wyel_images.i_shot_destroyed);
}

static vector<shot> shots;

void move_shots() noexcept {
  for(auto it = shots.begin(); it != shots.end();) {
    auto &i = *it;
    if(!i.valid()) {
      it = shots.erase(it);
      continue;
    }
    if(!i.destroyed())
      for(auto it2 = shots.begin(); it != it2; ++it2) {
        auto &j = *it2;
        if(!j.destroyed() && is_hit(i, j)) {
          i.notify_destroyed();
          j.notify_destroyed();
          break;
        }
      }

    if(!i.destroyed())
      i.move(i.d);
    ++it;
  }
}

void draw_shots(SDL_Renderer *target) {
  for(auto it = shots.begin(); it != shots.end();) {
    it->draw(target);
    if(it->destroyed())
      it = shots.erase(it);
    else
      ++it;
  }
}

void reserve_shots(const size_t asc) {
  shots.reserve(shots.size() + asc);
}

void fire_shot(int x, int y, direction_t d) {
  shots.emplace_back(x, y, d);
}

[[gnu::hot]]
bool is_ship_hit(const ship &s) noexcept {
  for(auto &i : shots)
    if(!i.destroyed() && is_hit(s, i)) {
      i.notify_destroyed();
      return true;
    }

  return false;
}
