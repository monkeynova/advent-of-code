// http://adventofcode.com/2024/day/23

#include "advent_of_code/2024/day23/day23.h"

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

std::vector<std::string_view> FindBiggest(
    absl::Span<const std::string_view> nodes,
    const absl::flat_hash_set<std::pair<std::string_view, std::string_view>>& edges,
    std::vector<std::string_view>& in,
    absl::flat_hash_set<std::string_view> out) {

  std::vector<std::string_view> ret = in;
  for (/*nop*/; !nodes.empty(); nodes = nodes.subspan(1)) {
    bool all_ok = true;
    for (std::string_view test : in) {
      if (!edges.contains({nodes[0], test})) {
        all_ok = false;
      }
    }
    if (all_ok) {
      in.push_back(nodes[0]);
      std::vector<std::string_view> sub = FindBiggest(nodes, edges, in, out);
      if (sub.size() > ret.size()) ret = sub;
      in.pop_back();
    }
    out.insert(nodes[0]);
  }

  return ret;
}

std::vector<std::string_view> FindBiggest(
    absl::Span<const std::string_view> nodes,
    const absl::flat_hash_set<std::pair<std::string_view, std::string_view>>& edges) {
  std::vector<std::string_view> in;
  absl::flat_hash_set<std::string_view> out;
  return FindBiggest(nodes, edges, in, out);
}

}  // namespace

absl::StatusOr<std::string> Day_2024_23::Part1(
    absl::Span<std::string_view> input) const {
  Graph<bool> g;
  absl::flat_hash_set<std::pair<std::string_view, std::string_view>> edges;
  for (std::string_view line : input) {
    if (line.size() != 5) return absl::InvalidArgumentError("line length");
    if (line[2] != '-') return absl::InvalidArgumentError("bad net");
    g.AddEdge(line.substr(0, 2), line.substr(3, 2));
    edges.insert({line.substr(0, 2), line.substr(3, 2)});
    edges.insert({line.substr(3, 2), line.substr(0, 2)});
  }
  int triplets = 0;
  for (std::string_view node : g.nodes()) {
    if (node[0] != 't') continue;
    const std::vector<std::string_view>* connected = g.Outgoing(node);
    for (int i = 0; i < connected->size(); ++i) {
      std::string_view n1 = (*connected)[i];
      if (n1[0] == 't' && n1 < node) continue;
      for (int j = i + 1; j < connected->size(); ++j) {
        std::string_view n2 = (*connected)[j];
        if (n2[0] == 't' && n2 < node) continue;
        if (edges.contains({n1, n2})) {
          ++triplets;
        }
      }
    }
  }
  return AdventReturn(triplets);
}

absl::StatusOr<std::string> Day_2024_23::Part2(
    absl::Span<std::string_view> input) const {
  Graph<bool> g;
  absl::flat_hash_set<std::pair<std::string_view, std::string_view>> edges;
  for (std::string_view line : input) {
    if (line.size() != 5) return absl::InvalidArgumentError("line length");
    if (line[2] != '-') return absl::InvalidArgumentError("bad net");
    g.AddEdge(line.substr(0, 2), line.substr(3, 2));
    edges.insert({line.substr(0, 2), line.substr(3, 2)});
    edges.insert({line.substr(3, 2), line.substr(0, 2)});
  }

  std::vector<std::string_view> nodes(g.nodes().begin(), g.nodes().end());
  std::vector<std::string_view> party = FindBiggest(nodes, edges);
  absl::c_sort(party);
  return AdventReturn(absl::StrJoin(party, ","));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/23,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_23()); });

}  // namespace advent_of_code
