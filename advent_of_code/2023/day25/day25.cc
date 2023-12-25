// http://adventofcode.com/2023/day/25

#include "advent_of_code/2023/day25/day25.h"

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

std::optional<std::pair<int, int>> IsForestPair(
    const std::vector<std::vector<int>>& graph,
    const std::vector<std::vector<bool>>& skip) {
  std::vector<bool> used(graph.size(), false);
  used[0] = true;
  int size = 1;
  for (std::deque<int> queue = {0}; !queue.empty(); queue.pop_front()) {
    for (int o : graph[queue.front()]) {
      if (skip[queue.front()][o]) continue;
      if (used[o]) continue;
      used[o] = true;
      ++size;
      queue.push_back(o);
    }
  }
  if (size == graph.size()) return std::nullopt;
  return std::pair<int, int>{size, graph.size() - size};
}

std::optional<int> FindDisjointSize(
    const std::vector<std::vector<int>>& graph,
    std::vector<std::vector<bool>>& skip,
    int start,
    const std::vector<bool>& used) {
  std::vector<bool> used2(graph.size(), false);
  used2[start] = true;
  int used2_size = 1;
  for (std::deque<int> queue = {start}; !queue.empty(); queue.pop_front()) {
    int i = queue.front();
    for (int o : graph[i]) {
      if (skip[i][o]) continue;
      if (used2[o]) continue;
      if (used[o]) return std::nullopt;
      used2[o] = true;
      ++used2_size;
      queue.push_back(o);
    }
  }  

  return used2_size;
}

std::vector<std::pair<int, int>> SpanningTree(
    const std::vector<std::vector<int>>& graph) {
  std::vector<std::pair<int, int>> ret;
  std::vector<bool> used(graph.size(), false);
  used[0] = true;
  for (std::deque<int> queue = {0}; !queue.empty(); queue.pop_front()) {
    int i = queue.front();
    for (int o : graph[i]) {
      if (used[o]) continue;
      used[o] = true;
      queue.push_back(o);
      ret.emplace_back(i, o);
    }
  }  

  return ret;
}

std::optional<std::pair<int, int>> FindThird(
    const std::vector<std::vector<int>>& graph,
    const std::vector<std::vector<int>>& edge_idx,
    int max_edge_idx,
    std::vector<std::vector<bool>>& skip) {
  std::vector<bool> used(graph.size(), false);
  used[0] = true;
  int used_count = 1;
  for (std::deque<int> queue = {0}; !queue.empty(); queue.pop_front()) {
    int i = queue.front();
    for (int o : graph[i]) {
      if (skip[i][o]) continue;
      if (used[o]) continue;
      if (edge_idx[i][o] < max_edge_idx) {
        skip[i][o] = true;
        skip[o][i] = true;
        std::optional<int> disjoint_rest = FindDisjointSize(graph, skip, o, used);
        if (disjoint_rest) {
          return std::pair<int,int>{*disjoint_rest, graph.size() - *disjoint_rest};
        }
        skip[i][o] = false;
        skip[o][i] = false;
      }
      used[o] = true;
      ++used_count;
      queue.push_back(o);
    }
  }
  return std::nullopt;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_25::Part1(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<std::string_view, int> name_to_id;
  std::vector<std::vector<int>> graph;
  std::vector<std::string_view> names;
  std::vector<std::pair<int, int>> edges;
  for (std::string_view line : input) {
    auto [a_str, list] = PairSplit(line, ": ");
    auto [it_a, a_inserted] = name_to_id.emplace(a_str, name_to_id.size());
    if (a_inserted) {
      graph.push_back({});
      names.push_back(a_str);
    }
    int a = it_a->second;
    for (std::string_view b_str : absl::StrSplit(list, " ")) {
      auto [it_b, b_inserted] = name_to_id.emplace(b_str, name_to_id.size());
      if (b_inserted) {
        graph.push_back({});
        names.push_back(a_str);
      }
      int b = it_b->second;
      edges.emplace_back(a, b);
      graph[a].push_back(b);
      graph[b].push_back(a);
    }
  }

  VLOG(1) << "nodes=" << graph.size() << "/edges=" << edges.size();
  std::vector<std::vector<int>> edge_idx(graph.size(), std::vector(graph.size(), -1));
  for (int i = 0; i < edges.size(); ++i) {
    edge_idx[edges[i].first][edges[i].second] = i;
    edge_idx[edges[i].second][edges[i].first] = i;
  }

  std::vector<std::vector<bool>> skip(graph.size(), std::vector<bool>(graph.size(), false));
  for (int i = 0; i < edges.size(); ++i) {
    LOG_EVERY_N_SEC(INFO, 15) << i << "/" << edges.size();
    skip[edges[i].first][edges[i].second] = true;
    skip[edges[i].second][edges[i].first] = true;
    for (int j = 0; j < i; ++j) {
      skip[edges[j].first][edges[j].second] = true;
      skip[edges[j].second][edges[j].first] = true;

      std::optional<std::pair<int, int>> ret = FindThird(graph, edge_idx, j, skip);
      if (ret) {
        return AdventReturn(ret->first * ret->second);
      }

      skip[edges[j].first][edges[j].second] = false;
      skip[edges[j].second][edges[j].first] = false;
    }
    skip[edges[i].first][edges[i].second] = false;
    skip[edges[i].second][edges[i].first] = false;
  }

  return AdventReturn(Error("Not found"));
}

absl::StatusOr<std::string> Day_2023_25::Part2(
    absl::Span<std::string_view> input) const {
  return AdventReturn("Merry Christmas!");
}

}  // namespace advent_of_code
