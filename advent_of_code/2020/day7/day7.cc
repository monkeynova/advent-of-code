#include "advent_of_code/2020/day7/day7.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

struct BagRule {
  std::string color;
  int count;
};

int CountContainingBags(
    const absl::flat_hash_map<std::string, std::vector<std::string>>& reverse,
    absl::string_view bag) {
  absl::flat_hash_set<std::string> can_contain;
  absl::flat_hash_set<std::string> added = {std::string(bag)};
  while (!added.empty()) {
    absl::flat_hash_set<std::string> new_added;
    for (const std::string& color : added) {
      if (auto it = reverse.find(color); it != reverse.end()) {
        for (const std::string& parent_color : it->second) {
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

absl::StatusOr<int> CountContainedBags(const absl::flat_hash_map<std::string, std::vector<BagRule>>& dependency,
     absl::string_view bag) {
  auto it = dependency.find(bag);
  if (it == dependency.end()) return absl::InvalidArgumentError("can't find bag");
  int bag_count = 1;
  for (const BagRule& rule : it->second) {
    absl::StatusOr<int> sub_bags = CountContainedBags(dependency, rule.color);
    if (!sub_bags.ok()) return sub_bags.status();
    bag_count += rule.count * *sub_bags;
  }
  return bag_count;
}

absl::StatusOr<std::vector<std::string>> Day7_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::flat_hash_map<std::string, std::vector<BagRule>> dependency;
  absl::flat_hash_map<std::string, std::vector<std::string>> reverse;
  for (absl::string_view str : input) {
    std::vector<absl::string_view> pieces = absl::StrSplit(str, " bags contain ");
    if (pieces.size() != 2) return absl::InvalidArgumentError("contains");
    std::string color = std::string(pieces[0]);
    if (dependency.contains(color)) return absl::InvalidArgumentError("color dupe");
    std::vector<BagRule>& insert = dependency[color];
    if (pieces[1] == "no other bags.") {
      // OK for the rule to be empty.
    } else {
      for (absl::string_view bag_rule_str : absl::StrSplit(pieces[1], ", ")) {
        BagRule bag_rule;
        if (!RE2::FullMatch(bag_rule_str, "(\\d+) (.*) bags?\\.?", &bag_rule.count, &bag_rule.color)) {
          return absl::InvalidArgumentError(absl::StrCat("bag rule: ", bag_rule_str));
        }
        insert.push_back(bag_rule);
        reverse[bag_rule.color].push_back(color);
      }
    }
  }
  LOG(WARNING) << dependency.size();
  return IntReturn(CountContainingBags(reverse, "shiny gold"));
}

absl::StatusOr<std::vector<std::string>> Day7_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::flat_hash_map<std::string, std::vector<BagRule>> dependency;
  for (absl::string_view str : input) {
    std::vector<absl::string_view> pieces = absl::StrSplit(str, " bags contain ");
    if (pieces.size() != 2) return absl::InvalidArgumentError("contains");
    std::string color = std::string(pieces[0]);
    if (dependency.contains(color)) return absl::InvalidArgumentError("color dupe");
    std::vector<BagRule>& insert = dependency[color];
    if (pieces[1] == "no other bags.") {
      // OK for the rule to be empty.
    } else {
      for (absl::string_view bag_rule_str : absl::StrSplit(pieces[1], ", ")) {
        BagRule bag_rule;
        if (!RE2::FullMatch(bag_rule_str, "(\\d+) (.*) bags?\\.?", &bag_rule.count, &bag_rule.color)) {
          return absl::InvalidArgumentError(absl::StrCat("bag rule: ", bag_rule_str));
        }
        insert.push_back(bag_rule);
      }
    }
  }
  absl::StatusOr<int> bags = CountContainedBags(dependency, "shiny gold");
  if (!bags.ok()) return bags.status();
  return IntReturn(*bags - 1); /* don't include top bag */
}
