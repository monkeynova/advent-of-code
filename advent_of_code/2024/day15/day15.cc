// http://adventofcode.com/2024/day/15

#include "advent_of_code/2024/day15/day15.h"

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

bool TryPushNS(CharBoard& b, const absl::flat_hash_set<Point>& test, Point dir) {
  absl::flat_hash_set<Point> sub_test;
  for (Point p : test) {
    CHECK(b.OnBoard(p+dir));
    switch (b[p+dir]) {
      case '#': return false;
      case '.': break;
      case '[': {
        sub_test.insert(p + dir);
        sub_test.insert(p + dir + Cardinal::kEast);
        break;
      }
      case ']': {
        sub_test.insert(p + dir);
        sub_test.insert(p + dir + Cardinal::kWest);
        break;
      }
      default: LOG(FATAL) << "Bad board char";
    }
  }
  if (!sub_test.empty() && !TryPushNS(b, sub_test, dir)) return false;
  for (Point p : test) {
    CHECK_EQ(b[p + dir], '.');
    b[p+dir] = b[p];
    b[p] = '.';
  }
  return true;
}

bool TryPushNS(CharBoard& b, Point p, Point dir) {
  absl::flat_hash_set<Point> test = {p};
  return TryPushNS(b, test, dir);
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_15::Part1(
    absl::Span<std::string_view> input) const {
  Point robot;
  bool in_board = true;
  CharBoard b;
  for (int i = 0; i < input.size(); ++i) {
    if (in_board) {
      if (input[i].empty()) {
        ASSIGN_OR_RETURN(b, CharBoard::Parse(input.subspan(0, i)));
        ASSIGN_OR_RETURN(robot, b.FindUnique('@'));
        in_board = false;
      }
      continue;
    }
    for (char c : input[i]) {
      Point dir;
      switch (c) {
        case '^': dir = Cardinal::kNorth; break;
        case 'v': dir = Cardinal::kSouth; break;
        case '<': dir = Cardinal::kWest; break;
        case '>': dir = Cardinal::kEast; break;
        default: return absl::InvalidArgumentError("Bad char");
      }
     for (Point freep = robot + dir; b.OnBoard(freep); freep += dir) {
        if (b[freep] == '#') break;
        if (b[freep] == '.') {
          std::swap(b[robot + dir], b[robot]);
          if (freep != robot+dir) {
            std::swap(b[freep], b[robot]);
          }
          robot += dir;
          break;
        }
      }  
    }
  }
  int gps_total = 0;
  for (Point p : b.Find('O')) {
    gps_total += p.y * 100 + p.x;
  }
  return AdventReturn(gps_total);
}

absl::StatusOr<std::string> Day_2024_15::Part2(
    absl::Span<std::string_view> input) const {
  Point robot;
  bool in_board = true;
  CharBoard b;
  for (int i = 0; i < input.size(); ++i) {
    if (in_board) {
      if (input[i].empty()) {
        ASSIGN_OR_RETURN(CharBoard in, CharBoard::Parse(input.subspan(0, i)));
        b = CharBoard(in.width() * 2, in.height());
        for (Point p : in.range()) {
          switch (in[p]) {
            case '#': {
              b[{p.x * 2, p.y}] = '#';
              b[{p.x * 2 + 1, p.y}] = '#';
              break;
            }
            case '.': {
              b[{p.x * 2, p.y}] = '.';
              b[{p.x * 2 + 1, p.y}] = '.';
              break;
            }
            case 'O': {
              b[{p.x * 2, p.y}] = '[';
              b[{p.x * 2 + 1, p.y}] = ']';
              break;
            }
            case '@': {
              b[{p.x * 2, p.y}] = '@';
              b[{p.x * 2 + 1, p.y}] = '.';
              break;
            }
            default: return absl::InvalidArgumentError("bad board");
          }
        }
        ASSIGN_OR_RETURN(robot, b.FindUnique('@'));
        in_board = false;
      }
      continue;
    }
    for (char c : input[i]) {
      //LOG(ERROR) << "Board:\n" << b;
      Point dir;
      switch (c) {
        case '^': dir = Cardinal::kNorth; break;
        case 'v': dir = Cardinal::kSouth; break;
        case '<': dir = Cardinal::kWest; break;
        case '>': dir = Cardinal::kEast; break;
        default: return absl::InvalidArgumentError("Bad char");
      }
      if (dir.y == 0) {
        for (Point freep = robot + dir; b.OnBoard(freep); freep += dir) {
          if (b[freep] == '#') break;
          if (b[freep] == '.') {
            for (Point p = freep; p != robot; p -= dir) {
              b[p] = b[p - dir];
            }
            b[robot] = '.';
            robot += dir;
            break;
          }
        }  
      } else {
        if (dir.x != 0) return absl::InternalError("Bad dir");
        if (TryPushNS(b, robot, dir)) {
          robot += dir;
        }
      }
    }
    //LOG(ERROR) << "Board:\n" << b;
  }
  int gps_total = 0;
  for (Point p : b.Find('[')) {
    gps_total += p.y * 100 + p.x;
  }
  return AdventReturn(gps_total);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/15,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_15()); });

}  // namespace advent_of_code
