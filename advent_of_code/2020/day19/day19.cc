// https://adventofcode.com/2020/day/19
//
// --- Day 19: Monster Messages ---
// --------------------------------
//
// You land in an airport surrounded by dense forest. As you walk to your
// high-speed train, the Elves at the Mythical Information Bureau contact
// you again. They think their satellite has collected an image of a sea
// monster! Unfortunately, the connection to the satellite is having
// problems, and many of the messages sent back from the satellite have
// been corrupted.
//
// They sent you a list of the rules valid messages should obey and a
// list of received messages they've collected so far (your puzzle
// input).
//
// The rules for valid messages (the top part of your puzzle input) are
// numbered and build upon each other. For example:
//
// 0: 1 2
// 1: "a"
// 2: 1 3 | 3 1
// 3: "b"
//
// Some rules, like 3: "b", simply match a single character (in this
// case, b).
//
// The remaining rules list the sub-rules that must be followed; for
// example, the rule 0: 1 2 means that to match rule 0, the text being
// checked must match rule 1, and the text after the part that matched
// rule 1 must then match rule 2.
//
// Some of the rules have multiple lists of sub-rules separated by a pipe
// (|). This means that at least one list of sub-rules must match. (The
// ones that match might be different each time the rule is encountered.)
// For example, the rule 2: 1 3 | 3 1 means that to match rule 2, the
// text being checked must match rule 1 followed by rule 3 or it must
// match rule 3 followed by rule 1.
//
// Fortunately, there are no loops in the rules, so the list of possible
// matches will be finite. Since rule 1 matches a and rule 3 matches b,
// rule 2 matches either ab or ba. Therefore, rule 0 matches aab or aba.
//
// Here's a more interesting example:
//
// 0: 4 1 5
// 1: 2 3 | 3 2
// 2: 4 4 | 5 5
// 3: 4 5 | 5 4
// 4: "a"
// 5: "b"
//
// Here, because rule 4 matches a and rule 5 matches b, rule 2 matches
// two letters that are the same (aa or bb), and rule 3 matches two
// letters that are different (ab or ba).
//
// Since rule 1 matches rules 2 and 3 once each in either order, it must
// match two pairs of letters, one pair with matching letters and one
// pair with different letters. This leaves eight possibilities: aaab,
// aaba, bbab, bbba, abaa, abbb, baaa, or babb.
//
// Rule 0, therefore, matches a (rule 4), then any of the eight options
// from rule 1, then b (rule 5): aaaabb, aaabab, abbabb, abbbab, aabaab,
// aabbbb, abaaab, or ababbb.
//
// The received messages (the bottom part of your puzzle input) need to
// be checked against the rules so you can determine which are valid and
// which are corrupted. Including the rules and the messages together,
// this might look like:
//
// 0: 4 1 5
// 1: 2 3 | 3 2
// 2: 4 4 | 5 5
// 3: 4 5 | 5 4
// 4: "a"
// 5: "b"
//
// ababbb
// bababa
// abbbab
// aaabbb
// aaaabbb
//
// Your goal is to determine the number of messages that completely match
// rule 0. In the above example, ababbb and abbbab match, but bababa,
// aaabbb, and aaaabbb do not, producing the answer 2. The whole message
// must match all of rule 0; there can't be extra unmatched characters in
// the message. (For example, aaaabbb might appear to match rule 0 above,
// but it has an extra unmatched b on the end.)
//
// How many messages completely match rule 0?
//
// --- Part Two ---
// ----------------
//
// As you look over the list of messages, you realize your matching rules
// aren't quite right. To fix them, completely replace rules 8: 42 and
// 11: 42 31 with the following:
//
// 8: 42 | 42 8
// 11: 42 31 | 42 11 31
//
// This small change has a big impact: now, the rules do contain loops,
// and the list of messages they could hypothetically match is infinite.
// You'll need to determine how these changes affect which messages are
// valid.
//
// Fortunately, many of the rules are unaffected by this change; it might
// help to start by looking at which rules always match the same set of
// values and how those rules (especially rules 42 and 31) are used by
// the new versions of rules 8 and 11.
//
// (Remember, you only need to handle the rules you have; building a
// solution that could handle any hypothetical combination of rules would
// be significantly more difficult.)
//
// For example:
//
// 42: 9 14 | 10 1
// 9: 14 27 | 1 26
// 10: 23 14 | 28 1
// 1: "a"
// 11: 42 31
// 5: 1 14 | 15 1
// 19: 14 1 | 14 14
// 12: 24 14 | 19 1
// 16: 15 1 | 14 14
// 31: 14 17 | 1 13
// 6: 14 14 | 1 14
// 2: 1 24 | 14 4
// 0: 8 11
// 13: 14 3 | 1 12
// 15: 1 | 14
// 17: 14 2 | 1 7
// 23: 25 1 | 22 14
// 28: 16 1
// 4: 1 1
// 20: 14 14 | 1 15
// 3: 5 14 | 16 1
// 27: 1 6 | 14 18
// 14: "b"
// 21: 14 1 | 1 14
// 25: 1 1 | 1 14
// 22: 14 14
// 8: 42
// 26: 14 22 | 1 20
// 18: 15 15
// 7: 14 5 | 1 21
// 24: 14 1
//
// abbbbbabbbaaaababbaabbbbabababbbabbbbbbabaaaa
// bbabbbbaabaabba
// babbbbaabbbbbabbbbbbaabaaabaaa
// aaabbbbbbaaaabaababaabababbabaaabbababababaaa
// bbbbbbbaaaabbbbaaabbabaaa
// bbbababbbbaaaaaaaabbababaaababaabab
// ababaaaaaabaaab
// ababaaaaabbbaba
// baabbaaaabbaaaababbaababb
// abbbbabbbbaaaababbbbbbaaaababb
// aaaaabbaabaaaaababaa
// aaaabbaaaabbaaa
// aaaabbaabbaaaaaaabbbabbbaaabbaabaaa
// babaaabbbaaabaababbaabababaaab
// aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba
//
// Without updating rules 8 and 11, these rules only match three
// messages: bbabbbbaabaabba, ababaaaaaabaaab, and ababaaaaabbbaba.
//
// However, after updating rules 8 and 11, a total of 12 messages match:
//
// * bbabbbbaabaabba
//
// * babbbbaabbbbbabbbbbbaabaaabaaa
//
// * aaabbbbbbaaaabaababaabababbabaaabbababababaaa
//
// * bbbbbbbaaaabbbbaaabbabaaa
//
// * bbbababbbbaaaaaaaabbababaaababaabab
//
// * ababaaaaaabaaab
//
// * ababaaaaabbbaba
//
// * baabbaaaabbaaaababbaababb
//
// * abbbbabbbbaaaababbbbbbaaaababb
//
// * aaaaabbaabaaaaababaa
//
// * aaaabbaabbaaaaaaabbbabbbaaabbaabaaa
//
// * aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba
//
// After updating rules 8 and 11, how many messages completely match rule
// 0?

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

absl::StatusOr<Rule> ParseRule(absl::string_view in) {
  Rule ret = {0, '\0', {}};
  const auto [rule, dest] = PairSplit(in, ": ");
  if (!absl::SimpleAtoi(rule, &ret.rule_num)) {
    return Error("Bad number: ", rule);
  }
  if (!RE2::FullMatch(dest, "\"(.)\"", &ret.token)) {
    for (absl::string_view piece : absl::StrSplit(dest, " | ")) {
      absl::StatusOr<std::vector<int64_t>> sub_rule =
          ParseAsInts(absl::StrSplit(piece, " "));
      if (!sub_rule.ok()) return sub_rule.status();
      ret.sub_rules.push_back(*sub_rule);
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

  for (const std::vector<int64_t>& sub_rule : r.sub_rules) {
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

bool MatchRuleSet(const absl::flat_hash_map<int, Rule>& rule_set,
                  absl::string_view str) {
  if (!MatchRulePartial(rule_set, 0, &str)) return false;
  if (!str.empty()) return false;
  return true;
}

struct RuleHist {
  absl::string_view str;
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
  absl::string_view str;
  absl::flat_hash_set<RuleHist> rule_hist;
  bool operator<(const State& o) const {
    // Priority queue orders by greater-than.
    // We want shortest strings first.
    return str.size() > o.str.size();
  }
};

bool MatchRuleSetWalk(const absl::flat_hash_map<int, Rule>& rule_set,
                      absl::string_view str) {
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
      if (rule_set.contains(rule->rule_num)) {
        return Error("Duplicate rule: ", rule->rule_num);
      }
      rule_set.emplace(rule->rule_num, *rule);
    } else {
      messages.push_back(in);
    }
  }

  int match_count = 0;
  for (absl::string_view m : messages) {
    bool matches = MatchRuleSet(rule_set, m);
    if (matches) ++match_count;
    VLOG(1) << m << "; " << (matches ? "MATCH" : "<no match>");
  }

  return IntReturn(match_count);
}

absl::StatusOr<std::string> Day_2020_19::Part2(
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
      if (rule_set.contains(rule->rule_num)) {
        return Error("Duplicate rule: ", rule->rule_num);
      }
      rule_set.emplace(rule->rule_num, *rule);
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
  for (absl::string_view m : messages) {
    bool matches = MatchRuleSetWalk(rule_set, m);
    if (matches) ++match_count;
    VLOG(1) << m << "; " << (matches ? "MATCH" : "<no match>");
  }

  return IntReturn(match_count);
}

}  // namespace advent_of_code
