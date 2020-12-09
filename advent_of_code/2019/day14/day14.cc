#include "advent_of_code/2019/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

struct Ingredient {
  std::string name;
  int quantity;
};

struct Rule {
  std::string name;
  int quantity_out;
  std::vector<Ingredient> in;
};

using RuleSet = std::vector<Rule>;

absl::StatusOr<RuleSet> DagSort(RuleSet rule_set) {
  absl::flat_hash_map<std::string, const Rule*> name_to_rule;
  absl::flat_hash_map<std::string, int> name_to_dep_count;
  for (const Rule& rule : rule_set) {
    name_to_rule[rule.name] = &rule;
    if (!name_to_dep_count.contains(rule.name)) {
      name_to_dep_count.emplace(rule.name, 0);
    }
    for (const Ingredient& in : rule.in) {
      ++name_to_dep_count[in.name];
    }
  }

  RuleSet out;
  while (!name_to_rule.empty()) {
    VLOG(2) << absl::StrJoin(
        name_to_dep_count, ",",
        [](std::string* out, const std::pair<std::string, int>& name_count) {
          absl::StrAppend(out, name_count.first, ":", name_count.second);
        });
    std::vector<const Rule*> to_remove;
    for (const auto& name_rule : name_to_rule) {
      int deps = name_to_dep_count[name_rule.first];
      if (deps < 0) return absl::InvalidArgumentError("Bad Deps");
      if (deps == 0) {
        to_remove.push_back(name_rule.second);
      }
    }
    if (to_remove.empty()) return absl::InvalidArgumentError("Not a DAG");
    for (const Rule* rule : to_remove) {
      out.push_back(*rule);
      VLOG(2) << "Removing: " << rule->name;
      name_to_rule.erase(rule->name);
      name_to_dep_count.erase(rule->name);
      for (const Ingredient& in : rule->in) {
        VLOG(2) << "  Decrementing: " << in.name;
        --name_to_dep_count[in.name];
      }
    }
  }
  return out;
}

absl::StatusOr<RuleSet> ParseRuleSet(
    const std::vector<absl::string_view>& input) {
  RuleSet rule_set;
  absl::flat_hash_set<absl::string_view> rule_history;
  for (absl::string_view rule_str : input) {
    std::vector<absl::string_view> in_out = absl::StrSplit(rule_str, " => ");
    if (in_out.size() != 2) return absl::InvalidArgumentError("Bad rule");
    std::vector<absl::string_view> inputs = absl::StrSplit(in_out[0], ", ");

    rule_set.push_back(Rule{});
    Rule& rule = rule_set.back();
    std::vector<absl::string_view> quantity_name =
        absl::StrSplit(in_out[1], " ");
    if (rule_history.contains(quantity_name[1]))
      return absl::InvalidArgumentError("Duplicate rule");
    rule_history.insert(quantity_name[1]);
    rule.name = quantity_name[1];
    if (quantity_name.size() != 2) {
      return absl::InvalidArgumentError("Bad ingredient");
    }
    if (!absl::SimpleAtoi(quantity_name[0], &rule.quantity_out)) {
      return absl::InvalidArgumentError("Bad quantity");
    }

    for (absl::string_view in : inputs) {
      std::vector<absl::string_view> quantity_name = absl::StrSplit(in, " ");
      if (quantity_name.size() != 2) {
        return absl::InvalidArgumentError("Bad ingredient");
      }
      Ingredient i;
      i.name = quantity_name[1];
      if (!absl::SimpleAtoi(quantity_name[0], &i.quantity)) {
        return absl::InvalidArgumentError("Bad quantity");
      }
      rule.in.push_back(i);
    }
  }
  return DagSort(std::move(rule_set));
}

absl::StatusOr<int64_t> ComputeOreNeedForFuel(const RuleSet& rule_set,
                                              int64_t fuel_needed = 1) {
  absl::flat_hash_map<absl::string_view, int64_t> needs;
  needs.emplace("FUEL", fuel_needed);
  for (const Rule& rule : rule_set) {
    VLOG(1) << absl::StrJoin(
        needs, ", ",
        [](std::string* out, const std::pair<absl::string_view, int>& need) {
          absl::StrAppend(out, need.first, ":", need.second);
        });
    int64_t needed = needs[rule.name];
    if (needed <= 0) continue;
    needs.erase(rule.name);

    int64_t mult = (needed + rule.quantity_out - 1) / rule.quantity_out;
    for (const Ingredient& in : rule.in) {
      needs[in.name] += mult * in.quantity;
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

absl::StatusOr<int> FuelFromOre(const RuleSet& rule_set, uint64_t ore_supply) {
  int64_t guess = 1;
  absl::StatusOr<int64_t> ore_needed = 0;
  while (*ore_needed < ore_supply) {
    ore_needed = ComputeOreNeedForFuel(rule_set, guess);
    if (!ore_needed.ok()) return ore_needed.status();
    VLOG(1) << guess << " => " << *ore_needed;
    guess <<= 1;
  }
  int64_t min = guess >> 2;
  int64_t max = guess >> 1;
  while (min < max) {
    guess = (min + max) / 2;
    ore_needed = ComputeOreNeedForFuel(rule_set, guess);
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
  ore_needed = ComputeOreNeedForFuel(rule_set, guess);
  if (*ore_needed > ore_supply) return guess - 1;
  return guess;
}

absl::StatusOr<std::vector<std::string>> Day14_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<RuleSet> rule_set = ParseRuleSet(input);
  if (!rule_set.ok()) return rule_set.status();

  if ((*rule_set)[0].name != "FUEL") {
    return absl::InvalidArgumentError("Rules aren't a DAG rooted with FUEL");
  }

  return IntReturn(ComputeOreNeedForFuel(*rule_set));
}

absl::StatusOr<std::vector<std::string>> Day14_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<RuleSet> rule_set = ParseRuleSet(input);
  if (!rule_set.ok()) return rule_set.status();

  if ((*rule_set)[0].name != "FUEL") {
    return absl::InvalidArgumentError("Rules aren't a DAG rooted with FUEL");
  }

  return IntReturn(FuelFromOre(*rule_set, 1000000000000));
}
