// https://adventofcode.com/2017/day/25
//
// --- Day 25: The Halting Problem ---
// -----------------------------------
//
// Following the twisty passageways deeper and deeper into the CPU, you
// finally reach the core of the computer. Here, in the expansive central
// chamber, you find a grand apparatus that fills the entire room,
// suspended nanometers above your head.
//
// You had always imagined CPUs to be noisy, chaotic places, bustling
// with activity. Instead, the room is quiet, motionless, and dark.
//
// Suddenly, you and the CPU's garbage collector startle each other.
// "It's not often we get many visitors here!", he says. You inquire
// about the stopped machinery.
//
// "It stopped milliseconds ago; not sure why. I'm a garbage collector,
// not a doctor." You ask what the machine is for.
//
// "Programs these days, don't know their origins. That's the Turing
// machine! It's what makes the whole computer work." You try to explain
// that Turing machines are merely models of computation, but he cuts you
// off. "No, see, that's just what they want you to think. Ultimately,
// inside every CPU, there's a Turing machine driving the whole thing!
// Too bad this one's broken. We're doomed!"
//
// You ask how you can help. "Well, unfortunately, the only way to get
// the computer running again would be to create a whole new Turing
// machine from scratch, but there's no way you can-" He notices the look
// on your face, gives you a curious glance, shrugs, and goes back to
// sweeping the floor.
//
// You find the Turing machine blueprints (your puzzle input) on a tablet
// in a nearby pile of debris. Looking back up at the broken Turing
// machine above, you can start to identify its parts:
//
// * A tape which contains 0 repeated infinitely to the left and right.
//
// * A cursor, which can move left or right along the tape and read or
// write values at its current position.
//
// * A set of states, each containing rules about what to do based on
// the current value under the cursor.
//
// Each slot on the tape has two possible values: 0 (the starting value
// for all slots) and 1. Based on whether the cursor is pointing at a 0
// or a 1, the current state says what value to write at the current
// position of the cursor, whether to move the cursor left or right one
// slot, and which state to use next.
//
// For example, suppose you found the following blueprint:
//
// Begin in state A.
// Perform a diagnostic checksum after 6 steps.
//
// In state A:
// If the current value is 0:
// - Write the value 1.
// - Move one slot to the right.
// - Continue with state B.
// If the current value is 1:
// - Write the value 0.
// - Move one slot to the left.
// - Continue with state B.
//
// In state B:
// If the current value is 0:
// - Write the value 1.
// - Move one slot to the left.
// - Continue with state A.
// If the current value is 1:
// - Write the value 1.
// - Move one slot to the right.
// - Continue with state A.
//
// Running it until the number of steps required to take the listed
// diagnostic checksum would result in the following tape configurations
// (with the cursor marked in square brackets):
//
// ... 0  0  0 [0] 0  0 ... (before any steps; about to run state A)
// ... 0  0  0  1 [0] 0 ... (after 1 step;     about to run state B)
// ... 0  0  0 [1] 1  0 ... (after 2 steps;    about to run state A)
// ... 0  0 [0] 0  1  0 ... (after 3 steps;    about to run state B)
// ... 0 [0] 1  0  1  0 ... (after 4 steps;    about to run state A)
// ... 0  1 [1] 0  1  0 ... (after 5 steps;    about to run state B)
// ... 0  1  1 [0] 1  0 ... (after 6 steps;    about to run state A)
//
// The CPU can confirm that the Turing machine is working by taking a
// diagnostic checksum after a specific number of steps (given in the
// blueprint). Once the specified number of steps have been executed, the
// Turing machine should pause; once it does, count the number of times 1
// appears on the tape. In the above example, the diagnostic checksum is
// 3.
//
// Recreate the Turing machine and save the computer! What is the
// diagnostic checksum it produces once it's working again?
//
// --- Part Two ---
// ----------------
//
// The Turing machine, and soon the entire computer, springs back to
// life. A console glows dimly nearby, awaiting your command.
//
// > reboot printer
// Error: That command requires    priority 50   . You currently have priority 0
// . You must deposit    50 stars    to increase your priority to the required
// level.
//
// The console flickers for a moment, and then prints another message:
//
// Star    accepted.
// You must deposit    49 stars    to increase your priority to the required
// level.
//
// The garbage collector winks at you, then continues sweeping.

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
      absl::Span<absl::string_view> input) {
    TuringMachine ret;
    Rule cur_rule;
    Rule::If* cur_if = nullptr;
    for (absl::string_view row : input) {
      int n;
      absl::string_view str;
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
      absl::string_view next_state;
      int write = -1;
    } if0, if1;
    absl::string_view in_state;
  };

  absl::flat_hash_set<int> tape_;
  int head_pos_ = 0;
  absl::string_view cur_state_;
  absl::flat_hash_map<absl::string_view, Rule> state_to_rule_;
  int stop_steps_;
};

}  // namespace

absl::StatusOr<std::string> Day_2017_25::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<TuringMachine> tm = TuringMachine::Parse(input);
  if (!tm.ok()) return tm.status();

  if (absl::Status st = tm->Run(); !st.ok()) return st;

  return IntReturn(tm->Checksum());
}

absl::StatusOr<std::string> Day_2017_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
