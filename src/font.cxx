#include "font.hpp"
#include "fs.hpp"

TTF_Font* get_font(const int ptsize) {
  static const std::string fpf = "/usr/share/fonts/";
  static const std::string stpf = get_wyel_static_home();
  static const char* const fonts[] = {
    "liberation-fonts/LiberationMono-Regular.ttf",
    "source-pro/SourceCodePro-Regular.otf",
    "dejavu/DejaVuSansMono.ttf",
    "croscorefonts/Cousine-Regular.ttf",
    "droid/DroidSansMono.ttf",
    "freefont/FreeMono.ttf",
    "corefonts/cour.ttf"
  };

  TTF_Font* ret = 0;
  for(auto &&i : fonts) {
    for(auto &&pf : {stpf, fpf}) {
      ret = TTF_OpenFont((pf + i).c_str(), ptsize);
      if(ret) break;
    }
    if(ret) break;
  }

  return ret;
}

SDL_Texture* CreateTextureFromText(SDL_Renderer* renderer, TTF_Font *font, const std::vector<std::string> &lines, const int max_x, const int max_y, const SDL_Color color, const unsigned int text_space, int& text_height) {
  SDL_Surface *surface = SDL_CreateRGBSurface(0, max_x, max_y, 32, 0, 0, 0, 0);
  SDL_Rect my_rect;
  my_rect.x = 0;
  my_rect.y = 0;

  for(auto &&i : lines) {
    SDL_Surface *txt_surface = TTF_RenderText_Solid(font, i.c_str(), color);
    my_rect.w = txt_surface->w;
    my_rect.h = txt_surface->h;
    SDL_BlitSurface(txt_surface, 0, surface, &my_rect);
    SDL_FreeSurface(txt_surface);
    my_rect.y += my_rect.h;
    my_rect.y += text_space;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  text_height = my_rect.h;

  return texture;
}
