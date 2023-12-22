// http://adventofcode.com/2023/day/21

#include "advent_of_code/2023/day21/day21.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

absl::StatusOr<int64_t> NaivePart2(
    const ImmutableCharBoard& b, Point s, int steps) {
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
    const ImmutableCharBoard& b, Point start, int start_d) {
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

int64_t PaintAndCount(const ImmutableCharBoard& b, Point start, int start_d, int steps) {
  if (start_d > steps) return 0;
  FastBoard fb(b);
  FastBoard::PointMap<int> paint(fb, -1);
  FastBoard::Point s_idx = fb.From(start);
  paint.Set(s_idx, start_d);
  int count = start_d % 2 == steps % 2;
  for (std::deque<FastBoard::Point> queue = {s_idx}; !queue.empty(); queue.pop_front()) {
    int dist = paint.Get(queue.front());
    CHECK_NE(dist, -1);
    if (dist == steps) break;
    ++dist;
    for (FastBoard::Dir d : {FastBoard::kNorth, FastBoard::kSouth, FastBoard::kEast, FastBoard::kWest}) {
      FastBoard::Point test = fb.Add(queue.front(), d);
      if (!fb.OnBoard(test)) continue;
      if (fb[test] == '#') continue;
      if (paint.Get(test) != -1) continue;
      paint.Set(test, dist);
      queue.push_back(test);
      if (dist % 2 == steps % 2) {
        CHECK_LE(dist, steps);
        ++count;
      }
    }
  }
  return count;
}

std::optional<int64_t> HackPart2(
    const ImmutableCharBoard& b, Point s, int steps) {
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
      int64_t count = PaintAndCount(b, start, mid_edge_dist, steps);
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
      int64_t count = PaintAndCount(b, start, mid_edge_dist, steps);
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
      int64_t count = PaintAndCount(b, start, corner_dist, steps);
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
      int64_t count = PaintAndCount(b, start, corner_dist, steps);
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
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  auto [p1, p2] = PairSplit(param(), ",");
  int steps;
  if (!absl::SimpleAtoi(p1, &steps)) {
    return Error("Bad int param: ", p1);
  }
  ASSIGN_OR_RETURN(Point s, b.FindUnique('S'));
  FastBoard fb(b);
  FastBoard::PointMap<int> dist(fb, std::numeric_limits<int>::max());
  FastBoard::Point s_idx = fb.From(s);
  dist.Set(s_idx, 0);
  int count = steps % 2 == 0 ? 1 : 0;
  for (std::deque<FastBoard::Point> queue = {s_idx}; !queue.empty(); queue.pop_front()) {
    int next_d = dist.Get(queue.front()) + 1;
    if (next_d > steps) break;
    for (FastBoard::Dir d : {FastBoard::kNorth, FastBoard::kSouth, FastBoard::kWest, FastBoard::kEast}) {
      FastBoard::Point test = fb.Add(queue.front(), d);
      if (!fb.OnBoard(test)) continue;
      if (fb[test] == '#') continue;
      if (dist.Get(test) <= next_d) continue;
      if (next_d % 2 == steps % 2) {
        ++count;
      }
      dist.Set(test, next_d);
      queue.push_back(test);
    }
  }

  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2023_21::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
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
