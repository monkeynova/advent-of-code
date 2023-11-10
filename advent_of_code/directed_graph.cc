#include "advent_of_code/directed_graph.h"

#include "advent_of_code/graph_walk.h"

namespace advent_of_code {

absl::flat_hash_set<std::string_view> DirectedGraphBase::Reachable(
    std::string_view src) const {
  if (!nodes_.contains(src)) return {};
  absl::flat_hash_set<std::string_view> ret = {src};
  for (std::deque<std::string_view> queue = {src}; !queue.empty();
       queue.pop_front()) {
    std::string_view cur = queue.front();
    const std::vector<std::string_view>* outgoing = Outgoing(cur);
    if (!outgoing) continue;
    for (std::string_view next : *outgoing) {
      if (ret.contains(next)) continue;
      ret.insert(next);
      queue.push_back(next);
    }
  }
  return ret;
}

std::vector<std::vector<std::string_view>> DirectedGraphBase::Forest() const {
  std::vector<std::vector<std::string_view>> forest;

  absl::flat_hash_set<std::string_view> to_assign = nodes();
  while (!to_assign.empty()) {
    std::string_view start = *to_assign.begin();
    forest.push_back({});
    GraphWalk({
                  .graph = this,
                  .start = start,
                  .is_good = [&](std::string_view node,
                                 int) { return to_assign.contains(node); },
                  .is_final =
                      [&](std::string_view node, int) {
                        forest.back().push_back(node);
                        to_assign.erase(node);
                        return false;
                      },
              })
        .Walk();
  }

  return forest;
}

absl::StatusOr<std::vector<std::string_view>> DirectedGraphBase::DAGSort()
    const {
  absl::flat_hash_map<std::string_view, int> name_to_dep_count;
  for (const auto& node : nodes()) {
    const std::vector<std::string_view>* incoming = Incoming(node);
    name_to_dep_count[node] = incoming == nullptr ? 0 : incoming->size();
  }
  std::vector<std::string_view> out;
  while (!name_to_dep_count.empty()) {
    VLOG(2) << absl::StrJoin(
        name_to_dep_count, ",",
        [](std::string* out,
           const std::pair<std::string_view, int>& name_count) {
          absl::StrAppend(out, name_count.first, ":", name_count.second);
        });
    std::vector<std::string_view> to_remove;
    for (const auto& [name, deps] : name_to_dep_count) {
      if (deps < 0) return absl::InvalidArgumentError("Bad Deps");
      if (deps == 0) {
        to_remove.push_back(name);
      }
    }
    if (to_remove.empty()) return absl::InvalidArgumentError("Not a DAG");
    for (std::string_view node : to_remove) {
      out.push_back(node);
      VLOG(2) << "Removing: " << node;
      name_to_dep_count.erase(node);
      const std::vector<std::string_view>* outgoing = Outgoing(node);
      if (outgoing != nullptr) {
        for (std::string_view sub_node : *outgoing) {
          VLOG(2) << "  Decrementing: " << sub_node;
          --name_to_dep_count[sub_node];
        }
      }
    }
  }
  return out;
}

absl::StatusOr<std::vector<std::string_view>> DirectedGraphBase::DAGSort(
    absl::FunctionRef<bool(std::string_view, std::string_view)> cmp) const {
  absl::flat_hash_map<std::string_view, int> name_to_dep_count;
  for (const auto& node : nodes()) {
    const std::vector<std::string_view>* incoming = Incoming(node);
    name_to_dep_count[node] = incoming == nullptr ? 0 : incoming->size();
  }
  std::vector<std::string_view> out;
  while (!name_to_dep_count.empty()) {
    VLOG(2) << absl::StrJoin(
        name_to_dep_count, ",",
        [](std::string* out,
           const std::pair<std::string_view, int>& name_count) {
          absl::StrAppend(out, name_count.first, ":", name_count.second);
        });
    std::string_view to_remove;
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
    const std::vector<std::string_view>* outgoing = Outgoing(to_remove);
    if (outgoing != nullptr) {
      for (std::string_view sub_node : *outgoing) {
        VLOG(2) << "  Decrementing: " << sub_node;
        --name_to_dep_count[sub_node];
      }
    }
  }
  return out;
}

}  // namespace advent_of_code