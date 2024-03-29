#include "advent_of_code/2015/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_06::Part1(
    absl::Span<std::string_view> input) const {
  CharBoard yard(1000, 1000);

  for (std::string_view str : input) {
    PointRectangle r;
    std::string_view cmd;
    if (!RE2::FullMatch(str, "(.*) (\\d+,\\d+) through (\\d+,\\d+)", &cmd,
                        r.min.Capture(), r.max.Capture())) {
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
    VLOG(1) << "  on: " << yard.CountOn();
  }

  return AdventReturn(yard.CountOn());
}

absl::StatusOr<std::string> Day_2015_06::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<std::vector<int>> yard;
  std::vector<int> empty_row;
  empty_row.resize(1000);
  for (int i = 0; i < 1000; ++i) {
    yard.push_back(empty_row);
  }

  for (std::string_view str : input) {
    Point min;
    Point max;
    std::string_view cmd;
    if (!RE2::FullMatch(str, "(.*) (\\d+,\\d+) through (\\d+,\\d+)", &cmd,
                        min.Capture(), max.Capture())) {
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

  return AdventReturn(total_brightness);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/6,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_06()); });

}  // namespace advent_of_code
