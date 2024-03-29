#include "advent_of_code/2017/day09/day09.h"

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

int TotalGroupScore(std::string_view str) {
  int total_score = 0;
  int group_level = 0;
  bool in_garbage = false;
  bool in_garbage_escape = false;

  for (char c : str) {
    VLOG(2) << c << ": group_level" << group_level
            << ", in_garbage=" << in_garbage
            << ", in_garbage_escape=" << in_garbage_escape;
    if (in_garbage) {
      if (in_garbage_escape) {
        in_garbage_escape = false;
      } else {
        if (c == '>') in_garbage = false;
        if (c == '!') in_garbage_escape = true;
      }
    } else {
      if (c == '{') ++group_level;
      if (c == '}') {
        total_score += group_level;
        --group_level;
      }
      if (c == '<') in_garbage = true;
    }
  }

  return total_score;
}

int GarbageCount(std::string_view str) {
  int garbage_count = 0;
  int group_level = 0;
  bool in_garbage = false;
  bool in_garbage_escape = false;

  for (char c : str) {
    VLOG(2) << c << ": group_level" << group_level
            << ", in_garbage=" << in_garbage
            << ", in_garbage_escape=" << in_garbage_escape;
    if (in_garbage) {
      if (in_garbage_escape) {
        in_garbage_escape = false;
      } else {
        if (c == '>')
          in_garbage = false;
        else if (c == '!')
          in_garbage_escape = true;
        else
          ++garbage_count;
      }
    } else {
      if (c == '{') ++group_level;
      if (c == '}') {
        --group_level;
      }
      if (c == '<') in_garbage = true;
    }
  }

  return garbage_count;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_09::Part1(
    absl::Span<std::string_view> input) const {
  if (run_audit()) {
    std::vector<std::pair<std::string_view, int>> tests = {
        {"{}", 1},
        {"{{{}}}", 6},
        {"{{}{}}", 5},
        {"{{<a!>},{<a!>},{<a!>},{<ab>}}", 3},
    };
    for (const auto& [str, expect] : tests) {
      int got = TotalGroupScore(str);
      if (got != expect) {
        return Error("Bad test: ", got, " != ", expect, ": ", str);
      }
    }
  }
  if (input.size() != 1) return Error("Bad input");
  return AdventReturn(TotalGroupScore(input[0]));
}

absl::StatusOr<std::string> Day_2017_09::Part2(
    absl::Span<std::string_view> input) const {
  if (run_audit()) {
    std::vector<std::pair<std::string_view, int>> tests = {
        {"<>", 0},
        {"<random characters>", 17},
        {"<{o\"i!a,<{i<a>", 10},
        {"<!!>", 0},
    };
    for (const auto& [str, expect] : tests) {
      int got = GarbageCount(str);
      if (got != expect) {
        return Error("Bad test: ", got, " != ", expect, ": ", str);
      }
    }
  }
  if (input.size() != 1) return Error("Bad input");
  return AdventReturn(GarbageCount(input[0]));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/9,
    []() { return std::unique_ptr<AdventDay>(new Day_2017_09()); });

}  // namespace advent_of_code
