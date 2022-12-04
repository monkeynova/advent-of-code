#ifndef ADVENT_OF_CODE_DIRECTED_GRAPH_H
#define ADVENT_OF_CODE_DIRECTED_GRAPH_H

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"
#include "absl/log/log.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

class DirectedGraphBase {
 public:
  DirectedGraphBase() = default;

  void AddNode(absl::string_view node_name) { nodes_.insert(node_name); }

  void AddEdge(absl::string_view src, absl::string_view dst) {
    VLOG(2) << "AddEdge: " << src << " => " << dst;
    node_to_outgoing_[src].push_back(dst);
    node_to_incoming_[dst].push_back(src);
    nodes_.insert(src);
    nodes_.insert(dst);
  }

  const absl::flat_hash_set<absl::string_view>& nodes() const { return nodes_; }

  const std::vector<absl::string_view>* Outgoing(absl::string_view node) const {
    auto it = node_to_outgoing_.find(node);
    if (it == node_to_outgoing_.end()) return nullptr;
    return &it->second;
  }

  const std::vector<absl::string_view>* Incoming(absl::string_view node) const {
    auto it = node_to_incoming_.find(node);
    if (it == node_to_incoming_.end()) return nullptr;
    return &it->second;
  }

  absl::flat_hash_set<absl::string_view> Reachable(absl::string_view src) const;
  std::vector<std::vector<absl::string_view>> Forest() const;
  absl::StatusOr<std::vector<absl::string_view>> DAGSort() const;
  absl::StatusOr<std::vector<absl::string_view>> DAGSort(
      absl::FunctionRef<bool(absl::string_view, absl::string_view)> cmp) const;

 private:
  absl::flat_hash_set<absl::string_view> nodes_;
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>
      node_to_outgoing_;
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>
      node_to_incoming_;
};

template <typename Storage>
class DirectedGraph : public DirectedGraphBase {
 public:
  void AddNode(absl::string_view node_name, Storage s) {
    DirectedGraphBase::AddNode(node_name);
    storage_[node_name] = std::move(s);
  }

  const Storage* GetData(absl::string_view node) const {
    auto it = storage_.find(node);
    if (it == storage_.end()) return nullptr;
    return &it->second;
  }

 private:
  absl::flat_hash_map<absl::string_view, Storage> storage_;
};

template <typename Storage>
class Graph : public DirectedGraph<Storage> {
 public:
  void AddEdge(absl::string_view a, absl::string_view b) {
    DirectedGraph<Storage>::AddEdge(a, b);
    DirectedGraph<Storage>::AddEdge(b, a);
  }
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_DIRECTED_GRAPH_H