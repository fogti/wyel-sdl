#include <stdlib.h>
#include <SDL_filesystem.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "config.hpp"
#include "fs.hpp"

using namespace std;

wyel_config get_wyel_config() {
  wyel_config ret;
  ret.winsize[0] = 640;
  ret.winsize[1] = 480;
  ret.fps[0] = 30;
  ret.fps[1] = 100;
  ret.rps[0] = 100;
  ret.rps[1] = 500;
  ret.max_ships = 2;
  ret.spawn_probab = 10;
  ret.noise_prec = 5;
  ret.noise_speed = 1000;

  const string myhome = get_wyel_home();
  ifstream cfgf(myhome + "/config");

  if(cfgf) {
    string line;
    vector<string> parts;
    while(getline(cfgf, line)) {
      parts.clear();
      {
        istringstream ss(line);
        string pn;
        while(getline(ss, pn, ' ')) parts.push_back(pn);
      }

      if(parts.empty() || parts[0] == "#") continue;

      const string param = parts.front();
      parts.erase(parts.begin());

      bool valid, param_known;
      {
        const unordered_map<string, short> known_params = {
          { "dataloc"    , -1 },
          { "fps"         , 2 },
          { "rps"         , 2 },
          { "max_ships"   , 1 },
          { "spawn_probab", 1 },
          { "noise"       , 2 },
          { "winsize"     , 2 },
        };
        const auto it = known_params.find(param);

        if((valid = param_known = (it != known_params.end()))) {
          valid = (it->second == -1)
                  ? (!parts.empty())
                  : (parts.size() == static_cast<size_t>(it->second));
        }
      }
      if(valid) {
        try {
          if(param == "dataloc") {
            ret.datalocs.insert(ret.datalocs.end(), parts.begin(), parts.end());
          } else if(param == "fps") {
            ret.fps[0] = stoi(parts[0]);
            ret.fps[1] = stoi(parts[1]);
            valid = ret.fps[0] && ret.fps[1];
          } else if(param == "rps") {
            ret.rps[0] = stoi(parts[0]);
            ret.rps[1] = stoi(parts[1]);
            valid = ret.rps[0] && ret.rps[1];
          } else if(param == "max_ships") {
            ret.max_ships = stoi(parts[0]);
            valid = ret.max_ships;
          } else if(param == "spawn_probab") {
            ret.spawn_probab = stoi(parts[0]);
            valid = ret.spawn_probab;
          } else if(param == "noise") {
            ret.noise_prec = stoi(parts[0]);
            ret.noise_speed = stoi(parts[1]);
            valid = ret.noise_prec && ret.noise_speed;
          } else if(param == "winsize") {
            ret.winsize[0] = stoi(parts[0]);
            ret.winsize[1] = stoi(parts[1]);
            valid = ret.winsize[0] && ret.winsize[1];
          }
        } catch(...) {
          // do nothing
        }
      } else if(!param_known)
        clog << "wyel-sdl-config: WARNING: unknown param '" << param << "'\n";
      else
        clog << "wyel-sdl-config: " << param << ": WARNING: wrong arguments\n";
    }
  }

  ret.datalocs.push_back(myhome);
  ret.datalocs.push_back(get_wyel_static_home());
  ret.datalocs.push_back("/usr/share/games/wyel-sdl");

  return ret;
}

void set_wyel_config(const wyel_config& cfg) {
  ofstream cfgf(get_wyel_home() + "/config");
  if(cfgf) {
    {
      const vector<string> twdl(cfg.datalocs.begin(), cfg.datalocs.end() - 3);
      if(!twdl.empty()) {
        cfgf << "datalocs";
        for(auto &&i : twdl) cfgf << ' ' << i;
        cfgf << '\n';
      }
    }

    cfgf << "fps " << cfg.fps[0] << ' ' << cfg.fps[1] << "\n"
            "rps " << cfg.rps[0] << ' ' << cfg.rps[1] << "\n"
            "max_ships " << cfg.max_ships << "\n"
            "spawn_probab " << cfg.spawn_probab << "\n"
            "noise " << cfg.noise_prec << ' ' << cfg.noise_speed << "\n"
            "winsize " << cfg.winsize[0] << ' ' << cfg.winsize[1] << "\n";
  }
}

void describe_wyel_config(ostream &out) {
  out << "-- Configuration file syntax (location: " << get_wyel_home() << "/config) --\n"
         "possible lines:\n"
         "  dataloc LOCATION...  add LOCATION to the data search path (e.g. images)\n"
         "  fps MIN MAX          set the inclusive minimum / maximum frames per second target values\n"
         "  rps MIN MAX          set the inclusive minimum / maximum rounds (moves) per second target values\n"
         "  max_ships COUNT      set the maximum number of AI ships\n"
         "  spawn_probab NUM     set the AI ship spawn probability to (1 : NUM)\n"
         "  noise PREC SPEED     set background noise parameters (PREC and SPEED are the reciprocals of the real values)\n"
         "  winsize X Y          set the start window size\n"
         "\n";
}
