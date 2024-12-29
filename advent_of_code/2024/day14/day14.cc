// http://adventofcode.com/2024/day/14

#include "advent_of_code/2024/day14/day14.h"

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

struct Drone {
  Point p;
  Point v;
};

}  // namespace

absl::StatusOr<std::string> Day_2024_14::Part1(
    absl::Span<std::string_view> input) const {
  Point tile;
  std::vector<Drone> drones;
  if (!RE2::FullMatch(param(), "(\\d+,\\d+)", tile.Capture())) {
    return absl::InternalError("bad param");
  }
  if (tile.x % 2 != 1) return absl::InternalError("bad param %x");
  if (tile.y % 2 != 1) return absl::InternalError("bad param %y");

  for (std::string_view line : input) {
    Drone d;
    if (!RE2::FullMatch(line, "p=(-?\\d+,-?\\d+) v=(-?\\d+,-?\\d+)", d.p.Capture(), d.v.Capture())) {
      return absl::InvalidArgumentError("bad line");
    }
    drones.push_back(d);
  }
  for (int i = 0; i < 100; ++i) {
    for (Drone& d : drones) {
      d.p = (d.p + d.v).TorusPoint(tile);
    } 
  }
  std::array<int, 4> quadrant_count = {0, 0, 0, 0};
  for (const Drone& d : drones) {
    if (d.p.x < tile.x / 2) {
      if (d.p.y < tile.y / 2) {
        ++quadrant_count[0];
      } else if (d.p.y > tile.y / 2) {
        ++quadrant_count[1];
      }
    } else if (d.p.x > tile.x / 2) {
      if (d.p.y < tile.y / 2) {
        ++quadrant_count[2];
      } else if (d.p.y > tile.y / 2) {
        ++quadrant_count[3];
      }
    }
  }
  int score = quadrant_count[0] * quadrant_count[1] * quadrant_count[2] * quadrant_count[3];
  return AdventReturn(score);
}

absl::StatusOr<std::string> Day_2024_14::Part2(
    absl::Span<std::string_view> input) const {
  Point tile;
  std::vector<Drone> drones;
  if (!RE2::FullMatch(param(), "(\\d+,\\d+)", tile.Capture())) {
    return absl::InternalError("bad param");
  }
  if (tile.x % 2 != 1) return absl::InternalError("bad param %x");
  if (tile.y % 2 != 1) return absl::InternalError("bad param %y");

  for (std::string_view line : input) {
    Drone d;
    if (!RE2::FullMatch(line, "p=(-?\\d+,-?\\d+) v=(-?\\d+,-?\\d+)", d.p.Capture(), d.v.Capture())) {
      return absl::InvalidArgumentError("bad line");
    }
    drones.push_back(d);
  }
  int steps = 0;
  for (;; ++steps) {
    absl::flat_hash_set<Point> test;
    for (Drone& d : drones) {
      test.insert(d.p);
      d.p = (d.p + d.v).TorusPoint(tile);
    }
    int edge_count = 0;
    for (Point p : test) {
      for (Point d : Cardinal::kFourDirs) {
        if (test.contains(p + d)) {
          ++edge_count;
        }
      }
    }
    if (edge_count > 1000) {
      CharBoard b = CharBoard::DrawNew(test);
      LOG(ERROR) << "Tree @" << steps << ";" << edge_count << "\n" << b;
      break;
    }
  }
  return AdventReturn(steps);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/14,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_14()); });

}  // namespace advent_of_code
