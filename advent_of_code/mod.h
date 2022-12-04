#ifndef ADVENT_OF_CODE_MOD_H
#define ADVENT_OF_CODE_MOD_H

#include <numeric>

#include "absl/functional/function_ref.h"
#include "absl/log/check.h"
#include "absl/types/optional.h"
#include "absl/log/log.h"

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
  // Inverse only exists if GCD is one. This method of calculating the inverse
  // is only valid if mod is prime.
  // If 'mod' is prime, n ** (mod - 1) == 1, which means
  // n ** -1 == n ** (mod - 2).
  // TODO(@monkeynova): Better checking if GCD as well as handling cases
  //                    where these values aren't relatively prime.
  CHECK(std::gcd(static_cast<int64_t>(n), static_cast<int64_t>(mod)) == 1);
  return PowerMod<T>(n, mod - 2, mod);
}

using OnExtendedEuclidean = absl::FunctionRef<absl::optional<int64_t>(
    int64_t, int64_t, int64_t, int64_t, int64_t, int64_t)>;
absl::optional<int64_t> ExtendedEuclideanAlgorithm(
    int64_t a, int64_t b, OnExtendedEuclidean on_result);

absl::optional<int64_t> ExtendedEuclideanAlgorithmInvert(int64_t n,
                                                         int64_t mod);
absl::optional<int64_t> ChineseRemainder(int64_t mod_a, int64_t a,
                                         int64_t mod_b, int64_t b);
absl::optional<int64_t> ChineseRemainder(
    std::vector<std::pair<int64_t, int64_t>> list);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_MOD_H