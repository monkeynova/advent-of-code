#include "advent_of_code/2020/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int64_t PowerMod(int64_t base, int64_t exp, int64_t mod) {
  int64_t product = 1;
  int64_t power_mult = base;
  for (int64_t bit = 1; bit <= exp; bit <<= 1) {
    if (exp & bit) product = (product * power_mult) % mod;
    power_mult = (power_mult * power_mult) % mod;
  }
  return product;
}

int64_t Transform(int64_t subject_number, int64_t loop_size) {
  return PowerMod(subject_number, loop_size, 20201227);
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day25_2020::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  absl::StatusOr<std::vector<int64_t>> ints = ParseAsInts(input);
  if (!ints.ok()) return ints.status();

  std::vector<int64_t> tmp(2);
  int have = 0;
  for (int i = 1; have < 2; ++i) {
    if (!tmp[0] && Transform(7, i) == (*ints)[0]) {
      ++have;
      tmp[0] = i;
    }
    if (!tmp[1] && Transform(7, i) == (*ints)[1]) {
      ++have;
      tmp[1] = i;
    }
  }
  VLOG(1) << "tmp=" << absl::StrJoin(tmp, ",");
  VLOG(1) << Transform((*ints)[0], tmp[1]);
  VLOG(1) << Transform((*ints)[1], tmp[0]);

  return IntReturn(Transform(7, tmp[0] * tmp[1]));
}

absl::StatusOr<std::vector<std::string>> Day25_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return std::vector<std::string>{"Merry Christmas"};
}

}  // namespace advent_of_code
