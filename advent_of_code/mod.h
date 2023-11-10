#ifndef ADVENT_OF_CODE_MOD_H
#define ADVENT_OF_CODE_MOD_H

#include <numeric>
#include <optional>
#include <vector>

#include "absl/functional/function_ref.h"
#include "absl/log/check.h"
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

std::optional<int64_t> InverseMod(int64_t n, int64_t mod);

using OnExtendedEuclidean = absl::FunctionRef<std::optional<int64_t>(
    int64_t, int64_t, int64_t, int64_t, int64_t, int64_t)>;
std::optional<int64_t> ExtendedEuclideanAlgorithm(
    int64_t a, int64_t b, OnExtendedEuclidean on_result);

std::optional<int64_t> ExtendedEuclideanAlgorithmInvert(int64_t n, int64_t mod);
std::optional<int64_t> ChineseRemainder(int64_t mod_a, int64_t a, int64_t mod_b,
                                        int64_t b);
std::optional<int64_t> ChineseRemainder(
    std::vector<std::pair<int64_t, int64_t>> list);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_MOD_H