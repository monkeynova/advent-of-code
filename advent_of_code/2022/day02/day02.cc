// http://adventofcode.com/2022/day/02

#include "advent_of_code/2022/day02/day02.h"

#include "absl/algorithm/container.h"
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

absl::StatusOr<std::string> Day_2022_02::Part1(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (absl::string_view round : input) {
    if (round.size() != 3) return Error("Bad round");
    int a = round[0] - 'A';
    int b = round[2] - 'X';
    score += b + 1;
    if (b == (a + 1) % 3) score += 6;
    if (b == a) score += 3;
  }
  return IntReturn(score);
}

absl::StatusOr<std::string> Day_2022_02::Part2(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (absl::string_view round : input) {
    if (round.size() != 3) return Error("Bad round");
    int a = round[0] - 'A';
    int b = round[2] - 'X';
    int c = (b + 2 + a) % 3;
    score += c + 1;
    if (c == (a + 1) % 3) score += 6;
    if (c == a) score += 3;
  }
  return IntReturn(score);
}

}  // namespace advent_of_code
