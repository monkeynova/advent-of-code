// http://adventofcode.com/2022/day/03

#include "advent_of_code/2022/day03/day03.h"

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

absl::StatusOr<char> UniqueCheck(absl::StatusOr<char> cur, char v) {
  if (cur.status().code() == absl::StatusCode::kNotFound) return v;
  if (!cur.ok()) return cur;
  if (*cur != v) {
    return Error(absl::StrFormat("Not the same: %c != %c", *cur, v));
  }
  return v;
}

absl::StatusOr<int> Score(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A' + 27;
  if (c >= 'a' && c <= 'z') return c - 'a' + 1;
  return Error("Bad line");
}

}  // namespace

absl::StatusOr<std::string> Day_2022_03::Part1(
    absl::Span<absl::string_view> input) const {
  int total_score = 0;
  for (absl::string_view line : input) {
    if (line.size() % 2 != 0) return Error("Line size isn't even");

    std::string_view first = line.substr(0, line.size() / 2);
    std::string_view second = line.substr(line.size() / 2);

    absl::flat_hash_set<char> set(first.begin(), first.end());
    absl::StatusOr<char> found = absl::NotFoundError("Bad line: no duplicate");
    for (char c : second) {
      if (!set.contains(c)) continue;
      found = UniqueCheck(found, c);
    }
    if (!found.ok()) return found.status();
    absl::StatusOr<int> score = Score(*found);
    if (!score.ok()) return score.status();
    total_score += *score;
  }
  return IntReturn(total_score);
}

absl::StatusOr<std::string> Day_2022_03::Part2(
    absl::Span<absl::string_view> input) const {
  int total_score = 0;
  if (input.size() % 3 != 0) return Error("Input isn't groups of 3");
  for (int i = 0; i < input.size(); i += 3) {
    absl::flat_hash_set<char> a_set(input[i].begin(), input[i].end());
    absl::flat_hash_set<char> b_set(input[i+1].begin(), input[i+1].end());
    absl::StatusOr<char> found = absl::NotFoundError("Bad line: no duplicate");
    for (char c : input[i+2]) {
      if (!a_set.contains(c)) continue;
      if (!b_set.contains(c)) continue;
      found = UniqueCheck(found, c);
    }
    if (!found.ok()) return found.status();
    absl::StatusOr<int> score = Score(*found);
    if (!score.ok()) return score.status();
    total_score += *score;
  }
  return IntReturn(total_score);
}

}  // namespace advent_of_code
