// source: https://github.com/sol-prog/Perlin_Noise

// THIS CLASS IS A TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

#ifndef PERLINNOISE_HPP
# define PERLINNOISE_HPP 1
# include <vector>
class PerlinNoise {
  // The permutation vector
  std::vector<int> p;
 public:
  // Generate a new permutation vector based on the value of seed
  explicit PerlinNoise(unsigned int seed);
  // Get a noise value, for 2D images z can have any value
  double noise(double x, double y, double z) const noexcept;
 private:
  double fade(double t) const noexcept;
  double lerp(double t, double a, double b) const noexcept;
  double grad(int hash, double x, double y, double z) const noexcept;
};
#endif
