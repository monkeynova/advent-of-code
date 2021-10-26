#include "advent_of_code/2017/day15/day15.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_15::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int64_t a_mult = 16807;
  int64_t b_mult = 48271;
  int64_t mod = 2147483647;
  int64_t generator_a;
  int64_t generator_b;
  for (absl::string_view row : input) {
    if (!RE2::FullMatch(row, "Generator A starts with (\\d+)", &generator_a) &&
        !RE2::FullMatch(row, "Generator B starts with (\\d+)", &generator_b)) {
      return Error("Bad line: ", row);
    }
  }
  int count = 0;
  for (int i = 0; i < 40'000'000; ++i) {
    generator_a = (generator_a * a_mult) % mod;
    generator_b = (generator_b * b_mult) % mod;
    if ((generator_a & 0xffff) == (generator_b & 0xffff)) ++count;
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2017_15::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int64_t a_mult = 16807;
  int64_t b_mult = 48271;
  int64_t mod = 2147483647;
  int64_t generator_a;
  int64_t generator_b;
  for (absl::string_view row : input) {
    if (!RE2::FullMatch(row, "Generator A starts with (\\d+)", &generator_a) &&
        !RE2::FullMatch(row, "Generator B starts with (\\d+)", &generator_b)) {
      return Error("Bad line: ", row);
    }
  }
  int count = 0;
  for (int i = 0; i < 5'000'000; ++i) {
    do {
      generator_a = (generator_a * a_mult) % mod;
    } while (generator_a % 4 != 0);
    do {
      generator_b = (generator_b * b_mult) % mod;
    } while (generator_b % 8 != 0);
    if ((generator_a & 0xffff) == (generator_b & 0xffff)) ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code
