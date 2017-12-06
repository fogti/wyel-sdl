#include <SDL_thread.h>
#include <vector>
#include <algorithm>

#include <img.hpp>
#include <shot.hpp>

using namespace std;

struct shot : public sprite {
  shot(int x, int y, direction_t _d);
  void notify_destroyed() noexcept;
  shot& operator++() noexcept;
};

shot::shot(int x, int y, direction_t _d): sprite(x, y, wyel_images.i_shot, _d) { }

void shot::notify_destroyed() noexcept {
  sprite::notify_destroyed();
  set_image(wyel_images.i_shot_destroyed);
}

shot& shot::operator++() noexcept {
  move(d);
  return *this;
}

static vector<shot> shots;

void cleanup_shots() {
  shots.erase(
    remove_if(shots.begin(), shots.end(), [](const shot &s) noexcept -> bool {
      return !s.valid();
    }),
    shots.end());

  for(auto &i : shots)
    if(!i.destroyed())
      for(auto &j : shots)
        if(&i != &j && !j.destroyed() && is_hit(i, j)) {
          i.notify_destroyed();
          j.notify_destroyed();
        }
}

void move_shots() noexcept {
  for(auto &i : shots) ++i;
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

void fire_shot(int x, int y, direction_t d) {
  shots.emplace_back(x, y, d);
}

bool is_ship_hit(const ship &s) noexcept {
  for(auto &i : shots)
    if(!i.destroyed() && is_hit(s, i)) {
      i.notify_destroyed();
      return true;
    }

  return false;
}
