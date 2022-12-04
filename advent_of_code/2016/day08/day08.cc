// http://adventofcode.com/2016/day/08

#include "advent_of_code/2016/day08/day08.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/ocr.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_08::Part1(
    absl::Span<absl::string_view> input) const {
  CharBoard display(50, 6);
  for (absl::string_view ins : input) {
    int x;
    int y;
    int r;
    if (RE2::FullMatch(ins, "rect (\\d+)x(\\d+)", &x, &y)) {
      PointRectangle r = {{0, 0}, {x - 1, y - 1}};
      for (Point p : r) display[p] = '#';
    } else if (RE2::FullMatch(ins, "rotate row y=(\\d+) by (\\d+)", &y, &r)) {
      CharBoard tmp = display;
      for (int row_x = 0; row_x < tmp.width(); ++row_x) {
        tmp[Point{(row_x + r) % tmp.width(), y}] = display[Point{row_x, y}];
      }
      display = tmp;
    } else if (RE2::FullMatch(ins, "rotate column x=(\\d+) by (\\d+)", &x,
                              &r)) {
      CharBoard tmp = display;
      for (int row_y = 0; row_y < tmp.height(); ++row_y) {
        tmp[Point{x, (row_y + r) % tmp.height()}] = display[Point{x, row_y}];
      }
      display = tmp;
    } else {
      return Error("Bad instruction: ", ins);
    }
    VLOG(1) << "Ins: " << ins;
    VLOG(1) << "Board:\n" << display;
  }
  LOG(INFO) << "Board:\n" << display;
  return IntReturn(display.CountOn());
}

absl::StatusOr<std::string> Day_2016_08::Part2(
    absl::Span<absl::string_view> input) const {
  CharBoard display(50, 6);
  for (absl::string_view ins : input) {
    int x;
    int y;
    int r;
    if (RE2::FullMatch(ins, "rect (\\d+)x(\\d+)", &x, &y)) {
      PointRectangle r = {{0, 0}, {x - 1, y - 1}};
      for (Point p : r) display[p] = '#';
    } else if (RE2::FullMatch(ins, "rotate row y=(\\d+) by (\\d+)", &y, &r)) {
      CharBoard tmp = display;
      for (int row_x = 0; row_x < tmp.width(); ++row_x) {
        tmp[Point{(row_x + r) % tmp.width(), y}] = display[Point{row_x, y}];
      }
      display = tmp;
    } else if (RE2::FullMatch(ins, "rotate column x=(\\d+) by (\\d+)", &x,
                              &r)) {
      CharBoard tmp = display;
      for (int row_y = 0; row_y < tmp.height(); ++row_y) {
        tmp[Point{x, (row_y + r) % tmp.height()}] = display[Point{x, row_y}];
      }
      display = tmp;
    } else {
      return Error("Bad instruction: ", ins);
    }
    VLOG(1) << "Ins: " << ins;
    VLOG(1) << "Board:\n" << display;
  }
  return OCRExtract(display);
}

}  // namespace advent_of_code
