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
  std::vector<std::pair<int, int>> rules;
  bool parse_rules = true;
  int sum = 0;
  for (std::string_view line : input) {
    if (parse_rules) {
      if (line.empty()) {
        parse_rules = false;
      } else {
        ASSIGN_OR_RETURN(std::vector<int64_t> rule,
                         ParseAsInts(absl::StrSplit(line, "|")));
        if (rule.size() != 2) return absl::InvalidArgumentError("bad line");
        rules.emplace_back(rule[0], rule[1]);
      }
      continue;
    }

    ASSIGN_OR_RETURN(std::vector<int64_t> ordered,
                     ParseAsInts(absl::StrSplit(line, ",")));
    int valid = true;
    for (int i = 0; valid && i < ordered.size(); ++i) {
      for (const auto& [pre, post] : rules) {
        if (pre == ordered[i]) {
          for (int j = 0; j < i; ++j) {
            if (post == ordered[j]) {
              valid = false;
              break;
            }
          }
        }
        if (post == ordered[i]) {
          for (int j = i + 1; j < ordered.size(); ++j) {
            if (pre == ordered[j]) {
              valid = false;
              break;
            }
          }
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
  std::vector<std::pair<int, int>> rules;
  bool parse_rules = true;
  int sum = 0;
  for (std::string_view line : input) {
    if (parse_rules) {
      if (line.empty()) {
        parse_rules = false;
      } else {
        ASSIGN_OR_RETURN(std::vector<int64_t> rule,
                         ParseAsInts(absl::StrSplit(line, "|")));
        if (rule.size() != 2) return absl::InvalidArgumentError("bad line");
        rules.emplace_back(rule[0], rule[1]);
      }
      continue;
    }

    ASSIGN_OR_RETURN(std::vector<int64_t> ordered,
                     ParseAsInts(absl::StrSplit(line, ",")));
    int valid = true;
  RETRY:
    for (int i = 0; i < ordered.size(); ++i) {
      for (const auto& [pre, post] : rules) {
        if (pre == ordered[i]) {
          for (int j = 0; j < i; ++j) {
            if (post == ordered[j]) {
              valid = false;
              std::swap(ordered[i], ordered[j]);
              goto RETRY;
            }
          }
        }
        if (post == ordered[i]) {
          for (int j = i + 1; j < ordered.size(); ++j) {
            if (pre == ordered[j]) {
              valid = false;
              std::swap(ordered[i], ordered[j]);
              goto RETRY;
            }
          }
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
