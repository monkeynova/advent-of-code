// http://adventofcode.com/2024/day/3

#include "advent_of_code/2024/day03/day03.h"

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

absl::StatusOr<std::string> Day_2024_03::Part1(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  RE2 re("mul\\((\\d+),(\\d+)\\)");
  for (std::string_view mem : input) {
    while (!mem.empty()) {
      int x = 0;
      int y = 0;
      if (!RE2::FindAndConsume(&mem, re, &x, &y)) {
        break;
      }
      sum += x * y;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_03::Part2(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  bool enabled = true;
  RE2 re("mul\\((\\d+),(\\d+)\\)|(do|don't)\\(\\)");
  for (std::string_view mem : input) {
    while (!mem.empty()) {
      std::optional<int> x = 0;
      std::optional<int> y = 0;
      std::string_view cmd;
      if (!RE2::FindAndConsume(&mem, re, &x, &y, &cmd)) {
        break;
      }
      LOG(ERROR) << "cmd = " << cmd;
      if (cmd == "don't") {
        enabled = false;
      } else if (cmd == "do") {
        enabled = true;
      } else if (enabled) {
        sum += *x * *y;
      }
    }
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/3,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_03()); });

}  // namespace advent_of_code
