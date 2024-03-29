#include "advent_of_code/2020/day19/day19.h"

#include <queue>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Rule {
  int rule_num;
  char token;
  std::vector<std::vector<int64_t>> sub_rules;
};

absl::StatusOr<Rule> ParseRule(std::string_view in) {
  Rule ret = {0, '\0', {}};
  const auto [rule, dest] = PairSplit(in, ": ");
  if (!absl::SimpleAtoi(rule, &ret.rule_num)) {
    return Error("Bad number: ", rule);
  }
  if (!RE2::FullMatch(dest, "\"(.)\"", &ret.token)) {
    for (std::string_view piece : absl::StrSplit(dest, " | ")) {
      ASSIGN_OR_RETURN(std::vector<int64_t> sub_rule,
                       ParseAsInts(absl::StrSplit(piece, " ")));
      ret.sub_rules.push_back(std::move(sub_rule));
    }
  }

  return ret;
}

bool MatchRulePartial(const absl::flat_hash_map<int, Rule>& rule_set,
                      int rule_num, std::string_view* str) {
  auto it = rule_set.find(rule_num);
  // TODO(@monkeynova): Error.
  CHECK(it != rule_set.end());
  const Rule& r = it->second;
  if (r.token != '\0') {
    if (str->empty() || (*str)[0] != r.token) return false;
    *str = str->substr(1);
    return true;
  }

  for (const std::vector<int64_t>& sub_rule : r.sub_rules) {
    std::string_view tmp = *str;
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

bool MatchRuleSet(const absl::flat_hash_map<int, Rule>& rule_set,
                  std::string_view str) {
  if (!MatchRulePartial(rule_set, 0, &str)) return false;
  if (!str.empty()) return false;
  return true;
}

struct RuleHist {
  std::string_view str;
  int rule_num;
  bool operator==(const RuleHist& o) const {
    return str == o.str && rule_num == o.rule_num;
  }
  bool operator!=(const RuleHist& o) const { return !operator==(o); }
  template <typename H>
  friend H AbslHashValue(H h, const RuleHist& rh) {
    return H::combine(std::move(h), rh.str, rh.rule_num);
  }
};

struct State {
  std::vector<int> rule_chain;
  std::string_view str;
  absl::flat_hash_set<RuleHist> rule_hist;
  bool operator<(const State& o) const {
    // Priority queue orders by greater-than.
    // We want shortest strings first.
    return str.size() > o.str.size();
  }
};

bool MatchRuleSetWalk(const absl::flat_hash_map<int, Rule>& rule_set,
                      std::string_view str) {
  std::priority_queue<State> frontier;
  frontier.push({.rule_chain = {0}, .str = str});
  while (!frontier.empty()) {
    State state = frontier.top();
    frontier.pop();
    VLOG(2) << absl::StrJoin(state.rule_chain, ", ") << ", ";
    if (state.rule_chain.empty()) {
      // Ran out of rules. If we're at the end of string,
      // return true. Otherwise remove this branch and
      // continue.
      if (state.str == "") return true;
      continue;
    }

    int next_rule = state.rule_chain.back();

    // Skip infinite loops. If we want to parse on a rule loop, it must
    // consume some input.
    RuleHist rh{.str = state.str, .rule_num = next_rule};
    if (state.rule_hist.contains(rh)) continue;
    state.rule_hist.insert(rh);

    state.rule_chain.pop_back();
    auto it = rule_set.find(next_rule);
    CHECK(it != rule_set.end()) << next_rule;
    const Rule& r = it->second;
    if (r.token != '\0') {
      // Matches a literal. Try to pull it off.
      if (!state.str.empty() && state.str[0] == r.token) {
        state.str = state.str.substr(1);
        frontier.push(state);
      }
      continue;
    }

    for (const std::vector<int64_t>& sub_rules : r.sub_rules) {
      State new_state = state;
      new_state.rule_chain.insert(new_state.rule_chain.end(),
                                  sub_rules.rbegin(), sub_rules.rend());
      frontier.push(new_state);
    }
  }
  return false;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2020_19::Part1(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<int, Rule> rule_set;
  bool parse_rules = true;
  std::vector<std::string_view> messages;
  for (std::string_view in : input) {
    if (in.empty()) {
      parse_rules = false;
      continue;
    }
    if (parse_rules) {
      ASSIGN_OR_RETURN(Rule rule, ParseRule(in));
      if (rule_set.contains(rule.rule_num)) {
        return Error("Duplicate rule: ", rule.rule_num);
      }
      rule_set.emplace(rule.rule_num, rule);
    } else {
      messages.push_back(in);
    }
  }

  int match_count = 0;
  for (std::string_view m : messages) {
    bool matches = MatchRuleSet(rule_set, m);
    if (matches) ++match_count;
    VLOG(1) << m << "; " << (matches ? "MATCH" : "<no match>");
  }

  return AdventReturn(match_count);
}

absl::StatusOr<std::string> Day_2020_19::Part2(
    absl::Span<std::string_view> input) const {
  absl::flat_hash_map<int, Rule> rule_set;
  bool parse_rules = true;
  std::vector<std::string_view> messages;
  for (std::string_view in : input) {
    if (in.empty()) {
      parse_rules = false;
      continue;
    }
    if (parse_rules) {
      ASSIGN_OR_RETURN(Rule rule, ParseRule(in));
      if (rule_set.contains(rule.rule_num)) {
        return Error("Duplicate rule: ", rule.rule_num);
      }
      rule_set.emplace(rule.rule_num, rule);
    } else {
      messages.push_back(in);
    }
  }

  rule_set.insert_or_assign(
      8, Rule{.rule_num = 8, .token = '\0', .sub_rules = {{42}, {42, 8}}});
  rule_set.insert_or_assign(11, Rule{.rule_num = 11,
                                     .token = '\0',
                                     .sub_rules = {{42, 31}, {42, 11, 31}}});

  if (VLOG_IS_ON(2)) {
    for (const auto& [rule_num, rule] : rule_set) {
      VLOG(2) << "Rule: " << rule_num << " (" << rule.rule_num << ")";
      char tmp_token[] = {rule.token, '\0'};
      if (rule.token != '\0') VLOG(1) << "  token = " << tmp_token;
      if (!rule.sub_rules.empty()) {
        for (const auto& sub_rule : rule.sub_rules) {
          VLOG(2) << "  SubRule: " << absl::StrJoin(sub_rule, "|");
        }
      }
    }
  }

  int match_count = 0;
  for (std::string_view m : messages) {
    bool matches = MatchRuleSetWalk(rule_set, m);
    if (matches) ++match_count;
    VLOG(1) << m << "; " << (matches ? "MATCH" : "<no match>");
  }

  return AdventReturn(match_count);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_19()); });

}  // namespace advent_of_code
