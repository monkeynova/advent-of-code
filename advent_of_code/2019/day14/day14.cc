#include "advent_of_code/2019/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/interval.h"

namespace advent_of_code {
namespace {

struct Rule {
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
  absl::flat_hash_map<std::string_view, int64_t> needs = {
      {"FUEL", fuel_needed}};
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
  ASSIGN_OR_RETURN(std::vector<std::string_view> ordered_ingredients,
                   rule_set.DAGSort());
  if (ordered_ingredients[0] != "FUEL") {
    return Error("Not a DAG rooted at FUEL");
  }
  if (ordered_ingredients.back() != "ORE") {
    return Error("Not a DAG with leaf at ORE");
  }
  return ComputeOreNeedForFuel(rule_set, ordered_ingredients, 1);
}

absl::StatusOr<int> FuelFromOre(const DirectedGraph<Rule>& rule_set,
                                int64_t ore_supply) {
  ASSIGN_OR_RETURN(std::vector<std::string_view> ordered_ingredients,
                   rule_set.DAGSort());
  if (ordered_ingredients[0] != "FUEL") {
    return Error("Not a DAG rooted at FUEL");
  }
  if (ordered_ingredients.back() != "ORE") {
    return Error("Not a DAG with leaf at ORE");
  }
  int smallest_unmakable = InfiniteBinarySearch([&](int guess) {
    absl::StatusOr<int64_t> ore_needed =
        ComputeOreNeedForFuel(rule_set, ordered_ingredients, guess);
    CHECK(ore_needed.ok());
    VLOG(1) << guess << " => " << *ore_needed;
    return *ore_needed < ore_supply;
  });
  return smallest_unmakable - 1;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_14::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<Rule> rule_set, ParseRuleSet(input));

  return AdventReturn(ComputeOreNeedForFuel(rule_set));
}

absl::StatusOr<std::string> Day_2019_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<Rule> rule_set, ParseRuleSet(input));

  return AdventReturn(FuelFromOre(rule_set, 1'000'000'000'000));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/14,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_14()); });

}  // namespace advent_of_code
