// http://adventofcode.com/2023/day/23

#include "advent_of_code/2023/day23/day23.h"

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

using GraphType = absl::flat_hash_map<Point, absl::flat_hash_map<Point, int>>;

GraphType BuildGraph(const CharBoard& b, bool directed = true) {
  GraphType ret;
  for (const auto [p, c] : b) {
    if (c == '#') continue;
    for (Point d : Cardinal::kFourDirs) {
      if (directed && c != '.') {
        if (c == '<' && d != Cardinal::kWest) continue;
        if (c == '>' && d != Cardinal::kEast) continue;
        if (c == '^' && d != Cardinal::kNorth) continue;
        if (c == 'v' && d != Cardinal::kSouth) continue;
      }
      Point t = p + d;
      if (b.OnBoard(t) && b[t] != '#') {
        ret[p].emplace(t, 1);
      }
    }
  }

  VLOG(1) << "Initial graph";

  for (bool work_done = true; work_done;) {
    VLOG(1) << ret.size();
    work_done = false;
    std::vector<Point> iter;
    for (const auto& [p, _] : ret) iter.push_back(p);
    for (Point p : iter) {
      if (ret[p].size() == 2) {
        std::vector<std::pair<Point, int>> out(ret[p].begin(), ret[p].end());
        ret[out[0].first].erase(p);
        ret[out[0].first].emplace(out[1].first, out[0].second + out[1].second);
        ret[out[1].first].erase(p);
        ret[out[1].first].emplace(out[0].first, out[0].second + out[1].second);
        ret.erase(p);
        work_done = true;
      }
    }
  }

  VLOG(1) << "Pruned graph";

  for (const auto& [p1, dset] : ret) {
    for (const auto& [p2, dist] : dset) {
      VLOG(2) << p1 << "-" << p2 << ": " << dist;
    }
  }

  return ret;
}

std::optional<int> FindLongestPath(const GraphType& g, absl::flat_hash_set<Point>& hist, Point cur, Point end) {
  VLOG(2) << cur;
  if (cur == end) return 0;
  std::optional<int> max;
  hist.insert(cur);
  auto it = g.find(cur);
  CHECK(it != g.end());
  for (const auto& [p, d] : it->second) {
    if (hist.contains(p)) continue;
    std::optional<int> sub = FindLongestPath(g, hist, p, end);
    if (sub) {
      if (!max || *max < *sub + d) max = *sub + d;
    }
  }
  hist.erase(cur);
  VLOG(2) << cur << ": " << (max ? *max : -1);
  return max;
}

std::optional<int> FindLongestPath(const GraphType& g, Point start, Point end) {
  absl::flat_hash_set<Point> hist;
  return FindLongestPath(g, hist, start, end);
}

}  // namespace

absl::StatusOr<std::string> Day_2023_23::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  Point start = {-1, -1};
  Point end= {-1, -1};
  for (const auto [p, c] : b) {
    if (c == '.') {
      if (start == Point{-1, -1}) start = p;
      end = p;
    }
  }
  if (end == Point{-1, -1}) return Error("No start/end found");
  return AdventReturn(FindLongestPath(BuildGraph(b), start, end));


  VLOG(1) << "start: " << start;
  struct Path {
    Point cur;
    absl::flat_hash_set<Point> hist;
  };
  int max_dist = 0;
  for (std::deque<Path> queue = {{start, {start}}}; !queue.empty(); queue.pop_front()) {
    Path& cur = queue.front();
    while (true) {
      if (cur.cur.y == b.height() - 1) {
        max_dist = std::max<int>(max_dist, cur.hist.size() - 1);
      }
      std::vector<Point> nexts;
      auto test = [&](Point d) {
        Point t = cur.cur + d;
        if (cur.hist.contains(t)) return;
        if (!b.OnBoard(t)) return;
        if (b[t] == '#') return;
        nexts.push_back(t);
      };
      switch (b[cur.cur]) {
        case '>': test(Cardinal::kEast); break;
        case '<': test(Cardinal::kWest); break;
        case '^': test(Cardinal::kNorth); break;
        case 'v': test(Cardinal::kSouth); break;
        case '.': for (Point d : Cardinal::kFourDirs) { test(d); }; break;
        case '#': {
          return Error("Standing on wall: ", cur.cur);
        }
        default: {
          return Error("Unexpected board: ", cur.cur);
        }
      }
      if (nexts.empty()) break;
      for (int i = 0; i < nexts.size() - 1; ++i) {
        VLOG(2) << "Forked path";
        Path next = cur;
        next.cur = nexts[i];
        next.hist.insert(nexts[i]);
        queue.push_back(next);
      }
      cur.cur = nexts.back();
      cur.hist.insert(nexts.back());
    }
    VLOG(2) << "Done path";
  }
  return AdventReturn(max_dist);
}

absl::StatusOr<std::string> Day_2023_23::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  Point start = {-1, -1};
  Point end= {-1, -1};
  for (const auto [p, c] : b) {
    if (c == '.') {
      if (start == Point{-1, -1}) start = p;
      end = p;
    }
  }
  if (end == Point{-1, -1}) return Error("No start/end found");
  return AdventReturn(FindLongestPath(BuildGraph(b, false), start, end));

  struct Path {
    Point cur;
    absl::flat_hash_set<Point> hist;
  };
  int max_dist = 0;
  for (std::deque<Path> queue = {{start, {start}}}; !queue.empty(); queue.pop_front()) {
    Path& cur = queue.front();
    while (true) {
      if (cur.cur.y == b.height() - 1) {
        max_dist = std::max<int>(max_dist, cur.hist.size() - 1);
      }
      std::vector<Point> nexts;
      auto test = [&](Point d) {
        Point t = cur.cur + d;
        if (cur.hist.contains(t)) return;
        if (!b.OnBoard(t)) return;
        if (b[t] == '#') return;
        nexts.push_back(t);
      };
      switch (b[cur.cur]) {
        case '>':
        case '<':
        case '^':
        case 'v':
        case '.': for (Point d : Cardinal::kFourDirs) { test(d); }; break;
        case '#': {
          return Error("Standing on wall: ", cur.cur);
        }
        default: {
          return Error("Unexpected board: ", cur.cur);
        }
      }
      if (nexts.empty()) break;
      for (int i = 0; i < nexts.size() - 1; ++i) {
        VLOG(2) << "Forked path";
        Path next = cur;
        next.cur = nexts[i];
        next.hist.insert(nexts[i]);
        queue.push_back(next);
      }
      cur.cur = nexts.back();
      cur.hist.insert(nexts.back());
    }
    VLOG(2) << "Done path";
  }
  return AdventReturn(max_dist);
}

}  // namespace advent_of_code
