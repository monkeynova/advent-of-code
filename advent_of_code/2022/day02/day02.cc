#include "advent_of_code/2022/day02/day02.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int Score1(char c1, char c2) {
  int p1 = c1 - 'A';
  int p2 = c2 - 'X';
  // Score my choice.
  int score = p2 + 1;
  // Win is exactly one more, modulo 3.
  if (p2 == (p1 + 1) % 3) score += 6;
  // Tie.
  if (p2 == p1) score += 3;
  return score;
}

int Score2(char c1, char c2) {
  int p1 = c1 - 'A';
  int outcome = c2 - 'X';
  // {0, 1, 2} -> {Lose, Draw Win} -> p2 - p1 = outcome - 1 MOD 3.
  int p2 = (outcome + 2 + p1) % 3;
  // Score my choice.
  int score = p2 + 1;
  // Win is exactly one more, modulo 3.
  if (p2 == (p1 + 1) % 3) score += 6;
  // Tie.
  if (p2 == p1) score += 3;
  return score;
}

absl::StatusOr<int> Play(absl::Span<std::string_view> input,
                         absl::FunctionRef<int(char, char)> score) {
  absl::flat_hash_map<std::string, int> line2score;
  for (char c1 : {'A', 'B', 'C'}) {
    for (char c2 : {'X', 'Y', 'Z'}) {
      line2score.emplace(absl::StrFormat("%c %c", c1, c2), score(c1, c2));
    }
  }
  int total_score = 0;
  for (std::string_view round : input) {
    auto it = line2score.find(round);
    if (it == line2score.end()) return Error("Bad round: ", round);
    total_score += it->second;
  }
  return total_score;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_02::Part1(
    absl::Span<std::string_view> input) const {
  return AdventReturn(Play(input, Score1));
}

absl::StatusOr<std::string> Day_2022_02::Part2(
    absl::Span<std::string_view> input) const {
  return AdventReturn(Play(input, Score2));
}

}  // namespace advent_of_code
