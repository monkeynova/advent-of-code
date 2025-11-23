// http://adventofcode.com/2024/day/23

#include "advent_of_code/2024/day23/day23.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/directed_graph.h"

namespace advent_of_code {

namespace {

using NodeType = uint16_t;

std::vector<NodeType> FindBiggest(absl::Span<const NodeType> nodes,
                                  const std::vector<bool>& edges,
                                  std::vector<NodeType>& in) {
  std::vector<bool> out(26 * 26, false);

  std::vector<NodeType> ret = in;
  for (/*nop*/; !nodes.empty(); nodes = nodes.subspan(1)) {
    if (out[nodes[0]]) continue;
    bool all_ok = true;
    for (NodeType test : in) {
      if (!edges[nodes[0] * 26 * 26 + test]) {
        all_ok = false;
      }
    }
    if (all_ok) {
      in.push_back(nodes[0]);
      std::vector<NodeType> sub = FindBiggest(nodes, edges, in);
      in.pop_back();
      if (sub.size() > ret.size()) {
        out.assign(26 * 26, false);
        for (NodeType n : sub) {
          out[n] = true;
        }
        ret = std::move(sub);
      }
    }
  }

  return ret;
}

std::vector<NodeType> FindBiggest(absl::Span<const NodeType> nodes,
                                  const std::vector<bool>& edges) {
  std::vector<NodeType> in;
  return FindBiggest(nodes, edges, in);
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
  absl::flat_hash_set<NodeType> node_set;
  std::vector<bool> edges(26 * 26 * 26 * 26, false);
  for (std::string_view line : input) {
    if (line.size() != 5) return absl::InvalidArgumentError("line length");
    if (line[2] != '-') return absl::InvalidArgumentError("bad net");
    NodeType n1 = (line[0] - 'a') * 26 + (line[1] - 'a');
    NodeType n2 = (line[3] - 'a') * 26 + (line[4] - 'a');
    node_set.insert(n1);
    node_set.insert(n2);
    edges[n1 * 26 * 26 + n2] = true;
    edges[n2 * 26 * 26 + n1] = true;
  }

  std::vector<NodeType> nodes(node_set.begin(), node_set.end());
  std::vector<NodeType> party = FindBiggest(nodes, edges);
  absl::c_sort(party);
  return AdventReturn(
      absl::StrJoin(party, ",", [](std::string* out, uint16_t node) {
        out->append(1, (node / 26) + 'a');
        out->append(1, (node % 26) + 'a');
      }));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/23,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_23()); });

}  // namespace advent_of_code
