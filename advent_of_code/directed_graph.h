#ifndef ADVENT_OF_CODE_DIRECTED_GRAPH_H
#define ADVENT_OF_CODE_DIRECTED_GRAPH_H

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

// A directed graph representation where all nodes are represented with
// absl::string_view names. Edges are stored indexed in both directions.
// The base class does not allow storing data with each node (and
// consequently is untemplated) so that graph algorithms can be implemented
// within the corresponding .cc file.
class DirectedGraphBase {
 public:
  DirectedGraphBase() = default;

  // Adds an unconnected node `node_name` to the graph.
  // `node_name` must refer to data that outlives the graph.
  void AddNode(absl::string_view node_name) { nodes_.insert(node_name); }

  // Adds a directed edge from `src` to `dst`. Ensures both nodes are present
  // in the graph.
  // `src` and `dst` must refer to data that outlives the graph.
  void AddEdge(absl::string_view src, absl::string_view dst) {
    VLOG(2) << "AddEdge: " << src << " => " << dst;
    node_to_outgoing_[src].push_back(dst);
    node_to_incoming_[dst].push_back(src);
    nodes_.insert(src);
    nodes_.insert(dst);
  }

  // Retuns all nodes in the graph.
  const absl::flat_hash_set<absl::string_view>& nodes() const { return nodes_; }

  // Returns all the nodes with an outbound edge from `node`.
  const std::vector<absl::string_view>* Outgoing(absl::string_view node) const {
    auto it = node_to_outgoing_.find(node);
    if (it == node_to_outgoing_.end()) return nullptr;
    return &it->second;
  }

  // Returns all the nodes with an inbound edge to `node`.
  const std::vector<absl::string_view>* Incoming(absl::string_view node) const {
    auto it = node_to_incoming_.find(node);
    if (it == node_to_incoming_.end()) return nullptr;
    return &it->second;
  }

  // Returns the set of all nodes transitively connected to `src`.
  absl::flat_hash_set<absl::string_view> Reachable(absl::string_view src) const;

  // Returns the sets of disjoint components of the graph.
  std::vector<std::vector<absl::string_view>> Forest() const;

  // Returns the nodes within the graph sorted such that all nodes on the
  // output of an edge are returned after all their inputs. In the event that
  // there is a cycle in the graph an error is returned. Ties are broken
  // arbitrarily.
  absl::StatusOr<std::vector<absl::string_view>> DAGSort() const;

  // The same as `DAGSort` with the exception that ties in ordering are
  // broken such that if cmp(a, b) returns true a is ordered before b in the
  // output.
  absl::StatusOr<std::vector<absl::string_view>> DAGSort(
      absl::FunctionRef<bool(absl::string_view, absl::string_view)> cmp) const;

 private:
  absl::flat_hash_set<absl::string_view> nodes_;
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>
      node_to_outgoing_;
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>
      node_to_incoming_;
};

// Represents a directed graph with templated storage to associate t each node.
template <typename Storage>
class DirectedGraph : public DirectedGraphBase {
 public:
  // Associates `s` with the `node_name`. Overwrites any previously associated
  // data.
  void AddNode(absl::string_view node_name, Storage s) {
    DirectedGraphBase::AddNode(node_name);
    storage_[node_name] = std::move(s);
  }

  // Returns the data associated with `node` or nullptr if node is present.
  const Storage* GetData(absl::string_view node) const {
    auto it = storage_.find(node);
    if (it == storage_.end()) return nullptr;
    return &it->second;
  }

  Storage* GetData(absl::string_view node) {
    auto it = storage_.find(node);
    if (it == storage_.end()) return nullptr;
    return &it->second;
  }

 private:
  absl::flat_hash_map<absl::string_view, Storage> storage_;
};

// Represents an undirected graph (as implemented by making every edige in a
// directed graph bi-directional).
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