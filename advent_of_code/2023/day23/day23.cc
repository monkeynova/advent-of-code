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

  std::optional<int> FindLongestPath(Point start, Point end);

 private:
  std::optional<int> FindLongestPath(std::vector<bool>& hist, int start, int end);
  std::optional<int> FindLongestPath(int64_t hist, int start, int end);

  absl::flat_hash_map<Point, int> point_to_idx_;
  std::vector<std::vector<std::pair<int, int>>> map_;
  int calls_ = 0;
};

void BoardGraph::Build(const CharBoard& b, bool directed) {
  absl::flat_hash_map<Point, absl::flat_hash_map<Point, int>> by_point;

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
        by_point[p].emplace(t, 1);
      }
    }
  }

  VLOG(1) << "Initial graph";

  for (bool work_done = true; work_done;) {
    VLOG(1) << by_point.size();
    work_done = false;
    std::vector<Point> iter;
    for (const auto& [p, _] : by_point) iter.push_back(p);
    for (Point p : iter) {
      if (by_point[p].size() == 2) {
        std::vector<std::pair<Point, int>> out(by_point[p].begin(), by_point[p].end());
        by_point[out[0].first].erase(p);
        CHECK(by_point[out[0].first].emplace(out[1].first, out[0].second + out[1].second).second)
          << "Double path resolution not implemented";
        by_point[out[1].first].erase(p);
        CHECK(by_point[out[1].first].emplace(out[0].first, out[0].second + out[1].second).second)
          << "Double path resolution not implemented";
        by_point.erase(p);
        work_done = true;
      }
    }
  }

  VLOG(1) << "Pruned graph";
  
  point_to_idx_.clear();
  map_.clear();

  for (const auto& [p, _] : by_point) {
    point_to_idx_[p] = point_to_idx_.size();
  }
  map_.resize(point_to_idx_.size());

  VLOG(1) << point_to_idx_.size();

  for (const auto& [p1, dset] : by_point) {
    auto it1 = point_to_idx_.find(p1);
    CHECK(it1 != point_to_idx_.end());
    std::vector<std::pair<int, int>>& map_build = map_[it1->second];
    for (const auto& [p2, dist] : dset) {
      VLOG(2) << p1 << "-" << p2 << ": " << dist;
      auto it2 = point_to_idx_.find(p2);
      CHECK(it2 != point_to_idx_.end());
      map_build.emplace_back(it2->second, dist);
    }
  }
}

std::optional<int> BoardGraph::FindLongestPath(std::vector<bool>& hist, int cur, int end) {
  ++calls_;
  VLOG(3) << cur;
  if (cur == end) return 0;

  std::optional<int> max;
  hist[cur] = true;
  CHECK_GE(cur, 0);
  CHECK_LT(cur, map_.size());
  for (const auto& [p, d] : map_[cur]) {
    if (hist[p]) continue;
    std::optional<int> sub = FindLongestPath(hist, p, end);
    if (sub) {
      if (!max || *max < *sub + d) max = *sub + d;
    }
  }
  hist[cur] = false;
  VLOG(3) << cur << ": " << (max ? *max : -1);
  return max;
}

std::optional<int> BoardGraph::FindLongestPath(int64_t hist, int cur, int end) {
  ++calls_;
  VLOG(3) << cur;
  if (cur == end) return 0;

  std::optional<int> max;
  hist |= (int64_t{1} << cur);
  CHECK_GE(cur, 0);
  CHECK_LT(cur, 63);
  for (const auto& [p, d] : map_[cur]) {
    if (hist & (int64_t{1} << p)) continue;
    std::optional<int> sub = FindLongestPath(hist, p, end);
    if (sub) {
      if (!max || *max < *sub + d) max = *sub + d;
    }
  }
  VLOG(3) << cur << ": " << (max ? *max : -1);
  return max;
}

std::optional<int> BoardGraph::FindLongestPath(Point start, Point end) {
  int start_idx = point_to_idx_[start];
  int end_idx = point_to_idx_[end];
  std::optional<int> ret;
  if (point_to_idx_.size() < 63) {
    ret = FindLongestPath(0, start_idx, end_idx);
  } else {
    std::vector<bool> hist(point_to_idx_.size(), false);
    ret= FindLongestPath(hist, start_idx, end_idx);
  }
  VLOG(1) << calls_ << " calls to FindLongestPath";
  return ret;
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
