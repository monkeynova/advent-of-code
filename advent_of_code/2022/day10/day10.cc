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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_10::Part1(
    absl::Span<absl::string_view> input) const {
  int x = 1;
  std::vector<int> at = {20, 60, 100, 140, 180, 220};
  std::vector<int>::iterator it = at.begin();
  int signal_strength = 0;
  int step = 0;
  for (absl::string_view line : input) {
    int start_x = x;
    int add;
    if (line == "noop") {
      ++step;
    } else if (RE2::FullMatch(line, "addx (-?\\d+)", &add)) {
      x += add;
      step += 2;
    }
    if (step >= *it) {
      int delta;
      if (true || step > *it) {
        delta = *it * start_x;
      } else {
        delta = *it * x;
      }
      VLOG(1) << x;
      VLOG(1) << start_x;
      VLOG(1) << *it << ": " << delta;
      signal_strength += delta;
      ++it;
      if (it == at.end()) return IntReturn(signal_strength);
    }
  }
  return absl::InternalError("Infinte loop");
}

absl::StatusOr<std::string> Day_2022_10::Part2(
    absl::Span<absl::string_view> input) const {
  CharBoard board(40, 6);
  int sprite = 1;
  int signal_strength = 0;
  int step = 0;
  board[{0,0}] = '#';
  for (absl::string_view line : input) {
    int start_sprite = sprite;

    int add;
    if (line == "noop") {
      ++step;
      Point p{step % 40, step / 40};
      if (abs(sprite - p.x) <= 1) board[p] = '#';
    } else if (RE2::FullMatch(line, "addx (-?\\d+)", &add)) {
      ++step;
      Point p{step % 40, step / 40};
      if (abs(sprite - p.x) <= 1) board[p] = '#';
      ++step;
      p = {step % 40, step / 40};
      sprite += add;
      if (abs(sprite - p.x) <= 1) board[p] = '#';
    }
  }
  return OCRExtract(board);
}

}  // namespace advent_of_code
