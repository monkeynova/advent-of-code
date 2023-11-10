#include "advent_of_code/2019/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"

namespace advent_of_code {
namespace {

struct Rule {
  std::string name;
  int quantity_out;
  absl::flat_hash_map<std::string_view, int> in;
};

absl::StatusOr<DirectedGraph<Rule>> ParseRuleSet(
    absl::Span<std::string_view> input) {
  DirectedGraph<Rule> rule_set;
  for (std::string_view rule_str : input) {
    const auto [inputs, output] = PairSplit(rule_str, " => ");

    Rule rule;
    const auto [src_q, src] = PairSplit(output, " ");
    if (rule_set.GetData(src) != nullptr) {
      return absl::InvalidArgumentError("Duplicate rule");
    }
    if (!absl::SimpleAtoi(src_q, &rule.quantity_out)) {
      return absl::InvalidArgumentError("Bad quantity");
    }

    for (std::string_view in : absl::StrSplit(inputs, ", ")) {
      const auto [dest_q, dest] = PairSplit(in, " ");
      int quantity;
      if (!absl::SimpleAtoi(dest_q, &quantity)) {
        return absl::InvalidArgumentError("Bad quantity");
      }
      rule.in.emplace(dest, quantity);
      rule_set.AddEdge(src, dest);
    }
    rule_set.AddNode(src, std::move(rule));
  }
  return rule_set;
}

absl::StatusOr<int64_t> ComputeOreNeedForFuel(
    const DirectedGraph<Rule>& rule_set,
    const std::vector<std::string_view>& ordered_ingredients,
    int64_t fuel_needed) {
  // TODO(@monkeynova): Remember this ordering...
  absl::flat_hash_map<std::string_view, int64_t> needs;
  needs.emplace("FUEL", fuel_needed);
  for (std::string_view node : ordered_ingredients) {
    if (node == "ORE") continue;
    const Rule* rule = rule_set.GetData(node);
    if (rule == nullptr) return Error("Cannot find ", node);
    VLOG(1) << absl::StrJoin(
        needs, ", ",
        [](std::string* out, const std::pair<std::string_view, int>& need) {
          absl::StrAppend(out, need.first, ":", need.second);
        });
    int64_t needed = needs[node];
    if (needed <= 0) continue;
    needs.erase(node);

    int64_t mult = (needed + rule->quantity_out - 1) / rule->quantity_out;
    for (const auto& [name, qty] : rule->in) {
      needs[name] += mult * qty;
    }
  }
  if (needs.size() != 1) {
    return absl::InvalidArgumentError("Didn't resolve to a single resources");
  }
  if (needs.begin()->first != "ORE") {
    return absl::InvalidArgumentError("Didn't resolve to ORE");
  }
  return needs.begin()->second;
}

absl::StatusOr<int64_t> ComputeOreNeedForFuel(
    const DirectedGraph<Rule>& rule_set) {
  absl::StatusOr<std::vector<std::string_view>> ordered_ingredients =
      rule_set.DAGSort();
  if (!ordered_ingredients.ok()) return ordered_ingredients.status();
  if (ordered_ingredients->at(0) != "FUEL") {
    return Error("Not a DAG rooted at FUEL");
  }
  if (ordered_ingredients->back() != "ORE") {
    return Error("Not a DAG with leaf at ORE");
  }
  return ComputeOreNeedForFuel(rule_set, *ordered_ingredients, 1);
}

absl::StatusOr<int> FuelFromOre(const DirectedGraph<Rule>& rule_set,
                                uint64_t ore_supply) {
  absl::StatusOr<std::vector<std::string_view>> ordered_ingredients =
      rule_set.DAGSort();
  if (!ordered_ingredients.ok()) return ordered_ingredients.status();
  if (ordered_ingredients->at(0) != "FUEL") {
    return Error("Not a DAG rooted at FUEL");
  }
  if (ordered_ingredients->back() != "ORE") {
    return Error("Not a DAG with leaf at ORE");
  }
  int64_t guess = 1;
  absl::StatusOr<int64_t> ore_needed = 0;
  while (*ore_needed < ore_supply) {
    ore_needed = ComputeOreNeedForFuel(rule_set, *ordered_ingredients, guess);
    if (!ore_needed.ok()) return ore_needed.status();
    VLOG(1) << guess << " => " << *ore_needed;
    guess <<= 1;
  }
  int64_t min = guess >> 2;
  int64_t max = guess >> 1;
  while (min < max) {
    guess = (min + max) / 2;
    ore_needed = ComputeOreNeedForFuel(rule_set, *ordered_ingredients, guess);
    if (!ore_needed.ok()) return ore_needed.status();
    VLOG(1) << guess << " => " << *ore_needed;
    if (*ore_needed == ore_supply) {
      min = max = guess;
    } else if (*ore_needed < ore_supply) {
      if (min == guess) break;
      min = guess;
    } else {
      max = guess;
    }
  }
  ore_needed = ComputeOreNeedForFuel(rule_set, *ordered_ingredients, guess);
  if (*ore_needed > ore_supply) return guess - 1;
  return guess;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_14::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<DirectedGraph<Rule>> rule_set = ParseRuleSet(input);
  if (!rule_set.ok()) return rule_set.status();

  return AdventReturn(ComputeOreNeedForFuel(*rule_set));
}

absl::StatusOr<std::string> Day_2019_14::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<DirectedGraph<Rule>> rule_set = ParseRuleSet(input);
  if (!rule_set.ok()) return rule_set.status();

  return AdventReturn(FuelFromOre(*rule_set, 1000000000000));
}

}  // namespace advent_of_code
