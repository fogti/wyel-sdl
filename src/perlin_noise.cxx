#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <Eigen/Dense>
#include "perlin_noise.hpp"

// THIS IS A DIRECT TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

// Generate a new permutation vector based on the value of seed
PerlinNoise::PerlinNoise(unsigned int seed) {
  p.resize(256);

  // Fill p with values from 0 to 255
  std::iota(p.begin(), p.end(), 0);

  // Initialize a random engine with seed
  std::default_random_engine engine(seed);

  // Suffle  using the above random engine
  std::shuffle(p.begin(), p.end(), engine);

  // Duplicate the permutation vector
  p.insert(p.end(), p.begin(), p.end());
}

static double lerp(double t, double a, double b) noexcept
  { return a + t * (b - a); }

static double insign(const int x, const double y) noexcept
  { return x ? (-y) : y; }

static double grad(int hash, double x, double y, double z) noexcept {
  int h = hash & 15;
  // Convert lower 4 bits of hash into 12 gradient directions
  double u = h < 8 ? x : y,
         v = h < 4 ? y : (h == 12 || h == 14) ? x : z;
  return insign(h & 1, u) + insign(h & 2, v);
}

double PerlinNoise::noise(double x_, double y_, double z_) const noexcept {
  Eigen::Array<double, 3, 4> PC;
  // Insert numbers
  PC.col(0) << x_, y_, z_;
  // Calculate floor
  PC.col(1) = PC.col(0).unaryExpr(std::ptr_fun(floor));
  // Find relative x, y, z of point in cube
  PC.col(2) = PC.col(0) - PC.col(1);
  // Compute fade curves for each of x, y, z
  PC.col(3) = PC.col(2).unaryExpr(
    [](double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
  );

  // Find the unit cube that contains the point
  Eigen::Array<int, 3, 1> UC = PC.col(1).unaryExpr(
    [](double t) -> int { return static_cast<int>(floor(t)) & 255; }
  );
  const int X = UC(0, 0), Y = UC(1, 0), Z = UC(2, 0);
  const auto dpx = [this](const int n) -> int { return p[n]; };

  const double
    x = PC(0, 2), y = PC(1, 2), z = PC(2, 2),
    u = PC(0, 3), v = PC(1, 3), w = PC(2, 3);

  // Hash coordinates of the 8 cube corners
  // matrix: row 1 = A; row 2 = B;
  //  col 1 = -x; col 2 = -A; col 3 = -B;
  Eigen::Array<int, 2, 3> HC;
  auto HCL = HC.col(0);
  HCL << X, (X + 1);
  HCL = HCL.unaryExpr(dpx) + Y;
  const int A = HCL(0), B = HCL(1);
  HC.col(1) = HCL;
  HC.col(2) = HCL + 1;

  typedef Eigen::Array<int, 2, 2> RHC_t;
  auto HCR = HC.rightCols(2);
  HCR = HCR.unaryExpr(dpx) + Z;

  const auto lerpvu = [&](const int offset) {
    const RHC_t RHC = HCR.unaryExpr([this, offset](const int n) -> int { return p[n + offset]; });
    const double oz = z - offset;
    return lerp(v,
      lerp(u, grad(RHC(0, 0), x, y  , oz), grad(RHC(1, 0), x-1, y  , oz)),
      lerp(u, grad(RHC(0, 1), x, y-1, oz), grad(RHC(1, 1), x-1, y-1, oz)));
  };

  // Add blended results from 8 corners of cube
  return (lerp(w, lerpvu(0), lerpvu(1)) + 1.0)/2.0;
}
