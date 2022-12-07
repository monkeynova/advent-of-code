// https://adventofcode.com/2020/day/7
//
// --- Day 7: Handy Haversacks ---
// -------------------------------
//
// You land at the regional airport in time for your next flight. In
// fact, it looks like you'll even have time to grab some food: all
// flights are currently delayed due to issues in luggage processing.
//
// Due to recent aviation regulations, many rules (your puzzle input) are
// being enforced about bags and their contents; bags must be color-coded
// and must contain specific quantities of other color-coded bags.
// Apparently, nobody responsible for these regulations considered how
// long they would take to enforce!
//
// For example, consider the following rules:
//
// light red bags contain 1 bright white bag, 2 muted yellow bags.
// dark orange bags contain 3 bright white bags, 4 muted yellow bags.
// bright white bags contain 1 shiny gold bag.
// muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
// shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
// dark olive bags contain 3 faded blue bags, 4 dotted black bags.
// vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
// faded blue bags contain no other bags.
// dotted black bags contain no other bags.
//
// These rules specify the required contents for 9 bag types. In this
// example, every faded blue bag is empty, every vibrant plum bag
// contains 11 bags (5 faded blue and 6 dotted black), and so on.
//
// You have a shiny gold bag. If you wanted to carry it in at least one
// other bag, how many different bag colors would be valid for the
// outermost bag? (In other words: how many colors can, eventually,
// contain at least one shiny gold bag?)
//
// In the above rules, the following options would be available to you:
//
// * A bright white bag, which can hold your shiny gold bag directly.
//
// * A muted yellow bag, which can hold your shiny gold bag directly,
// plus some other bags.
//
// * A dark orange bag, which can hold bright white and muted yellow
// bags, either of which could then hold your shiny gold bag.
//
// * A light red bag, which can hold bright white and muted yellow
// bags, either of which could then hold your shiny gold bag.
//
// So, in this example, the number of bag colors that can eventually
// contain at least one shiny gold bag is 4.
//
// How many bag colors can eventually contain at least one shiny gold
// bag? (The list of rules is quite long; make sure you get all of it.)
//
// --- Part Two ---
// ----------------
//
// It's getting pretty expensive to fly these days - not because of
// ticket prices, but because of the ridiculous number of bags you need
// to buy!
//
// Consider again your shiny gold bag and the rules from the above
// example:
//
// * faded blue bags contain 0 other bags.
//
// * dotted black bags contain 0 other bags.
//
// * vibrant plum bags contain 11 other bags: 5 faded blue bags and 6
// dotted black bags.
//
// * dark olive bags contain 7 other bags: 3 faded blue bags and 4
// dotted black bags.
//
// So, a single shiny gold bag must contain 1 dark olive bag (and the 7
// bags within it) plus 2 vibrant plum bags (and the 11 bags within each
// of those): 1 + 1*7 + 2 + 2*11 = 32 bags!
//
// Of course, the actual rules have a small chance of going several
// levels deeper than this example; be sure to count all of the bags,
// even if the nesting becomes topologically impractical!
//
// Here's another example:
//
// shiny gold bags contain 2 dark red bags.
// dark red bags contain 2 dark orange bags.
// dark orange bags contain 2 dark yellow bags.
// dark yellow bags contain 2 dark green bags.
// dark green bags contain 2 dark blue bags.
// dark blue bags contain 2 dark violet bags.
// dark violet bags contain no other bags.
//
// In this example, a single shiny gold bag must contain 126 other bags.
//
// How many individual bags are required inside your single shiny gold
// bag?

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
  absl::flat_hash_map<absl::string_view, int> bag_to_count;
};

absl::StatusOr<DirectedGraph<BagRule>> Parse(
    absl::Span<absl::string_view> input) {
  DirectedGraph<BagRule> ret;
  for (absl::string_view str : input) {
    const auto [outer, inner] = PairSplit(str, " bags contain ");
    if (ret.GetData(outer) != nullptr) {
      return Error("color dupe: ", outer);
    }
    if (inner == "no other bags.") {
      // OK for the rule to be empty.
      ret.AddNode(outer, BagRule{});
    } else {
      BagRule bag_rule;
      for (absl::string_view bag_rule_str : absl::StrSplit(inner, ", ")) {
        static LazyRE2 bag_pattern{"(\\d+) (.*) bags?\\.?"};
        int count;
        absl::string_view color;
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
                        absl::string_view bag) {
  absl::flat_hash_set<absl::string_view> can_contain;
  absl::flat_hash_set<absl::string_view> added = {bag};
  while (!added.empty()) {
    absl::flat_hash_set<absl::string_view> new_added;
    for (const absl::string_view& color : added) {
      const std::vector<absl::string_view>* incoming = bags.Incoming(color);
      if (incoming != nullptr) {
        for (absl::string_view parent_color : *incoming) {
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
                                       absl::string_view bag) {
  const BagRule* bag_rule = bags.GetData(bag);
  if (bag_rule == nullptr) {
    return Error("Cannot find bag: ", bag);
  }
  const std::vector<absl::string_view>* outgoing = bags.Outgoing(bag);
  int bag_count = 1;
  if (outgoing != nullptr) {
    for (absl::string_view sub_bag : *outgoing) {
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<BagRule>> bags = Parse(input);
  if (!bags.ok()) return bags.status();
  VLOG(1) << bags->nodes().size();
  return IntReturn(CountContainingBags(*bags, "shiny gold"));
}

absl::StatusOr<std::string> Day_2020_07::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<BagRule>> bags = Parse(input);
  if (!bags.ok()) return bags.status();

  absl::StatusOr<int> contained_bags = CountContainedBags(*bags, "shiny gold");
  if (!contained_bags.ok()) return bags.status();
  return IntReturn(*contained_bags - 1); /* don't include top bag */
}

}  // namespace advent_of_code
