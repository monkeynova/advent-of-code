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

  void Update(Point tile) {
    p = (p + v).TorusPoint(tile);
  }

  std::optional<int> Quadrant(Point tile) const {
    if (p.x < tile.x / 2) {
      if (p.y < tile.y / 2) {
        return 0;
      } else if (p.y > tile.y / 2) {
        return 1;
      }
    } else if (p.x > tile.x / 2) {
      if (p.y < tile.y / 2) {
        return 2;
      } else if (p.y > tile.y / 2) {
        return 3;
      }
    }
    return std::nullopt;
  }

  static absl::StatusOr<Drone> Parse(std::string_view line) {
    Drone ret;
    Tokenizer t(line);
    RETURN_IF_ERROR(t.NextAre({"p", "="}));
    RETURN_IF_ERROR(ret.p.From(t));
    RETURN_IF_ERROR(t.NextAre({"v", "="}));
    RETURN_IF_ERROR(ret.v.From(t));
    if (!t.Done()) {
      return absl::InvalidArgumentError("bad line");
    }
    return ret;
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2024_14::Part1(
    absl::Span<std::string_view> input) const {
  Point tile;
  {
    Tokenizer t(param());
    RETURN_IF_ERROR(tile.From(t));
    if (!t.Done()) return absl::InternalError("bad param");
  }
  if (tile.x % 2 != 1) return absl::InternalError("bad param %x");
  if (tile.y % 2 != 1) return absl::InternalError("bad param %y");

  std::vector<Drone> drones;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Drone d, Drone::Parse(line));
    drones.push_back(d);
  }
  for (int i = 0; i < 100; ++i) {
    for (Drone& d : drones) {
      d.Update(tile);
    } 
  }

  std::array<int, 4> quadrant_count = {0, 0, 0, 0};
  for (const Drone& d : drones) {
    std::optional<int> q = d.Quadrant(tile);
    if (q) ++quadrant_count[*q];
  }
  int score = quadrant_count[0] * quadrant_count[1] * quadrant_count[2] * quadrant_count[3];
  return AdventReturn(score);
}

absl::StatusOr<std::string> Day_2024_14::Part2(
    absl::Span<std::string_view> input) const {
  Point tile;
  {
    Tokenizer t(param());
    RETURN_IF_ERROR(tile.From(t));
    if (!t.Done()) return absl::InternalError("bad param");
  }
  if (tile.x % 2 != 1) return absl::InternalError("bad param %x");
  if (tile.y % 2 != 1) return absl::InternalError("bad param %y");

  std::vector<Drone> drones;
  for (std::string_view line : input) {
    ASSIGN_OR_RETURN(Drone d, Drone::Parse(line));
    drones.push_back(d);
  }
  int steps = 0;
  for (;; ++steps) {
    absl::flat_hash_set<Point> test;
    for (Drone& d : drones) {
      test.insert(d.p);
      d.Update(tile);
    }
    int edge_count = 0;
    for (Point p : test) {
      for (Point d : Cardinal::kFourDirs) {
        if (test.contains(p + d)) {
          ++edge_count;
        }
      }
    }
    if (edge_count > drones.size()) {
      VLOG(2) << "Tree @" << steps << ";" << edge_count << "\n" << CharBoard::DrawNew(test);
      break;
    }
  }
  return AdventReturn(steps);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/14,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_14()); });

}  // namespace advent_of_code
