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
  bool operator==(const State& o) const {
    return p1 == o.p1 && p2 == o.p2 && s1 == o.s1 && s2 == o.s2;
  }
  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.p1, s.p2, s.s1, s.s2);
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
  --p1;
  --p2;
  int score_p1 = 0;
  int score_p2 = 0;
  int die = 0;
  for (int turn = 1; true; turn += 2) {
    int delta1 = 0;
    delta1 += ++die;
    if (die == 100) die = 0;
    delta1 += ++die;
    if (die == 100) die = 0;
    delta1 += ++die;
    if (die == 100) die = 0;
    p1 = (p1 + delta1) % 10;
    VLOG(2) << "p1 on " << p1 + 1;
    score_p1 += p1 + 1;
    VLOG(2) << "p1 score " << score_p1;
    if (score_p1 >= 1000) {
      return IntReturn(3 * turn * score_p2);
    }
    int delta2 = 0;
    delta2 += ++die;
    if (die == 100) die = 0;
    delta2 += ++die;
    if (die == 100) die = 0;
    delta2 += ++die;
    if (die == 100) die = 0;
    p2 = (p2 + delta2) % 10;
    VLOG(2) << "p2 on " << p2 + 1;
    score_p2 += p2 + 1;
    VLOG(2) << "p2 score " << score_p2;
    if (score_p2 >= 1000) {
      return IntReturn(3 * (turn + 1) * score_p1);
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
  absl::flat_hash_map<State, int64_t> state_map = {
    {{p1 - 1, p2 - 1, 0, 0}, 1},
  };
  int64_t p1_wins = 0;
  int64_t p2_wins = 0;
  absl::flat_hash_map<int, int> rolls;
  for (int i = 1; i <= 3; i++) {
    for (int j = 1; j <= 3; j++) {
      for (int k = 1; k <= 3; k++) {
        ++rolls[i + j + k];
      }
    }
  }
  while (!state_map.empty()) {
    absl::flat_hash_map<State, int64_t> new_state_map;
    VLOG(2) << "new turn";
    for (const auto& [state, count] : state_map) {
      VLOG(2) << "1 @" << state.p1 << " s=" << state.s1 << "; " << "2 @" << state.p2 << " s=" << state.s2;
      for (const auto& [p1_roll, freq1] : rolls) {
        State p1_state = state;
        p1_state.p1 = (p1_state.p1 + p1_roll) % 10;
        p1_state.s1 += p1_state.p1 + 1;
        if (p1_state.s1 >= 21) {
          p1_wins += count * freq1;
        } else {
          int64_t sub_count = count * freq1;
          for (const auto& [p2_roll, freq2] : rolls) {
            State p2_state = p1_state;
            p2_state.p2 = (p2_state.p2 + p2_roll) % 10;
            p2_state.s2 += p2_state.p2 + 1;
            if (p2_state.s2 >= 21) {
              p2_wins += sub_count * freq2;
            } else {
              new_state_map[p2_state] += sub_count * freq2;
            }
          }
        }
      }
    }
    state_map = std::move(new_state_map);
  }
  

  return IntReturn(std::max(p1_wins, p2_wins));
}

}  // namespace advent_of_code
