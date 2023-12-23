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

class BoardGraph {
 public:
  BoardGraph() = default;

  void Build(const CharBoard& b, bool directed = true);

  std::optional<int> FindLongestPath(Point start, Point end) const;

 private:
  std::optional<int> FindLongestPath(absl::flat_hash_set<Point>& hist, Point start, Point end) const;

  absl::flat_hash_map<Point, absl::flat_hash_map<Point, int>> map_;
};

void BoardGraph::Build(const CharBoard& b, bool directed) {
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
        map_[p].emplace(t, 1);
      }
    }
  }

  VLOG(1) << "Initial graph";

  for (bool work_done = true; work_done;) {
    VLOG(1) << map_.size();
    work_done = false;
    std::vector<Point> iter;
    for (const auto& [p, _] : map_) iter.push_back(p);
    for (Point p : iter) {
      if (map_[p].size() == 2) {
        std::vector<std::pair<Point, int>> out(map_[p].begin(), map_[p].end());
        map_[out[0].first].erase(p);
        map_[out[0].first].emplace(out[1].first, out[0].second + out[1].second);
        map_[out[1].first].erase(p);
        map_[out[1].first].emplace(out[0].first, out[0].second + out[1].second);
        map_.erase(p);
        work_done = true;
      }
    }
  }

  VLOG(1) << "Pruned graph";

  for (const auto& [p1, dset] : map_) {
    for (const auto& [p2, dist] : dset) {
      VLOG(2) << p1 << "-" << p2 << ": " << dist;
    }
  }
}

std::optional<int> BoardGraph::FindLongestPath(absl::flat_hash_set<Point>& hist, Point cur, Point end) const {
  VLOG(2) << cur;
  if (cur == end) return 0;
  std::optional<int> max;
  hist.insert(cur);
  auto it = map_.find(cur);
  CHECK(it != map_.end());
  for (const auto& [p, d] : it->second) {
    if (hist.contains(p)) continue;
    std::optional<int> sub = FindLongestPath(hist, p, end);
    if (sub) {
      if (!max || *max < *sub + d) max = *sub + d;
    }
  }
  hist.erase(cur);
  VLOG(2) << cur << ": " << (max ? *max : -1);
  return max;
}

std::optional<int> BoardGraph::FindLongestPath(Point start, Point end) const {
  absl::flat_hash_set<Point> hist;
  return FindLongestPath(hist, start, end);
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
  BoardGraph g;
  g.Build(b);
  return AdventReturn(g.FindLongestPath(start, end));
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
  BoardGraph g;
  g.Build(b, false);
  return AdventReturn(g.FindLongestPath(start, end));
}

}  // namespace advent_of_code
