#ifndef ADVENT_OF_CODE_DAG_H
#define ADVENT_OF_CODE_DAG_H

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

namespace advent_of_code {

template <typename Storage>
class DAG {
 public:
  DAG() = default;

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

  absl::StatusOr<std::vector<absl::string_view>> DAGSort() const;

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
absl::StatusOr<std::vector<absl::string_view>> DAG<Storage>::DAGSort() const {
  absl::flat_hash_map<absl::string_view, int> name_to_dep_count;
  for (const auto& node : nodes_) {
    auto it = node_to_incoming_.find(node);
    if (it == node_to_incoming_.end()) {
      name_to_dep_count[node] = 0;
    } else {
      name_to_dep_count[node] = it->second.size();
    }
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
      const std::vector<absl::string_view>* outgoing = Outgoing(node);
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

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_DAG_H