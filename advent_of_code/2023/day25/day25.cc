// http://adventofcode.com/2023/day/25

#include "advent_of_code/2023/day25/day25.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

std::optional<int> FindDisjointSize(const std::vector<std::vector<int>>& graph,
                                    std::vector<std::vector<bool>>& skip,
                                    int start, const std::vector<bool>& used) {
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
    const std::vector<std::vector<int>>& graph, int start = 0) {
  std::vector<std::pair<int, int>> ret;
  std::vector<bool> used(graph.size(), false);
  used[start] = true;
  for (std::deque<int> queue = {start}; !queue.empty(); queue.pop_front()) {
    int i = queue.front();
    for (int o : graph[i]) {
      if (used[o]) continue;
      used[o] = true;
      queue.push_back(o);
      if (i < o)
        ret.emplace_back(i, o);
      else
        ret.emplace_back(o, i);
    }
  }

  return ret;
}

std::optional<std::pair<int, int>> FindThird(
    const std::vector<std::vector<int>>& graph,
    const absl::flat_hash_map<std::pair<int, int>, int>& edge_idx,
    int max_edge_idx, std::vector<std::vector<bool>>& skip) {
  std::vector<bool> used(graph.size(), false);
  used[0] = true;
  for (std::deque<int> queue = {0}; !queue.empty(); queue.pop_front()) {
    int i = queue.front();
    for (int o : graph[i]) {
      if (skip[i][o]) continue;
      if (used[o]) continue;
      std::pair<int, int> edge = {i, o};
      if (i > o) edge = {o, i};
      if (auto it = edge_idx.find(edge);
          it != edge_idx.end() && it->second < max_edge_idx) {
        skip[i][o] = true;
        skip[o][i] = true;
        std::optional<int> disjoint_rest =
            FindDisjointSize(graph, skip, o, used);
        if (disjoint_rest) {
          return std::pair<int, int>{*disjoint_rest,
                                     graph.size() - *disjoint_rest};
        }
        skip[i][o] = false;
        skip[o][i] = false;
      }
      used[o] = true;
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
    Tokenizer tok(line);
    std::string_view a_str = tok.Next();
    auto [it_a, a_inserted] = name_to_id.emplace(a_str, name_to_id.size());
    if (a_inserted) {
      graph.push_back({});
      names.push_back(a_str);
    }
    RETURN_IF_ERROR(tok.NextIs(":"));
    int a = it_a->second;
    while (!tok.Done()) {
      std::string_view b_str = tok.Next();
      auto [it_b, b_inserted] = name_to_id.emplace(b_str, name_to_id.size());
      if (b_inserted) {
        graph.push_back({});
        names.push_back(a_str);
      }
      int b = it_b->second;
      if (a < b)
        edges.emplace_back(a, b);
      else
        edges.emplace_back(b, a);
      graph[a].push_back(b);
      graph[b].push_back(a);
    }
  }

  absl::flat_hash_map<std::pair<int, int>, int> edge_to_span_count;
  for (int i = 0; i < graph.size(); i += 100) {
    std::vector<std::pair<int, int>> tree = SpanningTree(graph, i);
    for (const auto& pair : tree) {
      ++edge_to_span_count[pair];
    }
  }

  absl::c_sort(edges, [&](std::pair<int, int> a, std::pair<int, int> b) {
    return edge_to_span_count[a] > edge_to_span_count[b];
  });

  VLOG(1) << "nodes=" << graph.size() << "/edges=" << edges.size();
  absl::flat_hash_map<std::pair<int, int>, int> edge_idx;
  for (int i = 0; i < edges.size(); ++i) {
    edge_idx[edges[i]] = i;
  }

  std::vector<std::vector<bool>> skip(graph.size(),
                                      std::vector<bool>(graph.size(), false));
  for (int i = 0; i < edges.size(); ++i) {
    LOG_EVERY_N_SEC(INFO, 15) << i << "/" << edges.size();
    skip[edges[i].first][edges[i].second] = true;
    skip[edges[i].second][edges[i].first] = true;
    for (int j = 0; j < i; ++j) {
      skip[edges[j].first][edges[j].second] = true;
      skip[edges[j].second][edges[j].first] = true;

      std::optional<std::pair<int, int>> ret =
          FindThird(graph, edge_idx, j, skip);
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

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/25,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_25()); });

}  // namespace advent_of_code
