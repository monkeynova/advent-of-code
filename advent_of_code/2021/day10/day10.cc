#include "advent_of_code/2021/day10/day10.h"

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

absl::StatusOr<std::string> Day_2021_10::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<char, int64_t> scores = {
    {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}
  };
  absl::flat_hash_map<char, char> pair = {
    {'}', '{'}, {']', '['}, {')', '('}, {'>', '<'}
  };
  int64_t total_score = 0;
  for (absl::string_view line : input) {
    if (line == "HACK:") continue;
    std::vector<char> stack;
    for (char c : line) {
      auto it = pair.find(c);
      if (it == pair.end()) {
        stack.push_back(c);
        continue;
      } 
      if (stack.empty()) return Error("Bad line: ", line);
      if (stack.back() == it->second) {
        stack.pop_back();
      } else {
        auto it = scores.find(c);
        if (it == scores.end()) return Error("Bad score: ", line);
        total_score += it->second;
        break;
      }
    }
  }
  return IntReturn(total_score);
}

absl::StatusOr<std::string> Day_2021_10::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<char, int64_t> scores1 = {
    {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}
  };
  absl::flat_hash_map<char, int64_t> scores2 = {
    {'(', 1}, {'[', 2}, {'{', 3}, {'<', 4}
  };
  absl::flat_hash_map<char, char> pair = {
    {'}', '{'}, {']', '['}, {')', '('}, {'>', '<'}
  };
  int64_t total_score = 0;
  std::vector<int64_t> scores;
  for (absl::string_view line : input) {
    if (line == "HACK:") continue;
    std::vector<char> stack;
    bool corrupted = false;
    for (char c : line) {
      auto it = pair.find(c);
      if (it == pair.end()) {
        stack.push_back(c);
        continue;
      }
      if (stack.empty()) return Error("Bad line: ", line);
      if (stack.back() == it->second) {
        stack.pop_back();
      } else {
        auto it = scores1.find(c);
        if (it == scores1.end()) return Error("Bad score: ", line);
        corrupted = true;
        break;
      }
    }
    if (corrupted) continue;
    absl::c_reverse(stack);
    int64_t score = 0;
    for (char c : stack) {
      auto it = scores2.find(c);
      if (it == scores2.end()) return Error("Bad score2: ", line);
      score = score * 5 + it->second;
    }
    scores.push_back(score);
  }
  absl::c_sort(scores);
  if (scores.size() % 2 != 1) return Error("Bad median");

  return IntReturn(scores[(scores.size() + 1)/ 2 - 1]);
}

}  // namespace advent_of_code
