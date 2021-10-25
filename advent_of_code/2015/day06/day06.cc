#include "advent_of_code/2015/day06/day06.h"

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

int CountOn(const CharBoard& board) {
  int lit = 0;
  VLOG(1) << "CountOn: " << board.range();
  for (const Point p : board.range()) {
    VLOG(2) << "CountOn: " << p << ": " << board[p];
    if (board[p] == '#') ++lit;
  }
  return lit;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day_2015_06::Part1(
    absl::Span<absl::string_view> input) const {
  CharBoard yard(1000, 1000);

  for (absl::string_view str : input) {
    PointRectangle r;
    absl::string_view cmd;
    if (!RE2::FullMatch(str, "(.*) (\\d+),(\\d+) through (\\d+),(\\d+)", &cmd,
                        &r.min.x, &r.min.y, &r.max.x, &r.max.y)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
    VLOG(1) << cmd << ": " << r;
    if (cmd == "turn on") {
      for (Point p : r) {
        VLOG(2) << p;
        yard[p] = '#';
      }
    } else if (cmd == "turn off") {
      for (Point p : r) {
        VLOG(2) << p;
        yard[p] = '.';
      }
    } else if (cmd == "toggle") {
      for (Point p : r) {
        VLOG(2) << p << ": " << yard[p];
        yard[p] = yard[p] == '#' ? '.' : '#';
      }
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
    VLOG(1) << "  on: " << CountOn(yard);
  }

  return IntReturn(CountOn(yard));
}

absl::StatusOr<std::vector<std::string>> Day_2015_06::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<std::vector<int>> yard;
  std::vector<int> empty_row;
  empty_row.resize(1000);
  for (int i = 0; i < 1000; ++i) {
    yard.push_back(empty_row);
  }

  for (absl::string_view str : input) {
    Point min;
    Point max;
    absl::string_view cmd;
    if (!RE2::FullMatch(str, "(.*) (\\d+),(\\d+) through (\\d+),(\\d+)", &cmd,
                        &min.x, &min.y, &max.x, &max.y)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
    if (cmd == "turn on") {
      for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
          ++yard[y][x];
        }
      }
    } else if (cmd == "turn off") {
      for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
          if (--yard[y][x] <= 0) yard[y][x] = 0;
        }
      }
    } else if (cmd == "toggle") {
      for (int y = min.y; y <= max.y; ++y) {
        for (int x = min.x; x <= max.x; ++x) {
          yard[y][x] += 2;
        }
      }
    } else {
      return absl::InvalidArgumentError(absl::StrCat("Bad instruction: ", str));
    }
  }

  int total_brightness = 0;
  for (const std::vector<int>& row : yard) {
    for (int b : row) {
      total_brightness += b;
    }
  }

  return IntReturn(total_brightness);
}

}  // namespace advent_of_code
