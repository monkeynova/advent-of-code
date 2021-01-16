#ifndef ADVENT_OF_CODE_DAG_SORT_H
#define ADVENT_OF_CODE_DAG_SORT_H

#include <vector>

#include "absl/status/statusor.h"
#include "advent_of_code/directed_graph.h"

namespace advent_of_code {

template <typename Storage>
absl::StatusOr<std::vector<absl::string_view>> DAGSort(
    const DirectedGraph<Storage>& graph) {
  absl::flat_hash_map<absl::string_view, int> name_to_dep_count;
  for (const auto& node : graph.nodes()) {
    const std::vector<absl::string_view>* incoming = graph.Incoming(node);
    name_to_dep_count[node] = incoming == nullptr ? 0 : incoming->size();
  }
  std::vector<absl::string_view> out;
  while (!name_to_dep_count.empty()) {
    VLOG(2) << absl::StrJoin(
        name_to_dep_count, ",",
        [](std::string* out,
           const std::pair<absl::string_view, int>& name_count) {
          absl::StrAppend(out, name_count.first, ":", name_count.second);
        });
    std::vector<absl::string_view> to_remove;
    for (const auto& [name, deps] : name_to_dep_count) {
      if (deps < 0) return absl::InvalidArgumentError("Bad Deps");
      if (deps == 0) {
        to_remove.push_back(name);
      }
    }
    if (to_remove.empty()) return absl::InvalidArgumentError("Not a DAG");
    for (absl::string_view node : to_remove) {
      out.push_back(node);
      VLOG(2) << "Removing: " << node;
      name_to_dep_count.erase(node);
      const std::vector<absl::string_view>* outgoing = graph.Outgoing(node);
      if (outgoing != nullptr) {
        for (absl::string_view sub_node : *outgoing) {
          VLOG(2) << "  Decrementing: " << sub_node;
          --name_to_dep_count[sub_node];
        }
      }
    }
  }
  return out;
}

template <typename Storage, typename Comparator>
absl::StatusOr<std::vector<absl::string_view>> DAGSort(
    const DirectedGraph<Storage>& graph, Comparator cmp) {
  absl::flat_hash_map<absl::string_view, int> name_to_dep_count;
  for (const auto& node : graph.nodes()) {
    const std::vector<absl::string_view>* incoming = graph.Incoming(node);
    name_to_dep_count[node] = incoming == nullptr ? 0 : incoming->size();
  }
  std::vector<absl::string_view> out;
  while (!name_to_dep_count.empty()) {
    VLOG(2) << absl::StrJoin(
        name_to_dep_count, ",",
        [](std::string* out,
           const std::pair<absl::string_view, int>& name_count) {
          absl::StrAppend(out, name_count.first, ":", name_count.second);
        });
    absl::string_view to_remove;
    for (const auto& [name, deps] : name_to_dep_count) {
      if (deps < 0) return absl::InvalidArgumentError("Bad Deps");
      if (deps == 0 && (to_remove.empty() || cmp(name, to_remove))) {
        to_remove = name;
      }
    }
    if (to_remove.empty()) return absl::InvalidArgumentError("Not a DAG");
    out.push_back(to_remove);
    VLOG(2) << "Removing: " << to_remove;
    name_to_dep_count.erase(to_remove);
    const std::vector<absl::string_view>* outgoing = graph.Outgoing(to_remove);
    if (outgoing != nullptr) {
      for (absl::string_view sub_node : *outgoing) {
        VLOG(2) << "  Decrementing: " << sub_node;
        --name_to_dep_count[sub_node];
      }
    }
  }
  return out;
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_DAG_SORT_H