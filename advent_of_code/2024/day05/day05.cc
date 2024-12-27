// http://adventofcode.com/2024/day/5

#include "advent_of_code/2024/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_05::Part1(
    absl::Span<std::string_view> input) const {
  std::array<std::vector<int>, 100> pre;
  std::array<std::vector<int>, 100> post;
  bool parse_rules = true;
  int sum = 0;
  for (std::string_view line : input) {
    if (parse_rules) {
      if (line.empty()) {
        parse_rules = false;
      } else {
        if (line.size() != 5) return absl::InvalidArgumentError("bad line");
        int pre_val, post_val;
        if (!absl::SimpleAtoi(line.substr(0, 2), &pre_val)) {
          return absl::InvalidArgumentError("bad line");
        }
        if (line[2] != '|') return absl::InvalidArgumentError("bad line");
        if (!absl::SimpleAtoi(line.substr(3, 2), &post_val)) {
          return absl::InvalidArgumentError("bad line");
        }
        pre[post_val].push_back(pre_val);
        post[pre_val].push_back(post_val);
      }
      continue;
    }

    ASSIGN_OR_RETURN(std::vector<int64_t> ordered,
                     ParseAsInts(absl::StrSplit(line, ",")));
    std::array<int, 100> pos;
    memset(&pos, -1, sizeof(pos));
    for (int i = 0; i < ordered.size(); ++i) {
      if (pos[ordered[i]] != -1) return absl::InvalidArgumentError("dupe");
      pos[ordered[i]] = i;
    }
    int valid = true;
    for (int i = 0; valid && i < ordered.size(); ++i) {
      for (int pre_test : pre[ordered[i]]) {
        if (pos[pre_test] > i) {
          valid = false;
          break;
        }
      }
      for (int post_test : post[ordered[i]]) {
        if (pos[post_test] < i && pos[post_test] != -1) {
          valid = false;
          break;
        }
      }
    }
    if (!valid) continue;
    sum += ordered[ordered.size() / 2];
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_05::Part2(
    absl::Span<std::string_view> input) const {
  std::array<std::vector<int>, 100> pre;
  std::array<std::vector<int>, 100> post;
  bool parse_rules = true;
  int sum = 0;
  for (std::string_view line : input) {
    if (parse_rules) {
      if (line.empty()) {
        parse_rules = false;
      } else {
        if (line.size() != 5) return absl::InvalidArgumentError("bad line");
        int pre_val, post_val;
        if (!absl::SimpleAtoi(line.substr(0, 2), &pre_val)) {
          return absl::InvalidArgumentError("bad line");
        }
        if (line[2] != '|') return absl::InvalidArgumentError("bad line");
        if (!absl::SimpleAtoi(line.substr(3, 2), &post_val)) {
          return absl::InvalidArgumentError("bad line");
        }
        pre[post_val].push_back(pre_val);
        post[pre_val].push_back(post_val);
      }
      continue;
    }

    ASSIGN_OR_RETURN(std::vector<int64_t> ordered,
                     ParseAsInts(absl::StrSplit(line, ",")));
    std::array<int, 100> pos;
    memset(&pos, -1, sizeof(pos));
    for (int i = 0; i < ordered.size(); ++i) {
      if (pos[ordered[i]] != -1) return absl::InvalidArgumentError("dupe");
      pos[ordered[i]] = i;
    }
    int valid = true;
    int start_i = 0;
  RETRY:
    for (int i = start_i; i < ordered.size(); ++i) {
      for (int pre_test : pre[ordered[i]]) {
        if (pos[pre_test] > i) {
          valid = false;
          std::swap(ordered[i], ordered[pos[pre_test]]);
          std::swap(pos[ordered[i]], pos[ordered[pos[pre_test]]]);
          start_i = i;
          goto RETRY;
        }
      }
      for (int post_test : post[ordered[i]]) {
        if (pos[post_test] < i && pos[post_test] != -1) {
          valid = false;
          std::swap(ordered[i], ordered[pos[post_test]]);
          std::swap(pos[ordered[i]], pos[ordered[pos[post_test]]]);
          start_i = pos[ordered[i]];
          goto RETRY;
        }
      }
    }

    if (valid) continue;

    sum += ordered[ordered.size() / 2];
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/5,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_05()); });

}  // namespace advent_of_code
