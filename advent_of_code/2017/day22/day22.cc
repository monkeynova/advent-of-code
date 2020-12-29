#include "advent_of_code/2017/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Nav {
  Point cur{0, 0};
  Point dir = Cardinal::kNorth;
};

absl::StatusOr<absl::flat_hash_set<Point>> Parse(absl::Span<absl::string_view> input) {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();
  if (b->width() % 2 != 1) return AdventDay::Error("Bad width");
  if (b->height() % 2 != 1) return AdventDay::Error("Bad width");

  Point center = {b->width() / 2, b->height() / 2};

  absl::flat_hash_set<Point> sparse_board;
  for (Point p : b->range()) if (b->at(p) == '#') sparse_board.insert(p - center);
  return sparse_board;
}

bool Move(Nav& nav, absl::flat_hash_set<Point>& board) {
  bool ret;

  if (board.contains(nav.cur)) {
    nav.dir = nav.dir.rotate_right();
    board.erase(nav.cur);
    ret = false;
  } else {
    nav.dir = nav.dir.rotate_left();
    board.insert(nav.cur);
    ret = true;
  }

  nav.cur = nav.cur + nav.dir;

  return ret;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day22_2017::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_set<Point>> sparse_board = Parse(input);

  Nav n;
  int infected = 0;
  for (int i = 0; i < 10000; ++i) {
    VLOG(1) << absl::StrJoin(*sparse_board, ",", [](std::string* out, Point p) { absl::StrAppend(out, p.DebugString());});
    if (Move(n, *sparse_board)) ++infected;
  }

  return IntReturn(infected);
}

absl::StatusOr<std::vector<std::string>> Day22_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
