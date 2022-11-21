// http://adventofcode.com/2021/day/21

// http://adventofcode.com/2021/day/21

#include "advent_of_code/2021/day21/day21.h"

#include "absl/algorithm/container.h"
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

struct State {
  int p1;
  int p2;
  int s1;
  int s2;

  void SetP1Roll(int roll) {
    p1 = (p1 + roll) % 10;
    s1 += p1 + 1;
  }

  void SetP2Roll(int roll) {
    p2 = (p2 + roll) % 10;
    s2 += p2 + 1;
  }

  bool operator==(const State& o) const {
    return p1 == o.p1 && p2 == o.p2 && s1 == o.s1 && s2 == o.s2;
  }
  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.p1, s.p2, s.s1, s.s2);
  }
  friend std::ostream& operator<<(std::ostream& o, const State& state) {
    return o << "1 @" << state.p1 << " (s=" << state.s1 << "); "
             << "2 @" << state.p2 << " (s=" << state.s2 << ")";
  }
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_21::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int p1;
  int p2;
  if (!RE2::FullMatch(input[0], "Player 1 starting position: (\\d+)", &p1)) {
    return Error("Bad p1");
  }
  if (!RE2::FullMatch(input[1], "Player 2 starting position: (\\d+)", &p2)) {
    return Error("Bad p2");
  }
  State game = {p1 - 1, p2 - 1, 0, 0};
  int die = -1;
  auto roll = [&die]() {
    die = (die + 1) % 100;
    return die + 1;
  };
  for (int turn = 1; true; turn += 2) {
    game.SetP1Roll(roll() + roll() + roll());
    VLOG(2) << game;
    if (game.s1 >= 1000) {
      return IntReturn(3 * turn * game.s2);
    }
    game.SetP2Roll(roll() + roll() + roll());
    VLOG(2) << game;
    if (game.s2 >= 1000) {
      return IntReturn(3 * (turn + 1) * game.s1);
    }
  }
  return Error("Left infinite loop");
}

absl::StatusOr<std::string> Day_2021_21::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int p1;
  int p2;
  if (!RE2::FullMatch(input[0], "Player 1 starting position: (\\d+)", &p1)) {
    return Error("Bad p1");
  }
  if (!RE2::FullMatch(input[1], "Player 2 starting position: (\\d+)", &p2)) {
    return Error("Bad p2");
  }
  int64_t p1_wins = 0;
  int64_t p2_wins = 0;

  const absl::flat_hash_map<int, int> kRollToFrequency = {
      {3, 1}, {4, 3}, {5, 6}, {6, 7}, {7, 6}, {8, 3}, {9, 1},
  };

  absl::flat_hash_map<State, int64_t> state_map = {
      {{p1 - 1, p2 - 1, 0, 0}, 1},
  };
  while (!state_map.empty()) {
    absl::flat_hash_map<State, int64_t> new_state_map;
    VLOG(2) << "new turn";
    for (const auto& [state, count] : state_map) {
      VLOG(2) << state;
      for (const auto& [p1_roll, freq1] : kRollToFrequency) {
        int64_t sub_count1 = count * freq1;
        State p1_state = state;
        p1_state.SetP1Roll(p1_roll);
        if (p1_state.s1 >= 21) {
          // Victory for p1, no longer need to track this branch.
          p1_wins += sub_count1;
          continue;
        }
        for (const auto& [p2_roll, freq2] : kRollToFrequency) {
          int64_t sub_count2 = sub_count1 * freq2;
          State p2_state = p1_state;
          p2_state.SetP2Roll(p2_roll);
          if (p2_state.s2 >= 21) {
            // Victory for p2, no longer need to track this branch.
            p2_wins += sub_count2;
            continue;
          }
          new_state_map[p2_state] += sub_count2;
        }
      }
    }
    state_map = std::move(new_state_map);
  }

  return IntReturn(std::max(p1_wins, p2_wins));
}

}  // namespace advent_of_code
