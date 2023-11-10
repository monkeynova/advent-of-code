#include "advent_of_code/2020/day07/day07.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct BagRule {
  absl::flat_hash_map<std::string_view, int> bag_to_count;
};

absl::StatusOr<DirectedGraph<BagRule>> Parse(
    absl::Span<std::string_view> input) {
  DirectedGraph<BagRule> ret;
  for (std::string_view str : input) {
    const auto [outer, inner] = PairSplit(str, " bags contain ");
    if (ret.GetData(outer) != nullptr) {
      return Error("color dupe: ", outer);
    }
    if (inner == "no other bags.") {
      // OK for the rule to be empty.
      ret.AddNode(outer, BagRule{});
    } else {
      BagRule bag_rule;
      for (std::string_view bag_rule_str : absl::StrSplit(inner, ", ")) {
        static LazyRE2 bag_pattern{"(\\d+) (.*) bags?\\.?"};
        int count;
        std::string_view color;
        if (!RE2::FullMatch(bag_rule_str, *bag_pattern, &count, &color)) {
          return Error("bag rule: ", bag_rule_str);
        }
        bag_rule.bag_to_count.emplace(color, count);
        ret.AddEdge(outer, color);
      }
      ret.AddNode(outer, std::move(bag_rule));
    }
  }
  return ret;
}

int CountContainingBags(const DirectedGraph<BagRule>& bags,
                        std::string_view bag) {
  absl::flat_hash_set<std::string_view> can_contain;
  absl::flat_hash_set<std::string_view> added = {bag};
  while (!added.empty()) {
    absl::flat_hash_set<std::string_view> new_added;
    for (const std::string_view& color : added) {
      const std::vector<std::string_view>* incoming = bags.Incoming(color);
      if (incoming != nullptr) {
        for (std::string_view parent_color : *incoming) {
          if (can_contain.contains(parent_color)) continue;
          new_added.insert(parent_color);
          can_contain.insert(parent_color);
        }
      }
    }
    added = std::move(new_added);
  }
  return can_contain.size();
}

absl::StatusOr<int> CountContainedBags(const DirectedGraph<BagRule>& bags,
                                       std::string_view bag) {
  const BagRule* bag_rule = bags.GetData(bag);
  if (bag_rule == nullptr) {
    return Error("Cannot find bag: ", bag);
  }
  const std::vector<std::string_view>* outgoing = bags.Outgoing(bag);
  int bag_count = 1;
  if (outgoing != nullptr) {
    for (std::string_view sub_bag : *outgoing) {
      absl::StatusOr<int> sub_bags = CountContainedBags(bags, sub_bag);
      if (!sub_bags.ok()) return sub_bags.status();
      auto sub_bag_count_it = bag_rule->bag_to_count.find(sub_bag);
      if (sub_bag_count_it == bag_rule->bag_to_count.end()) {
        return Error("Cannot find bag count: ", sub_bag);
      }
      bag_count += sub_bag_count_it->second * *sub_bags;
    }
  }
  return bag_count;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_07::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<DirectedGraph<BagRule>> bags = Parse(input);
  if (!bags.ok()) return bags.status();
  VLOG(1) << bags->nodes().size();
  return AdventReturn(CountContainingBags(*bags, "shiny gold"));
}

absl::StatusOr<std::string> Day_2020_07::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<DirectedGraph<BagRule>> bags = Parse(input);
  if (!bags.ok()) return bags.status();

  absl::StatusOr<int> contained_bags = CountContainedBags(*bags, "shiny gold");
  if (!contained_bags.ok()) return bags.status();
  return AdventReturn(*contained_bags - 1); /* don't include top bag */
}

}  // namespace advent_of_code
