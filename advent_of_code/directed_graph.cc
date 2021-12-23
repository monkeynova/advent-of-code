#include "advent_of_code/directed_graph.h"

namespace advent_of_code {

absl::flat_hash_set<absl::string_view> DirectedGraphBase::Reachable(
    absl::string_view src) const {
  if (!nodes_.contains(src)) return {};
  absl::flat_hash_set<absl::string_view> ret = {src};
  for (std::deque<absl::string_view> queue = {src}; !queue.empty(); queue.pop_front()) {
    absl::string_view cur = queue.front();
    const std::vector<absl::string_view>* outgoing = Outgoing(cur);
    if (!outgoing) continue;
    for (absl::string_view next : *outgoing) {
      if (ret.contains(next)) continue;
      ret.insert(next);
      queue.push_back(next);
    }
  }
  return ret;
}

std::vector<std::vector<absl::string_view>> DirectedGraphBase::Forest() const {
  std::vector<std::vector<absl::string_view>> forest;

  absl::flat_hash_set<absl::string_view> to_assign = nodes();
  while (!to_assign.empty()) {
    forest.push_back({});
    std::deque<absl::string_view> frontier = {*to_assign.begin()};
    while (!frontier.empty()) {
      absl::string_view name = frontier.front();
      forest.back().push_back(name);
      frontier.pop_front();
      to_assign.erase(name);
      const std::vector<absl::string_view>* outgoing = Outgoing(name);
      if (outgoing != nullptr) {
        for (absl::string_view tmp : *outgoing) {
          if (to_assign.contains(tmp)) frontier.push_back(tmp);
        }
      }
    }
  }

  return forest;
}

absl::StatusOr<std::vector<absl::string_view>>
DirectedGraphBase::DAGSort() const {
  absl::flat_hash_map<absl::string_view, int> name_to_dep_count;
  for (const auto& node : nodes()) {
    const std::vector<absl::string_view>* incoming = Incoming(node);
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

absl::StatusOr<std::vector<absl::string_view>> DirectedGraphBase::DAGSort(
    absl::FunctionRef<bool(absl::string_view, absl::string_view)> cmp) const {
  absl::flat_hash_map<absl::string_view, int> name_to_dep_count;
  for (const auto& node : nodes()) {
    const std::vector<absl::string_view>* incoming = Incoming(node);
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
    const std::vector<absl::string_view>* outgoing = Outgoing(to_remove);
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