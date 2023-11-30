#include "advent_of_code/2017/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::optional<int> FindBadWeight(std::string_view root,
                                 const DirectedGraph<int>& dag,
                                 int* this_weight_out = nullptr) {
  const int* weight = dag.GetData(root);
  CHECK(weight != nullptr);

  const std::vector<std::string_view>* children = dag.Outgoing(root);
  if (children == nullptr) {
    if (this_weight_out != nullptr) {
      *this_weight_out = *weight;
    }
    return absl::nullopt;
  }
  absl::flat_hash_map<int, int> sub_weight_counts;
  absl::flat_hash_map<int, int> sub_weight_example;
  for (int i = 0; i < children->size(); ++i) {
    int this_weight;
    std::optional<int> bad = FindBadWeight((*children)[i], dag, &this_weight);
    VLOG(1) << (*children)[i] << ": " << this_weight;
    if (bad) return bad;
    ++sub_weight_counts[this_weight];
    const int* child_weight = dag.GetData((*children)[i]);
    CHECK(child_weight != nullptr);
    sub_weight_example[this_weight] = *child_weight;
  }
  CHECK(sub_weight_counts.size() < 3);
  CHECK(sub_weight_counts.size() > 0);
  if (sub_weight_counts.size() == 2) {
    int miss_target_weight = -1;
    int miss_weight;
    int target_weight = -1;
    for (const auto& [weight, count] : sub_weight_counts) {
      if (count == 1) {
        miss_target_weight = weight;
        miss_weight = sub_weight_example[weight];
      } else {
        target_weight = weight;
      }
    }
    CHECK_NE(target_weight, -1);
    CHECK_NE(miss_target_weight, -1);
    return target_weight - miss_target_weight + miss_weight;
  }
  if (this_weight_out != nullptr) {
    *this_weight_out =
        *weight + children->size() * sub_weight_counts.begin()->first;
  }
  return absl::nullopt;
}

absl::StatusOr<DirectedGraph<int>> Parse(absl::Span<std::string_view> input) {
  DirectedGraph<int> dag;
  for (std::string_view str : input) {
    std::vector<std::string_view> p_and_c_list = absl::StrSplit(str, " -> ");
    if (p_and_c_list.size() > 2) return Error("Bad input: ", str);
    std::string_view parent;
    int weight;
    if (!RE2::FullMatch(p_and_c_list[0], "(.*) \\((\\d+)\\)", &parent,
                        &weight)) {
      return Error("Bad parent: ", p_and_c_list[0]);
    }
    dag.AddNode(parent, weight);

    if (p_and_c_list.size() == 1) continue;
    for (std::string_view child : absl::StrSplit(p_and_c_list[1], ", ")) {
      dag.AddEdge(parent, child);
    }
  }
  return dag;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_07::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<int> dag, Parse(input));
  std::string_view root;
  for (std::string_view node : dag.nodes()) {
    const std::vector<std::string_view>* incoming = dag.Incoming(node);
    if (incoming == nullptr) {
      if (root != "") return Error("Dupe roots");
      root = node;
    }
  }
  return std::string(root);
}

absl::StatusOr<std::string> Day_2017_07::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<int> dag, Parse(input));
  std::string_view root;
  for (std::string_view node : dag.nodes()) {
    const std::vector<std::string_view>* incoming = dag.Incoming(node);
    if (incoming == nullptr) {
      if (root != "") return Error("Dupe roots");
      root = node;
    }
  }
  return AdventReturn(FindBadWeight(root, dag));
}

}  // namespace advent_of_code
