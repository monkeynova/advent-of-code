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

absl::StatusOr<std::string> Day_2022_02::Part1(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (absl::string_view round : input) {
    if (round.size() != 3) return Error("Bad round: ", round);
    if (!RE2::FullMatch(round, "[A-C] [X-Z]")) {
      return Error("Bad round: ", round);
    }
    int p1 = round[0] - 'A';
    int p2 = round[2] - 'X';
    // Score my choice.
    score += p2 + 1;
    // Win is exactly one more, modulo 3.
    if (p2 == (p1 + 1) % 3) score += 6;
    // Tie.
    if (p2 == p1) score += 3;
  }
  return IntReturn(score);
}

absl::StatusOr<std::string> Day_2022_02::Part2(
    absl::Span<absl::string_view> input) const {
  int score = 0;
  for (absl::string_view round : input) {
    if (round.size() != 3) return Error("Bad round: ", round);
    if (!RE2::FullMatch(round, "[A-C] [X-Z]")) {
      return Error("Bad round: ", round);
    }
    int p1 = round[0] - 'A';
    int outcome = round[2] - 'X';
    // {0, 1, 2} -> {Lose, Draw Win} -> p2 - p1 = outcome - 1 MOD 3.
    int p2 = (outcome + 2 + p1) % 3;
    // Score my choice.
    score += p2 + 1;
    // Win is exactly one more, modulo 3.
    if (p2 == (p1 + 1) % 3) score += 6;
    // Tie.
    if (p2 == p1) score += 3;
  }
  return IntReturn(score);
}

}  // namespace advent_of_code
