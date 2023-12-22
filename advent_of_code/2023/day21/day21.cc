// http://adventofcode.com/2023/day/21

#include "advent_of_code/2023/day21/day21.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/node_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
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

absl::StatusOr<int64_t> NaivePart2(
    const CharBoard& b, Point s, int steps) {
  absl::flat_hash_set<Point> set;
  absl::flat_hash_set<Point> frontier = {s};
  if (steps % 2 == 0) {
    set.insert(s);
  } else {
    absl::flat_hash_set<Point> next_frontier;
    for (Point p : frontier) {
      for (Point d1 : Cardinal::kFourDirs) {
        Point next = p + d1;
        if (b[b.TorusPoint(next)] == '#') continue;
        if (set.insert(next).second) {
          next_frontier.insert(next);
        }
      }
    }
    frontier = std::move(next_frontier);
  }
  int64_t removed = 0;
  for (int i = steps % 2; i < steps; i += 2) {
    LOG_EVERY_N_SEC(INFO, 15) << i << ": " << removed << "/" << set.size() << "/" << frontier.size();
    absl::flat_hash_set<Point> next_frontier;
    for (Point p : frontier) {
      for (Point d1 : Cardinal::kFourDirs) {
        if (b[b.TorusPoint(p + d1)] == '#') continue;
        for (Point d2 : Cardinal::kFourDirs) {
          Point next = p + d1 + d2;
          Point test = b.TorusPoint(next);
          if (b[test] == '#') continue;
          if (set.insert(next).second ) {
            next_frontier.insert(next);
          }
        }
      }
    }
    frontier = std::move(next_frontier);
  } 
  absl::flat_hash_map<Point, int> grid_count;
  for (Point p : set) {
    Point g = {
      static_cast<int>(floor(1.0 * p.x / b.width())),
      static_cast<int>(floor(1.0 * p.y / b.height()))
    };
    ++grid_count[g];
  }
  for (const auto& [p, c] : grid_count) {
    VLOG(1) << p << ": " << c;
  }
  return removed + set.size();
}

absl::flat_hash_map<Point, int> Paint(
    const CharBoard& b, Point start, int start_d) {
  absl::flat_hash_map<Point, int> ret = {{start, start_d}};
  for (std::deque<Point> queue = {start}; !queue.empty(); queue.pop_front()) {
    auto it = ret.find(queue.front());
    CHECK(it != ret.end());
    for (Point d : Cardinal::kFourDirs) {
      Point t = it->first + d;
      if (!b.OnBoard(t) || b[t] == '#') continue;
      if (ret.emplace(t, it->second + 1).second) {
        queue.push_back(t);
      }
    }
  }
  return ret;
}

int64_t CountPainted(const absl::flat_hash_map<Point, int>& paint, int steps) {
  int64_t count = 0;
  for (const auto& [_, d] : paint) {
    if (d % 2 == steps % 2 && d <= steps) {
      ++count;
    }
  }
  return count;
}

std::optional<int64_t> HackPart2(
    const CharBoard& b, Point s, int steps) {
  if (b.width() != b.height()) return std::nullopt;
  if (b.width() % 2 != 1) return std::nullopt;
  if (s.x != b.width() / 2) return std::nullopt;
  if (s.y != b.height() / 2) return std::nullopt;
  if (steps % b.width() == 0) return std::nullopt;

  absl::flat_hash_map<Point, int> start_paint = Paint(b, s, 0);
  for (int i = 0; i < b.width(); ++i) {
    int test_dist = (s - Point{0, i}).dist();
    if ((start_paint[{0, i}]) != test_dist) return std::nullopt;
    if ((start_paint[{i, 0}]) != test_dist) return std::nullopt;
    if ((start_paint[{b.width() - 1, i}]) != test_dist) return std::nullopt;
    if ((start_paint[{i, b.height() - 1}]) != test_dist) return std::nullopt;
  }
  int64_t total = 0;

  int64_t max_x = s.x + steps;
  //  ./#
  // ./##
  // <### ...
  VLOG(1) << max_x % b.width() << " >?> " << s.x;
  int64_t max_tile_x = max_x / b.width();

  // steps - max_x % b.width() + s.y < steps

  // s.x=s.y=66, w=h=131, steps=300
  // s.x + steps = 366
  // 366 % 131 = 104
  // 

  bool extra_corner = false;
  if (max_x % b.width() < s.x) {
    extra_corner = true;
    --max_tile_x;
  }

  int64_t full_paint_count = CountPainted(start_paint, steps);
  VLOG(1) << "Full: " << full_paint_count;

  // 1, 5, 13, 25
  // 1 + SUM(4n)  
  // 1 + 2n(n+1)
  int64_t full_max_tile_x = (max_tile_x + 1) / 2;
  int64_t full_off_max_tile_x = max_tile_x - full_max_tile_x;
  VLOG(1) << max_tile_x << " -> " << full_max_tile_x << " + " << full_off_max_tile_x;
  int64_t full_tile_count = 1 + 4 * full_max_tile_x * (full_max_tile_x - 1);
  VLOG(1) << " x" << full_tile_count;
  total += full_tile_count * full_paint_count;

  // Full tiles are offset on which step mod they use.
  // SUM(4 * (2n + 1))
  int64_t full_off_paint_count = CountPainted(start_paint, steps - 1);
  VLOG(1) << "Full (off): " << full_off_paint_count;
  int64_t full_off_tile_count = 4 * full_off_max_tile_x * (full_off_max_tile_x - 1) + 4 * full_off_max_tile_x;
  VLOG(1) << " x" << full_off_tile_count;
  total += full_off_tile_count * full_off_paint_count;


  {
    int mid_edge_dist = steps - max_x % b.width();
    std::vector<std::pair<std::string, Point>> corner_tiles = {
      {"Right Corner", {0, s.y}},
      {"Bottom Corner", {s.x, 0}},
      {"Left Corner", {b.width() - 1, s.y}},
      {"Top Corner", {s.x, b.height() - 1}},
    };
    for (const auto& [name, start] : corner_tiles) {
      absl::flat_hash_map<Point, int> paint = Paint(b, start, mid_edge_dist);
      int64_t count = CountPainted(paint, steps);
      VLOG(1) << name << ": " << count;
      total += count;
    }
  }
  if (extra_corner) {
    int mid_edge_dist = steps - max_x % b.width() - b.width();
    std::vector<std::pair<std::string, Point>> corner_tiles = {
      {"Right Corner (Extra)", {0, s.y}},
      {"Bottom Corner (Extra)", {s.x, 0}},
      {"Left Corner (Extra)", {b.width() - 1, s.y}},
      {"Top Corner (Extra)", {s.x, b.height() - 1}},
    };
    for (const auto& [name, start] : corner_tiles) {
      absl::flat_hash_map<Point, int> paint = Paint(b, start, mid_edge_dist);
      int64_t count = CountPainted(paint, steps);
      VLOG(1) << name << ": " << count;
      total += count;
    }
  }
  {
    int corner_dist = steps - max_x % b.width() + s.y + 1;
    if (extra_corner) {
      corner_dist -= b.width();
    }
    // CHECK_LT(corner_dist, steps);
    std::vector<std::pair<std::string, Point>> corner_tiles = {
      {"UR Edge Small", {0, b.height() - 1}},
      {"BR Edge Small", {0, 0}},
      {"UL Edge Small", {b.width() - 1, b.height() - 1}},
      {"BL Edge Small", {b.width() - 1, 0}},
    };
    for (const auto& [name, start] : corner_tiles) {
      absl::flat_hash_map<Point, int> paint = Paint(b, start, corner_dist);
      int64_t count = CountPainted(paint, steps);
      VLOG(1) << name << ": " << count;
      VLOG(1) << " x" << max_tile_x;
      total += count * max_tile_x;
    }
  }

  {
    int corner_dist = steps - max_x % b.width() + s.y + 1 - b.width();
    if (extra_corner) {
      corner_dist -= b.width();
    }
    // CHECK_LT(corner_dist, steps);
    std::vector<std::pair<std::string, Point>> corner_tiles = {
      {"UR Edge Large", {0, b.height() - 1}},
      {"BR Edge Large", {0, 0}},
      {"UL Edge Large", {b.width() - 1, b.height() - 1}},
      {"BL Edge Large", {b.width() - 1, 0}},
    };
    for (const auto& [name, start] : corner_tiles) {
      absl::flat_hash_map<Point, int> paint = Paint(b, start, corner_dist);
      int64_t count = CountPainted(paint, steps);
      VLOG(1) << name << ": " << count;
      VLOG(1) << " x" << max_tile_x - 1;
      total += count * (max_tile_x - 1);
    }
  }

  return total;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_21::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  auto [p1, p2] = PairSplit(param(), ",");
  int steps;
  if (!absl::SimpleAtoi(p1, &steps)) {
    return Error("Bad int param: ", p1);
  }
  absl::flat_hash_set<Point> s = b.Find('S');
  if (s.size() != 1) return Error("Start not unique");
  for (int i = 0; i < steps; ++i) {
    absl::flat_hash_set<Point> n;
    for (Point p : s) {
      for (Point d : Cardinal::kFourDirs) {
        Point t = p + d;
        if (b.OnBoard(t) && b[t] != '#') {
          n.insert(t);
        }
      }
    }
    s = std::move(n);
  }

  return AdventReturn(s.size());
}

absl::StatusOr<std::string> Day_2023_21::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  auto [p1, p2] = PairSplit(param(), ",");
  int steps;
  if (!absl::SimpleAtoi(p2.empty() ? p1 : p2, &steps)) {
    return Error("Bad int param: ", p2.empty() ? p1 : p2);
  }

  ASSIGN_OR_RETURN(Point s, b.FindUnique('S'));
  std::optional<int64_t> maybe_anser = HackPart2(b, s, steps);
  if (maybe_anser) return AdventReturn(*maybe_anser);

  return AdventReturn(NaivePart2(b, s, steps));
}

}  // namespace advent_of_code
