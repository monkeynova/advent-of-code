#include "advent_of_code/2017/day25/day25.h"

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

class TuringMachine {
 public:
  static absl::StatusOr<TuringMachine> Parse(
      absl::Span<std::string_view> input) {
    TuringMachine ret;
    Rule cur_rule;
    Rule::If* cur_if = nullptr;
    for (std::string_view row : input) {
      int n;
      std::string_view str;
      if (RE2::FullMatch(row, "Begin in state (.*).", &ret.cur_state_)) {
        // OK.
      } else if (RE2::FullMatch(
                     row, "Perform a diagnostic checksum after (\\d+) steps.",
                     &ret.stop_steps_)) {
        // OK.
      } else if (RE2::FullMatch(row, "In state (.*):", &str)) {
        if (!cur_rule.in_state.empty()) {
          ret.state_to_rule_[cur_rule.in_state] = cur_rule;
          cur_rule = Rule{};
          cur_if = nullptr;
        }
        cur_rule.in_state = str;
      } else if (RE2::FullMatch(row, "  If the current value is ([01]):", &n)) {
        if (cur_rule.in_state.empty())
          return Error("Current value with no state");
        if (n == 0) {
          cur_if = &cur_rule.if0;
        } else if (n == 1) {
          cur_if = &cur_rule.if1;
        } else {
          return Error("Bad current value: ", row);
        }
      } else if (RE2::FullMatch(row, "    - Write the value ([01]).", &n)) {
        if (cur_if == nullptr) return Error("Write with no current");
        cur_if->write = n;
      } else if (RE2::FullMatch(row, "    - Move one slot to the (left|right).",
                                &str)) {
        if (cur_if == nullptr) return Error("Write with no current");
        if (str == "left") {
          cur_if->dir = -1;
        } else {
          cur_if->dir = 1;
        }
      } else if (RE2::FullMatch(row, "    - Continue with state (.*).", &str)) {
        if (cur_if == nullptr) return Error("Write with no current");
        cur_if->next_state = str;
      } else if (row.empty()) {
        // OK.
      } else {
        return Error("Bad line: ", row);
      }
    }
    if (!cur_rule.in_state.empty()) {
      ret.state_to_rule_[cur_rule.in_state] = cur_rule;
    }
    return ret;
  }

  absl::Status Run() {
    for (int step = 0; step < stop_steps_; ++step) {
      auto it = state_to_rule_.find(cur_state_);
      if (it == state_to_rule_.end()) return Error("Bad state: ", cur_state_);
      const Rule& r = it->second;
      const Rule::If& rif = tape_.contains(head_pos_) ? r.if1 : r.if0;
      if (rif.write)
        tape_.insert(head_pos_);
      else
        tape_.erase(head_pos_);
      head_pos_ += rif.dir;
      cur_state_ = rif.next_state;
    }
    return absl::OkStatus();
  }

  int Checksum() { return tape_.size(); }

 private:
  TuringMachine() = default;

  struct Rule {
    struct If {
      int dir = 0;
      std::string_view next_state;
      int write = -1;
    } if0, if1;
    std::string_view in_state;
  };

  absl::flat_hash_set<int> tape_;
  int head_pos_ = 0;
  std::string_view cur_state_;
  absl::flat_hash_map<std::string_view, Rule> state_to_rule_;
  int stop_steps_;
};

}  // namespace

absl::StatusOr<std::string> Day_2017_25::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(TuringMachine tm, TuringMachine::Parse(input));
  RETURN_IF_ERROR(tm.Run());
  return AdventReturn(tm.Checksum());
}

absl::StatusOr<std::string> Day_2017_25::Part2(
    absl::Span<std::string_view> input) const {
  return "Merry Christmas!";
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/25, []() {
  return std::unique_ptr<AdventDay>(new Day_2017_25());
});

}  // namespace advent_of_code
