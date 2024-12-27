// http://adventofcode.com/2024/day/6

#include "advent_of_code/2024/day06/day06.h"

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

absl::StatusOr<std::string> Day_2024_06::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point p, b.FindUnique('^'));
  absl::flat_hash_set<Point> visited;
  Point dir = Cardinal::kNorth;
  visited.insert(p);
  for (p += dir; b.OnBoard(p); p += dir) {
    if (b[p] == '#') {
      p -= dir;
      dir = dir.rotate_right();
    }
    visited.insert(p);
  }

  return AdventReturn(visited.size());
}

absl::StatusOr<std::string> Day_2024_06::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('^'));

  absl::flat_hash_set<Point> blockable;
  absl::flat_hash_set<std::pair<Point, Point>> history;
  int make_loop = 0;
  {
    Point dir = Cardinal::kNorth;
    blockable.insert(start);
    history.emplace(start, dir);
    for (Point p = start + dir; b.OnBoard(p); p += dir) {
      if (b[p] == '#') {
        p -= dir;
        dir = dir.rotate_right();
      } else if (!blockable.contains(p)) {
        b[p] = '#';
        absl::flat_hash_set<std::pair<Point, Point>> history2;
        Point dir2 = dir.rotate_right();
        for (Point p2 = p - dir; b.OnBoard(p2); p2 += dir2) {
          if (b[p2] == '#') {
            p2 -= dir2;
            dir2 = dir2.rotate_right();
            continue;
          }
          if (history.contains({p2, dir2})) {
            ++make_loop;
            break;
          }
          auto [it, inserted] = history2.emplace(p2, dir2);
          if (!inserted) {
            ++make_loop;
            break;
          }
        }
        b[p] = '.';
      }
      blockable.insert(p);
      history.emplace(p, dir);
    }
  }

  return AdventReturn(make_loop);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/6,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_06()); });

}  // namespace advent_of_code
