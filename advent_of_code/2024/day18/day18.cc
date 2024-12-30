// http://adventofcode.com/2024/day/18

#include "advent_of_code/2024/day18/day18.h"

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

absl::StatusOr<std::string> Day_2024_18::Part1(
    absl::Span<std::string_view> input) const {
  CharBoard b;
  int steps = 0;
  {
    Tokenizer t(param());
    ASSIGN_OR_RETURN(int dim, t.NextInt());
    b = CharBoard(dim, dim);
    RETURN_IF_ERROR(t.NextIs(","));
    ASSIGN_OR_RETURN(steps, t.NextInt());
    if (!t.Done()) return absl::InvalidArgumentError("bad param");
  }
  if (input.size() < steps) return absl::InvalidArgumentError("bad steps");
  for (int i = 0; i < steps; ++i) {
    Point p;
    {
      Tokenizer t(input[i]);
      RETURN_IF_ERROR(p.From(t));
      if (!t.Done()) return absl::InvalidArgumentError("bad point");
    }
    if (!b.OnBoard(p)) return absl::InvalidArgumentError("off board");
    b[p] = '#';
  }
  return AdventReturn(PointWalk({
    .start = Point{0, 0},
    .is_good = [&](Point p, int) {
      return b.OnBoard(p) && b[p] != '#';
    },
    .is_final = [&](Point p, int) {
      return p.x == b.width() - 1 && p.y == b.height() - 1;
    }
  }).FindMinSteps());
}

absl::StatusOr<std::string> Day_2024_18::Part2(
    absl::Span<std::string_view> input) const {
  CharBoard board;
  int steps = 0;
  {
    Tokenizer t(param());
    ASSIGN_OR_RETURN(int dim, t.NextInt());
    board = CharBoard(dim, dim);
    RETURN_IF_ERROR(t.NextIs(","));
    ASSIGN_OR_RETURN(steps, t.NextInt());
    if (!t.Done()) return absl::InvalidArgumentError("bad param");
  }
  if (input.size() < steps) return absl::InvalidArgumentError("bad steps");

  std::vector<Point> points;
  absl::flat_hash_map<Point, int> point_to_idx;
  for (int i = 0; i < input.size(); ++i) {
    Point p;
    {
      Tokenizer t(input[i]);
      RETURN_IF_ERROR(p.From(t));
      if (!t.Done()) return absl::InvalidArgumentError("bad point");
    }
    if (!board.OnBoard(p)) return absl::InvalidArgumentError("off board");
    points.push_back(p);
    point_to_idx[p] = i;
  }

  Point end = {board.width() - 1, board.height() - 1};

  int a = 0;
  int b = points.size();
  while (a < b) {
    int idx = (a + b) / 2;
    std::optional<int> min_steps = PointWalk({
      .start = Point{0, 0},
      .is_good = [&](Point p, int) {
        if (!board.OnBoard(p)) return false;
        auto it = point_to_idx.find(p);
        if (it == point_to_idx.end()) return true;
        return it->second > idx;
      },
      .is_final = [&](Point p, int) {
        return p == end;
      }
    }).FindMinSteps();
    if (min_steps) {
      a = idx + 1;
    } else {
      b = idx;
    }
  }
  if (a == points.size()) return absl::InvalidArgumentError("No blockage");
  return AdventReturn(points[a]);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/18, []() {
      auto ret = std::unique_ptr<AdventDay>(new Day_2024_18());
      ret->set_param("71,1024");
      return ret;
    });

}  // namespace advent_of_code
