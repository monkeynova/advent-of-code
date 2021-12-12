#ifndef ADVENT_OF_CODE_DIRECTED_GRAPH_H
#define ADVENT_OF_CODE_DIRECTED_GRAPH_H

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

namespace advent_of_code {

template <typename Storage>
class DirectedGraph {
 public:
  DirectedGraph() = default;

  void AddNode(absl::string_view node_name, Storage s) {
    storage_[node_name] = std::move(s);
    nodes_.insert(node_name);
  }

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

  const Storage* GetData(absl::string_view node) const {
    auto it = storage_.find(node);
    if (it == storage_.end()) return nullptr;
    return &it->second;
  }

 private:
  absl::flat_hash_set<absl::string_view> nodes_;
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>
      node_to_outgoing_;
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>
      node_to_incoming_;
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