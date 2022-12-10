// http://adventofcode.com/2022/day/10

#include "advent_of_code/2022/day10/day10.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/ocr.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

static LazyRE2 addx_re = {"addx (-?\\d+)"};

}  // namespace

absl::StatusOr<std::string> Day_2022_10::Part1(
    absl::Span<absl::string_view> input) const {
  int x = 1;
  std::vector<int> at = {20, 60, 100, 140, 180, 220};
  std::vector<int>::iterator it = at.begin();
  int signal_strength = 0;
  int step = 0;
  auto on_step = [&]() -> bool {
    ++step;
    if (step == *it) {
      signal_strength += x * step;
      if (++it == at.end()) return true;
    }
    return false;
  };
  for (absl::string_view line : input) {
    if (line == "noop") {
      if (on_step()) return IntReturn(signal_strength);
    } else if (int add; RE2::FullMatch(line, *addx_re, &add)) {
      if (on_step()) return IntReturn(signal_strength);
      if (on_step()) return IntReturn(signal_strength);
      x += add;
    }
  }
  return absl::InternalError("Left infinte loop");
}

absl::StatusOr<std::string> Day_2022_10::Part2(
    absl::Span<absl::string_view> input) const {
  CharBoard board(40, 6);
  int sprite = 1;
  int step = -1;
  auto on_step = [&]() {
    ++step;
    Point p{step % 40, step / 40};
    if (abs(sprite - p.x) <= 1) board[p] = '#';
  };
  on_step();
  for (absl::string_view line : input) {
    if (line == "noop") {
      on_step();
    } else if (int add; RE2::FullMatch(line, *addx_re, &add)) {
      on_step();
      sprite += add;
      on_step();
    }
  }
  return OCRExtract(board);
}

}  // namespace advent_of_code
