// http://adventofcode.com/2022/day/17

#include "advent_of_code/2022/day17/day17.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Rock {
  Point off;
  std::vector<Point> delta;

  bool TryMove(PointRectangle bounds, const absl::flat_hash_set<Point>& stopped, Point d) {
    for (Point p : delta) {
      Point t = p + off + d;
      if (!bounds.Contains(t)) return false;
      if (stopped.contains(t)) return false;
    }
    off += d;
    return true;
  }
};

CharBoard Draw(const absl::flat_hash_set<Point>& stopped, int height) {
  CharBoard b(7, height);
  for (Point p : stopped) {
    b[p] = '#';
  }
  return b;
}

CharBoard AddPiece(CharBoard b, Rock r) {
  for (Point p : r.delta) {
    b[p + r.off] = '@';
  }
  return b;
}

struct PointLt {
  const bool operator()(Point a, Point b) const {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
  }
};

CharBoard Draw(const std::set<Point, PointLt>& stopped) {
  PointRectangle r = {*stopped.begin(), *stopped.begin()};
  for (Point p : stopped) r.ExpandInclude(p);
  CharBoard b(r);
  for (Point p : stopped) {
    b[p - r.min] = '#';
  }
  return b;
}

struct State {
  int w_idx;
  int r_idx;
  std::set<Point, PointLt> bounds;

  bool operator==(const State& o) const {
    return w_idx == o.w_idx && r_idx == o.r_idx && bounds == o.bounds;
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.w_idx, s.r_idx, s.bounds);
  }
};

State Summarize(const absl::flat_hash_set<Point>& stopped, int w_idx, int r_idx, int height) {
  State ret = {.w_idx = w_idx, .r_idx = r_idx, .bounds = {}};

  PointRectangle bounds = {{0, 0}, {6, height}};

  Point start = {0, height};
  absl::flat_hash_set<Point> visited = {start};
  for (std::deque queue = {{start}}; !queue.empty(); queue.pop_front()) {
    for (Point d : Cardinal::kFourDirs) {
      Point t = d + queue.front();
      if (!bounds.Contains(t)) continue;
      if (stopped.contains(t)) {
        ret.bounds.insert(t - start);
        continue;
      }
      if (visited.insert(t).second) {
        queue.push_back(t);
      }
    }
  }
  return ret;
}

int64_t Extrapolate(
    const std::vector<int>& heights, int rep_start, int rep_end) {

}

}  // namespace

absl::StatusOr<std::string> Day_2022_17::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<Rock> kRockTypes = {
    Rock{.off = {0, 0}, .delta = {{0, 0}, {1, 0}, {2, 0}, {3, 0}}},
    Rock{.off = {0, 0}, .delta = {{2, 1}, {1, 0}, {1, 1}, {1, 2}, {0, 1}}},
    Rock{.off = {0, 0}, .delta = {{2, 2}, {2, 1}, {0, 0}, {1, 0}, {2, 0}}},
    Rock{.off = {0, 0}, .delta = {{0, 3}, {0, 2}, {0, 1}, {0, 0}}},
    Rock{.off = {0, 0}, .delta = {{1, 0}, {1, 1}, {0, 0}, {0, 1}}},
  };
  std::vector<Point> wind;
  for (char c : input[0]) {
    if (c == '<') wind.push_back(Cardinal::kWest);
    else if (c == '>') wind.push_back(Cardinal::kEast);
    else return Error("Bad wind");
  }
  // Drop rocks.
  absl::flat_hash_set<Point> stopped;
  int w_idx = -1;
  int r_idx = -1;
  int height = 0;
  PointRectangle bounds = {{0, 0}, {6, 2022 * 10}};
  absl::flat_hash_map<State, int> state_to_idx;
  std::vector<int> heights;
  for (int i = 0; true; ++i) {
    r_idx = (r_idx + 1) % kRockTypes.size();
    Rock drop = kRockTypes[r_idx];
    drop.off = {2, height + 3};
    if (!bounds.Contains(drop.off + Point{4, 4})) return Error("Bad bounds");
    while (true) {
      w_idx = (w_idx + 1) % wind.size();
      Point d = wind[w_idx];
      VLOG(2) << "Drop: " << drop.off << "; Wind: " << d;
      drop.TryMove(bounds, stopped, d);
      if (!drop.TryMove(bounds, stopped, Cardinal::kNorth)) {
        break;
      }
    }
    for (Point p : drop.delta) {
      Point t = p + drop.off;
      if (!stopped.insert(t).second) return Error("Double insert!");
      height = std::max(height, t.y + 1);
    }
    heights.push_back(height);
    auto [it, inserted] = state_to_idx.emplace(Summarize(stopped, w_idx, r_idx, height), i);
    VLOG(2) << "Summary:\n" << Draw(it->first.bounds);
    if (!inserted) {
      VLOG(1) << "Repeat @" << i << " == " << it->second;
      int64_t find = 1000000000000 - 1;
      int prev_i = it->second;

      int64_t delta_h = heights[i] - heights[prev_i];

      int64_t ret_height = heights[prev_i];
      find -= prev_i;
      ret_height += delta_h * (find / (i - prev_i));
      find %= (i - prev_i);
      ret_height += heights[find + prev_i] - heights[prev_i];

      VLOG(1) << "Return " << ret_height;

      return IntReturn(ret_height);
    }
  }
  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2022_17::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::vector<Rock> kRockTypes = {
    Rock{.off = {0, 0}, .delta = {{0, 0}, {1, 0}, {2, 0}, {3, 0}}},
    Rock{.off = {0, 0}, .delta = {{2, 1}, {1, 0}, {1, 1}, {1, 2}, {0, 1}}},
    Rock{.off = {0, 0}, .delta = {{2, 2}, {2, 1}, {0, 0}, {1, 0}, {2, 0}}},
    Rock{.off = {0, 0}, .delta = {{0, 3}, {0, 2}, {0, 1}, {0, 0}}},
    Rock{.off = {0, 0}, .delta = {{1, 0}, {1, 1}, {0, 0}, {0, 1}}},
  };
  std::vector<Point> wind;
  for (char c : input[0]) {
    if (c == '<') wind.push_back(Cardinal::kWest);
    else if (c == '>') wind.push_back(Cardinal::kEast);
    else return Error("Bad wind");
  }
  // Drop rocks.
  absl::flat_hash_set<Point> stopped;
  int w_idx = -1;
  int r_idx = -1;
  int height = 0;
  PointRectangle bounds = {{0, 0}, {6, std::numeric_limits<int>::max()}};
  for (int i = 0; i < 2022; ++i) {
    r_idx = (r_idx + 1) % kRockTypes.size();
    Rock drop = kRockTypes[r_idx];
    drop.off = {2, height + 3};
    if (!bounds.Contains(drop.off + Point{4, 4})) return Error("Bad bounds");
    while (true) {
      w_idx = (w_idx + 1) % wind.size();
      Point d = wind[w_idx];
      VLOG(2) << "Drop: " << drop.off << "; Wind: " << d;
      if (i == 13) {
        VLOG(2) << "Board (" << i << "):\n" << AddPiece(Draw(stopped, height + 10), drop);
      }
      drop.TryMove(bounds, stopped, d);
      if (!drop.TryMove(bounds, stopped, Cardinal::kNorth)) {
        break;
      }
    }
    for (Point p : drop.delta) {
      Point t = p + drop.off;
      if (!stopped.insert(t).second) return Error("Double insert!");
      height = std::max(height, t.y + 1);
    }
    VLOG(2) << "Board (" << i << "):\n" << Draw(stopped, height);
  }
  return IntReturn(height);
}

}  // namespace advent_of_code
