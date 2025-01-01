// http://adventofcode.com/2024/day/18

#include "advent_of_code/2024/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_18::Part1(
    absl::Span<std::string_view> input) const {
  PointRectangle bounds = {{0, 0}, {0, 0}};
  Point end;
  int steps = 0;
  {
    Tokenizer t(param());
    ASSIGN_OR_RETURN(int dim, t.NextInt());
    end = {dim - 1, dim - 1};
    bounds.ExpandInclude(end);
    RETURN_IF_ERROR(t.NextIs(","));
    ASSIGN_OR_RETURN(steps, t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  if (input.size() < steps) return absl::InvalidArgumentError("bad steps");

  absl::flat_hash_set<Point> points;
  for (int i = 0; i < steps; ++i) {
    Point p;
    {
      Tokenizer t(input[i]);
      RETURN_IF_ERROR(p.From(t));
      RETURN_IF_ERROR(t.AssertDone());
    }
    if (!bounds.Contains(p)) return absl::InvalidArgumentError("off board");
    points.insert(p);
  }
  return AdventReturn(PointWalk({
    .start = Point{0, 0},
    .is_good = [&](Point p, int) {
      return bounds.Contains(p) && !points.contains(p);
    },
    .is_final = [&](Point p, int) {
      return p == end;
    }
  }).FindMinSteps());
}

absl::StatusOr<std::string> Day_2024_18::Part2(
    absl::Span<std::string_view> input) const {
  PointRectangle bounds = {{0, 0}, {0, 0}};
  Point end;
  int steps = 0;
  {
    Tokenizer t(param());
    ASSIGN_OR_RETURN(int dim, t.NextInt());
    end = {dim - 1, dim - 1};
    bounds.ExpandInclude(end);
    RETURN_IF_ERROR(t.NextIs(","));
    ASSIGN_OR_RETURN(steps, t.NextInt());
    RETURN_IF_ERROR(t.AssertDone());
  }
  if (input.size() < steps) return absl::InvalidArgumentError("bad steps");

  std::vector<Point> points;
  absl::flat_hash_map<Point, int> point_to_idx;
  for (int i = 0; i < input.size(); ++i) {
    Point p;
    {
      Tokenizer t(input[i]);
      RETURN_IF_ERROR(p.From(t));
      RETURN_IF_ERROR(t.AssertDone());
    }
    if (!bounds.Contains(p)) return absl::InvalidArgumentError("off board");
    points.push_back(p);
    point_to_idx[p] = i;
  }

  int a = 0;
  int b = points.size();
  while (a < b) {
    int idx = (a + b) / 2;
    std::optional<int> min_steps = PointWalk({
      .start = Point{0, 0},
      .is_good = [&](Point p, int) {
        if (!bounds.Contains(p)) return false;
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
