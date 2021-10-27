#include "advent_of_code/2015/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

CharBoard RunStep(const CharBoard& in) {
  CharBoard out = in;
  for (Point p : in.range()) {
    int neighbors = 0;
    for (Point dir : Cardinal::kEightDirs) {
      Point n = p + dir;
      if (in.OnBoard(n) && in[n] == '#') {
        ++neighbors;
      }
    }
    if (in[p] == '#') {
      out[p] = neighbors == 2 || neighbors == 3 ? '#' : '.';
    } else {
      out[p] = neighbors == 3 ? '#' : '.';
    }
  }
  return out;
}

CharBoard RunStep2(const CharBoard& in) {
  CharBoard out = in;
  for (Point p : in.range()) {
    int neighbors = 0;
    for (Point dir : Cardinal::kEightDirs) {
      Point n = p + dir;
      if (in.OnBoard(n) && in[n] == '#') {
        ++neighbors;
      }
    }
    if (in[p] == '#') {
      out[p] = neighbors == 2 || neighbors == 3 ? '#' : '.';
    } else {
      out[p] = neighbors == 3 ? '#' : '.';
    }
  }
  out[Point{0, 0}] = '#';
  out[Point{out.width() - 1, 0}] = '#';
  out[Point{0, out.height() - 1}] = '#';
  out[Point{out.width() - 1, out.height() - 1}] = '#';
  return out;
}

int CountOn(const CharBoard& in) {
  int on = 0;
  for (Point p : in.range()) {
    on += in[p] == '#';
  }
  return on;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board.status();
  CharBoard cur = *board;
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cur;
    cur = RunStep(cur);
  }
  return IntReturn(CountOn(cur));
}

absl::StatusOr<std::string> Day_2015_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board.status();
  CharBoard cur = *board;
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cur;
    cur = RunStep2(cur);
  }
  return IntReturn(CountOn(cur));
}

}  // namespace advent_of_code
