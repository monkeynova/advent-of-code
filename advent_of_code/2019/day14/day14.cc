// https://adventofcode.com/2019/day/14
//
// --- Day 14: Space Stoichiometry ---
// -----------------------------------
// 
// As you approach the rings of Saturn, your ship's low fuel indicator
// turns on. There isn't any fuel here, but the rings have plenty of raw
// material. Perhaps your ship's Inter-Stellar Refinery Union brand
// nanofactory can turn these raw materials into fuel.
// 
// You ask the nanofactory to produce a list of the reactions it can
// perform that are relevant to this process (your puzzle input). Every
// reaction turns some quantities of specific input chemicals into some
// quantity of an output chemical. Almost every chemical is produced by
// exactly one reaction; the only exception, ORE, is the raw material
// input to the entire process and is not produced by a reaction.
// 
// You just need to know how much ORE you'll need to collect before you
// can produce one unit of FUEL.
// 
// Each reaction gives specific quantities for its inputs and output;
// reactions cannot be partially run, so only whole integer multiples of
// these quantities can be used. (It's okay to have leftover chemicals
// when you're done, though.) For example, the reaction 1 A, 2 B, 3 C =>
// 2 D means that exactly 2 units of chemical D can be produced by
// consuming exactly 1 A, 2 B and 3 C. You can run the full reaction as
// many times as necessary; for example, you could produce 10 D by
// consuming 5 A, 10 B, and 15 C.
// 
// Suppose your nanofactory produces the following list of reactions:
// 
// 10 ORE => 10 A
// 1 ORE => 1 B
// 7 A, 1 B => 1 C
// 7 A, 1 C => 1 D
// 7 A, 1 D => 1 E
// 7 A, 1 E => 1 FUEL
// 
// The first two reactions use only ORE as inputs; they indicate that you
// can produce as much of chemical A as you want (in increments of 10
// units, each 10 costing 10 ORE) and as much of chemical B as you want
// (each costing 1 ORE). To produce 1 FUEL, a total of 31 ORE is
// required: 1 ORE to produce 1 B, then 30 more ORE to produce the 7 + 7
// + 7 + 7 = 28 A (with 2 extra A wasted) required in the reactions to
// convert the B into C, C into D, D into E, and finally E into FUEL. (30
// A is produced because its reaction requires that it is created in
// increments of 10.)
// 
// Or, suppose you have the following list of reactions:
// 
// 9 ORE => 2 A
// 8 ORE => 3 B
// 7 ORE => 5 C
// 3 A, 4 B => 1 AB
// 5 B, 7 C => 1 BC
// 4 C, 1 A => 1 CA
// 2 AB, 3 BC, 4 CA => 1 FUEL
// 
// The above list of reactions requires 165 ORE to produce 1 FUEL:
// 
// * Consume 45 ORE to produce 10 A.
// 
// * Consume 64 ORE to produce 24 B.
// 
// * Consume 56 ORE to produce 40 C.
// 
// * Consume 6 A, 8 B to produce 2 AB.
// 
// * Consume 15 B, 21 C to produce 3 BC.
// 
// * Consume 16 C, 4 A to produce 4 CA.
// 
// * Consume 2 AB, 3 BC, 4 CA to produce 1 FUEL.
// 
// Here are some larger examples:
// 
// * 
// 
// 13312 ORE for 1 FUEL:
// 
// 157 ORE => 5 NZVS
// 165 ORE => 6 DCFZ
// 44 XJWVT, 5 KHKGT, 1 QDVJ, 29 NZVS, 9 GPVTF, 48 HKGWZ => 1 FUEL
// 12 HKGWZ, 1 GPVTF, 8 PSHF => 9 QDVJ
// 179 ORE => 7 PSHF
// 177 ORE => 5 HKGWZ
// 7 DCFZ, 7 PSHF => 2 XJWVT
// 165 ORE => 2 GPVTF
// 3 DCFZ, 7 NZVS, 5 HKGWZ, 10 PSHF => 8 KHKGT     
// 
// * 
// 
// 180697 ORE for 1 FUEL:
// 
// 2 VPVL, 7 FWMGM, 2 CXFTF, 11 MNCFX => 1 STKFG
// 17 NVRVD, 3 JNWZP => 8 VPVL
// 53 STKFG, 6 MNCFX, 46 VJHF, 81 HVMC, 68 CXFTF, 25 GNMV => 1 FUEL
// 22 VJHF, 37 MNCFX => 5 FWMGM
// 139 ORE => 4 NVRVD
// 144 ORE => 7 JNWZP
// 5 MNCFX, 7 RFSQX, 2 FWMGM, 2 VPVL, 19 CXFTF => 3 HVMC
// 5 VJHF, 7 MNCFX, 9 VPVL, 37 CXFTF => 6 GNMV
// 145 ORE => 6 MNCFX
// 1 NVRVD => 8 CXFTF
// 1 VJHF, 6 MNCFX => 4 RFSQX
// 176 ORE => 6 VJHF     
// 
// * 
// 
// 2210736 ORE for 1 FUEL:
// 
// 171 ORE => 8 CNZTR
// 7 ZLQW, 3 BMBT, 9 XCVML, 26 XMNCP, 1 WPTQ, 2 MZWV, 1 RJRHP => 4 PLWSL
// 114 ORE => 4 BHXH
// 14 VRPVC => 6 BMBT
// 6 BHXH, 18 KTJDG, 12 WPTQ, 7 PLWSL, 31 FHTLT, 37 ZDVW => 1 FUEL
// 6 WPTQ, 2 BMBT, 8 ZLQW, 18 KTJDG, 1 XMNCP, 6 MZWV, 1 RJRHP => 6 FHTLT
// 15 XDBXC, 2 LTCX, 1 VRPVC => 6 ZLQW
// 13 WPTQ, 10 LTCX, 3 RJRHP, 14 XMNCP, 2 MZWV, 1 ZLQW => 1 ZDVW
// 5 BMBT => 4 WPTQ
// 189 ORE => 9 KTJDG
// 1 MZWV, 17 XDBXC, 3 XCVML => 2 XMNCP
// 12 VRPVC, 27 CNZTR => 2 XDBXC
// 15 KTJDG, 12 BHXH => 5 XCVML
// 3 BHXH, 2 VRPVC => 7 MZWV
// 121 ORE => 7 VRPVC
// 7 XCVML => 6 RJRHP
// 5 BHXH, 4 VRPVC => 5 LTCX     
// 
// Given the list of reactions in your puzzle input, what is the minimum
// amount of ORE required to produce exactly 1 FUEL?
//
// --- Part Two ---
// ----------------
// 
// After collecting ORE for a while, you check your cargo hold: 1
// trillion (1000000000000) units of ORE.
// 
// With that much ore, given the examples above:
// 
// * The 13312 ORE-per-FUEL example could produce 82892753 FUEL.
// 
// * The 180697 ORE-per-FUEL example could produce 5586022 FUEL.
// 
// * The 2210736 ORE-per-FUEL example could produce 460664 FUEL.
// 
// Given 1 trillion ORE, what is the maximum amount of FUEL you can
// produce?


#include "advent_of_code/2019/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "absl/log/log.h"

namespace advent_of_code {
namespace {

struct Rule {
  std::string name;
  int quantity_out;
  absl::flat_hash_map<absl::string_view, int> in;
};

absl::StatusOr<DirectedGraph<Rule>> ParseRuleSet(
    absl::Span<absl::string_view> input) {
  DirectedGraph<Rule> rule_set;
  for (absl::string_view rule_str : input) {
    const auto [inputs, output] = PairSplit(rule_str, " => ");

    Rule rule;
    const auto [src_q, src] = PairSplit(output, " ");
    if (rule_set.GetData(src) != nullptr) {
      return absl::InvalidArgumentError("Duplicate rule");
    }
    if (!absl::SimpleAtoi(src_q, &rule.quantity_out)) {
      return absl::InvalidArgumentError("Bad quantity");
    }

    for (absl::string_view in : absl::StrSplit(inputs, ", ")) {
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
    const std::vector<absl::string_view>& ordered_ingredients,
    int64_t fuel_needed) {
  // TODO(@monkeynova): Remember this ordering...
  absl::flat_hash_map<absl::string_view, int64_t> needs;
  needs.emplace("FUEL", fuel_needed);
  for (absl::string_view node : ordered_ingredients) {
    if (node == "ORE") continue;
    const Rule* rule = rule_set.GetData(node);
    if (rule == nullptr) return Error("Cannot find ", node);
    VLOG(1) << absl::StrJoin(
        needs, ", ",
        [](std::string* out, const std::pair<absl::string_view, int>& need) {
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
  absl::StatusOr<std::vector<absl::string_view>> ordered_ingredients =
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
  absl::StatusOr<std::vector<absl::string_view>> ordered_ingredients =
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<Rule>> rule_set = ParseRuleSet(input);
  if (!rule_set.ok()) return rule_set.status();

  return IntReturn(ComputeOreNeedForFuel(*rule_set));
}

absl::StatusOr<std::string> Day_2019_14::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<Rule>> rule_set = ParseRuleSet(input);
  if (!rule_set.ok()) return rule_set.status();

  return IntReturn(FuelFromOre(*rule_set, 1000000000000));
}

}  // namespace advent_of_code
