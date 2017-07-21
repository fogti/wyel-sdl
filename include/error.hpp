#ifndef ERROR_HPP
# define ERROR_HPP 1
# include <string>
void errmsg(const std::string &msg);
void sdl_errmsg(const std::string &fn);
void ttf_errmsg(const std::string &fn);
#endif
