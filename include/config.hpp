#ifndef WYEL_CONFIG_HPP
# define WYEL_CONFIG_HPP 1
# include <string>
# include <vector>
# include <ostream>
struct wyel_config {
  std::vector<std::string> datalocs;
  int winsize[2];
  unsigned int fps[2], rps[2], max_ships, spawn_probab, noise_prec, noise_speed;
};

wyel_config get_wyel_config();
void set_wyel_config(const wyel_config &c);
void describe_wyel_config(std::ostream &out);
#endif
