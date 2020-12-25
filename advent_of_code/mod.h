#ifndef ADVENT_OF_CODE_MOD_H
#define ADVENT_OF_CODE_MOD_H

#include "glog/logging.h"

namespace advent_of_code {

template <typename T>
T PowerMod(T base, T exp, T mod) {
  T product = 1;
  T power_mult = base;
  for (T bit = 1; bit <= exp; bit <<= 1) {
    if (exp & bit) product = (product * power_mult) % mod;
    power_mult = (power_mult * power_mult) % mod;
  }
  return product;
}

template <typename T>
T InverseMod(T n, T mod) {
  // TODO(@monkeynova): Better checking if GCD as well as handling cases
  //                    where these values aren't relatively prime.
  CHECK(std::gcd(static_cast<int64_t>(n), static_cast<int64_t>(mod)) == 1);
  // If 'n' and 'mod' are relatively prime, n ** (mod - 1) == 1, which means
  // n ** -1 == n ** (mod - 2).
  return PowerMod<T>(n, mod - 2, mod);
}

}  // advent_of_code

#endif  // ADVENT_OF_CODE_MOD_H