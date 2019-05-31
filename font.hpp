#ifndef FONT_HPP
# define FONT_HPP 1
# include <SDL.h>
# include <SDL_ttf.h>
# include <vector>
# include <string>
/* get_font
 * @param ptsize : font size
 * @ret            TTF_Font instance

 * get a font instance with specific size
 * searches for suitable fonts via location-list
 */
TTF_Font* get_font(const int ptsize);
SDL_Texture* CreateTextureFromText(SDL_Renderer* renderer, TTF_Font *font, const std::vector<std::string> &lines, const int max_x, const int max_y, const SDL_Color color, const unsigned int text_space, int& text_height);
#endif
