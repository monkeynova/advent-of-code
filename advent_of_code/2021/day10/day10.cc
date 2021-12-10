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
  static const absl::flat_hash_map<char, int64_t> kScoreMap = {
      {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};
  static const absl::flat_hash_map<char, char> kPairMap = {
      {'}', '{'}, {']', '['}, {')', '('}, {'>', '<'}};

  int64_t total_score = 0;
  for (absl::string_view line : input) {
    std::vector<char> stack;
    for (char c : line) {
      auto it = kPairMap.find(c);
      if (it == kPairMap.end()) {
        stack.push_back(c);
        continue;
      }
      if (stack.empty()) return Error("Bad line: ", line);
      if (stack.back() != it->second) {
        auto it = kScoreMap.find(c);
        if (it == kScoreMap.end()) return Error("Bad score: ", line);
        total_score += it->second;
        break;
      }
      stack.pop_back();
    }
  }
  return IntReturn(total_score);
}

absl::StatusOr<std::string> Day_2021_10::Part2(
    absl::Span<absl::string_view> input) const {
  static const absl::flat_hash_map<char, int64_t> kScoreMap = {
      {'(', 1}, {'[', 2}, {'{', 3}, {'<', 4}};
  static const absl::flat_hash_map<char, char> kPairMap = {
      {'}', '{'}, {']', '['}, {')', '('}, {'>', '<'}};

  std::vector<int64_t> scores;
  for (absl::string_view line : input) {
    std::vector<char> stack;
    bool corrupted = false;
    for (char c : line) {
      auto it = kPairMap.find(c);
      if (it == kPairMap.end()) {
        stack.push_back(c);
        continue;
      }
      if (stack.empty()) return Error("Bad line: ", line);
      if (stack.back() != it->second) {
        corrupted = true;
        break;
      }
      stack.pop_back();
    }
    if (corrupted) continue;
    absl::c_reverse(stack);
    int64_t score = 0;
    for (char c : stack) {
      auto it = kScoreMap.find(c);
      if (it == kScoreMap.end()) return Error("Bad score: ", line);
      score = score * 5 + it->second;
    }
    scores.push_back(score);
  }
  absl::c_sort(scores);
  if (scores.size() % 2 != 1) return Error("Bad median");

  return IntReturn(scores[scores.size() / 2]);
}

}  // namespace advent_of_code
