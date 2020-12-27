#include "advent_of_code/2017/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::optional<int> FindBadWeight(
    absl::string_view root,
    const absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>& tree, 
    const absl::flat_hash_map<absl::string_view, int>& weights,
    int* this_weight = nullptr) {
  auto weight_it = weights.find(root);
  CHECK(weight_it != weights.end());

  auto it = tree.find(root);
  if (it == tree.end()) {
    if (this_weight != nullptr) {
      *this_weight = weight_it->second;
    }
    return absl::nullopt;
  }
  const std::vector<absl::string_view>& children = it->second;
  absl::flat_hash_map<int, int> sub_weight_counts;
  absl::flat_hash_map<int, int> sub_weight_example;
  for (int i = 0; i < children.size(); ++i) {
    int this_weight;
    absl::optional<int> bad = FindBadWeight(children[i], tree, weights, &this_weight);
    VLOG(1) << children[i] << ": " << this_weight;
    if (bad) return bad;
    ++sub_weight_counts[this_weight];
    auto child_weight_it = weights.find(children[i]);
    CHECK(child_weight_it != weights.end());
    sub_weight_example[this_weight] = child_weight_it->second;
  }
  CHECK(sub_weight_counts.size() < 3);
  CHECK(sub_weight_counts.size() > 0);
  if (sub_weight_counts.size() == 2) {
    int miss_target_weight;
    int miss_weight;
    int target_weight;
    for (const auto& [weight, count] : sub_weight_counts) {
      if (count == 1) {
        miss_target_weight = weight;
        miss_weight = sub_weight_example[weight];
      } else {
        target_weight = weight;
      }
    }
    return target_weight - miss_target_weight + miss_weight;
  }
  if (this_weight != nullptr) {
    *this_weight = weight_it->second + children.size() * sub_weight_counts.begin()->first;
  }
  return absl::nullopt;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day07_2017::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<absl::string_view> parents;
  absl::flat_hash_set<absl::string_view> children;
  for (absl::string_view str : input) {
    std::vector<absl::string_view> p_and_c_list = absl::StrSplit(str, " -> ");
    if (p_and_c_list.size() > 2) return Error("Bad input: ", str);
    if (p_and_c_list.size() == 1) continue;
    for (absl::string_view child : absl::StrSplit(p_and_c_list[1], ", ")) {
      children.insert(child);
    }
    absl::string_view parent;
    if (!RE2::FullMatch(p_and_c_list[0], "(.*) \\(\\d+\\)", &parent)) {
      return Error("Bad parent: ", p_and_c_list[0]);
    }
    parents.insert(parent);
  }
  absl::string_view root;
  for (absl::string_view p : parents) {
    if (!children.contains(p)) {
      if (root != "") return Error("Dupe roots");
      root = p;
    }
  }
  return std::vector<std::string>{std::string(root)};
}

absl::StatusOr<std::vector<std::string>> Day07_2017::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>> tree;
  absl::flat_hash_map<absl::string_view, int> weights;
  absl::flat_hash_set<absl::string_view> children;
  for (absl::string_view str : input) {
    std::vector<absl::string_view> p_and_c_list = absl::StrSplit(str, " -> ");
    if (p_and_c_list.size() > 2) return Error("Bad input: ", str);
    absl::string_view parent;
    int weight;
    if (!RE2::FullMatch(p_and_c_list[0], "(.*) \\((\\d+)\\)", &parent, &weight)) {
      return Error("Bad parent: ", p_and_c_list[0]);
    }
    weights[parent] = weight;
    if (p_and_c_list.size() == 1) continue;
    std::vector<absl::string_view>& child_vec = tree[parent];
    for (absl::string_view child : absl::StrSplit(p_and_c_list[1], ", ")) {
      child_vec.push_back(child);
      children.insert(child);
    }
  }
  absl::string_view root;
  for (const auto& [p, _] : tree) {
    if (!children.contains(p)) {
      if (root != "") return Error("Dupe roots");
      root = p;
    }
  }
  return IntReturn(FindBadWeight(root, tree, weights));
}

}  // namespace advent_of_code
