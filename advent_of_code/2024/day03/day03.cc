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
  for (std::string_view mem : input) {
    for (int cpos = 0; cpos + 4 < mem.size(); ++cpos) {
      if (mem.substr(cpos, 4) != "mul(") continue;
      int xstart = cpos + 4;
      int xend = cpos + 4;
      while (std::isdigit(mem[xend])) ++xend;
      if (xend == xstart) continue;
      if (mem[xend] != ',') continue;
      int ystart = xend + 1;
      int yend = xend + 1;
      while (std::isdigit(mem[yend])) ++yend;
      if (yend == ystart) continue;
      if (mem[yend] != ')') continue;
      int x;
      int y;
      if (!absl::SimpleAtoi(mem.substr(xstart, xend - xstart), &x)) continue;
      if (!absl::SimpleAtoi(mem.substr(ystart, yend - ystart), &y)) continue;
      sum += x * y;
      cpos = yend;
    }
  }
  return AdventReturn(sum);
}

absl::StatusOr<std::string> Day_2024_03::Part2(
    absl::Span<std::string_view> input) const {
  int sum = 0;
  bool enabled = true;
  for (std::string_view mem : input) {
    for (int cpos = 0; cpos + 4 < mem.size(); ++cpos) {
      if (mem.substr(cpos, 2) == "do") {
        if (mem.substr(cpos + 2, 2) == "()") {
          enabled = true;
          cpos += 3;
          continue;
        }
        if (mem.substr(cpos + 2, 5) == "n't()") {
          enabled = false;
          cpos += 6;
          continue;
        }
        ++cpos;
        continue;
      }
      if (!enabled) continue;

      if (mem.substr(cpos, 4) != "mul(") continue;
      int xstart = cpos + 4;
      int xend = cpos + 4;
      while (std::isdigit(mem[xend])) ++xend;
      if (xend == xstart) continue;
      if (mem[xend] != ',') continue;
      int ystart = xend + 1;
      int yend = xend + 1;
      while (std::isdigit(mem[yend])) ++yend;
      if (yend == ystart) continue;
      if (mem[yend] != ')') continue;
      int x;
      int y;
      if (!absl::SimpleAtoi(mem.substr(xstart, xend - xstart), &x)) continue;
      if (!absl::SimpleAtoi(mem.substr(ystart, yend - ystart), &y)) continue;
      sum += x * y;
      cpos = yend;
    }
  }
  return AdventReturn(sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/3,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_03()); });

}  // namespace advent_of_code