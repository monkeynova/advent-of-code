#include "advent_of_code/2017/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Nav {
  Point cur{0, 0};
  Point dir = Cardinal::kNorth;
};

absl::StatusOr<absl::flat_hash_set<Point>> Parse(
    absl::Span<absl::string_view> input) {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();
  if (b->width() % 2 != 1) return Error("Bad width");
  if (b->height() % 2 != 1) return Error("Bad width");

  Point center = {b->width() / 2, b->height() / 2};

  absl::flat_hash_set<Point> sparse_board;
  for (Point p : b->range())
    if (b->at(p) == '#') sparse_board.insert(p - center);
  return sparse_board;
}

bool Move(Nav& nav, absl::flat_hash_set<Point>& board) {
  bool infected = false;

  if (board.contains(nav.cur)) {
    nav.dir = nav.dir.rotate_right();
    board.erase(nav.cur);
  } else {
    nav.dir = nav.dir.rotate_left();
    board.insert(nav.cur);
    infected = true;
  }

  nav.cur += nav.dir;

  return infected;
}

enum State {
  kClean = 0,
  kWeakened = 1,
  kInfected = 2,
  kFlagged = 3,
};

absl::StatusOr<absl::flat_hash_map<Point, State>> Parse2(
    absl::Span<absl::string_view> input) {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();
  if (b->width() % 2 != 1) return Error("Bad width");
  if (b->height() % 2 != 1) return Error("Bad width");

  Point center = {b->width() / 2, b->height() / 2};

  absl::flat_hash_map<Point, State> sparse_board;
  for (Point p : b->range())
    if (b->at(p) == '#') sparse_board.emplace(p - center, kInfected);
  return sparse_board;
}

bool Move2(Nav& nav, absl::flat_hash_map<Point, State>& board) {
  bool infected = false;

  if (!board.contains(nav.cur)) board[nav.cur] = kClean;
  switch (board[nav.cur]) {
    case kClean: {
      nav.dir = nav.dir.rotate_left();
      board[nav.cur] = kWeakened;
      break;
    }
    case kWeakened: {
      // Same direction.
      board[nav.cur] = kInfected;
      infected = true;
      break;
    }
    case kInfected: {
      nav.dir = nav.dir.rotate_right();
      board[nav.cur] = kFlagged;
      break;
    }
    case kFlagged: {
      nav.dir = -nav.dir;
      board[nav.cur] = kClean;
      break;
    }
    default: {
      LOG(FATAL) << "Bad state";
    }
  }

  nav.cur += nav.dir;

  return infected;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_22::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_set<Point>> sparse_board = Parse(input);

  Nav n;
  int infected = 0;
  for (int i = 0; i < 10'000; ++i) {
    VLOG(1) << absl::StrJoin(*sparse_board, ",", [](std::string* out, Point p) {
      absl::StrAppend(out, p);
    });
    if (Move(n, *sparse_board)) ++infected;
  }

  return IntReturn(infected);
}

absl::StatusOr<std::string> Day_2017_22::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<Point, State>> sparse_board =
      Parse2(input);

  Nav n;
  int infected = 0;
  for (int i = 0; i < 10'000'000; ++i) {
    if (Move2(n, *sparse_board)) ++infected;
  }

  return IntReturn(infected);
}

}  // namespace advent_of_code
