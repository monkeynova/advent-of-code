#include "advent_of_code/2020/day19/day19.h"

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

struct Rule {
  int rule_num;
  char token;
  std::vector<std::vector<int>> sub_rules;
};

absl::StatusOr<Rule> ParseRule(absl::string_view in) {
  Rule ret = {0, '\0', {}};
  std::vector<absl::string_view> rule_and_dest = absl::StrSplit(in, ": ");
  if (rule_and_dest.size() != 2) {
    return AdventDay::Error("Bad rule: ", in);
  }
  if (!absl::SimpleAtoi(rule_and_dest[0], &ret.rule_num)) {
    return AdventDay::Error("Bad number: ", rule_and_dest[0]);
  }
  if (!RE2::FullMatch(rule_and_dest[1], "\"(.)\"", &ret.token)) {
    std::vector<absl::string_view> pieces = absl::StrSplit(rule_and_dest[1], " | ");
    for (absl::string_view piece : pieces) {
      std::vector<absl::string_view> rule_order = absl::StrSplit(piece, " ");
      std::vector<int> sub_rule;
      for (absl::string_view sub_rule_num : rule_order) {
        sub_rule.push_back(0);
        if (!absl::SimpleAtoi(sub_rule_num, &sub_rule.back())) {
          return AdventDay::Error("Bad number: [", sub_rule_num, "]");
        }
      }
      ret.sub_rules.push_back(sub_rule);
    }
  }

  return ret;
}

bool MatchRulePartial(const absl::flat_hash_map<int, Rule>& rule_set,
                      int rule_num, absl::string_view* str) {
  auto it = rule_set.find(rule_num);
  // TODO(@monkeynova): Error.
  CHECK(it != rule_set.end());
  const Rule& r = it->second;
  if (r.token != '\0') {
    if (str->empty() || (*str)[0] != r.token) return false;
    *str = str->substr(1);
    return true;
  }
    
  for (const std::vector<int>& sub_rule : r.sub_rules) {
    absl::string_view tmp = *str;
    bool match_all = true;
    for (int rule_num : sub_rule) {
      if (!MatchRulePartial(rule_set, rule_num, &tmp)) {
        match_all = false;
        break;
      }
    }
    if (match_all) {
      *str = tmp;
      return true;
    }
  }

  return false;
}

bool MatchRuleSet(const absl::flat_hash_map<int, Rule>& rule_set, absl::string_view str) {
  if (!MatchRulePartial(rule_set, 0, &str)) return false;
  if (!str.empty()) return false;
  return true;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day19_2020::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int, Rule> rule_set;
  bool parse_rules = true;
  std::vector<absl::string_view> messages;
  for (absl::string_view in : input) {
    if (in.empty()) {
      parse_rules = false;
      continue;
    }
    if (parse_rules) {
      absl::StatusOr<Rule> rule = ParseRule(in);
      if (!rule.ok()) return rule.status();
      if (rule_set.contains(rule->rule_num)) return Error("Duplicate rule: ", rule->rule_num);
      rule_set.emplace(rule->rule_num, *rule);
    } else {
      messages.push_back(in);
    }
  }

  int match_count = 0;
  for (absl::string_view m : messages) {
    if (MatchRuleSet(rule_set, m)) ++match_count;
  }

  return IntReturn(match_count);
}

absl::StatusOr<std::vector<std::string>> Day19_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
