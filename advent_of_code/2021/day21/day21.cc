// https://adventofcode.com/2021/day/21
//
// --- Day 21: Dirac Dice ---
// --------------------------
// 
// There's not much to do as you slowly descend to the bottom of the
// ocean. The submarine computer challenges you to a nice game of Dirac
// Dice.
// 
// This game consists of a single die, two pawns, and a game board with a
// circular track containing ten spaces marked 1 through 10 clockwise.
// Each player's starting space is chosen randomly (your puzzle input).
// Player 1 goes first.
// 
// Players take turns moving. On each player's turn, the player rolls the
// die three times and adds up the results. Then, the player moves their
// pawn that many times forward around the track (that is, moving
// clockwise on spaces in order of increasing value, wrapping back around
// to 1 after 10). So, if a player is on space 7 and they roll 2, 2, and
// 1, they would move forward 5 times, to spaces 8, 9, 10, 1, and finally
// stopping on 2.
// 
// After each player moves, they increase their score by the value of the
// space their pawn stopped on. Players' scores start at 0. So, if the
// first player starts on space 7 and rolls a total of 5, they would stop
// on space 2 and add 2 to their score (for a total score of 2). The game
// immediately ends as a win for any player whose score reaches at least
// 1000.
// 
// Since the first game is a practice game, the submarine opens a
// compartment labeled deterministic dice and a 100-sided die falls out.
// This die always rolls 1 first, then 2, then 3, and so on up to 100,
// after which it starts over at 1 again. Play using this die.
// 
// For example, given these starting positions:
// 
// Player 1 starting position: 4
// Player 2 starting position: 8
// 
// This is how the game would go:
// 
// * Player 1 rolls 1+2+3 and moves to space 10 for a total score of 10.
// 
// * Player 2 rolls 4+5+6 and moves to space 3 for a total score of 3.
// 
// * Player 1 rolls 7+8+9 and moves to space 4 for a total score of 14.
// 
// * Player 2 rolls 10+11+12 and moves to space 6 for a total score of
// 9.
// 
// * Player 1 rolls 13+14+15 and moves to space 6 for a total score of
// 20.
// 
// * Player 2 rolls 16+17+18 and moves to space 7 for a total score of
// 16.
// 
// * Player 1 rolls 19+20+21 and moves to space 6 for a total score of
// 26.
// 
// * Player 2 rolls 22+23+24 and moves to space 6 for a total score of
// 22.
// 
// ...after many turns...
// 
// * Player 2 rolls 82+83+84 and moves to space 6 for a total score of
// 742.
// 
// * Player 1 rolls 85+86+87 and moves to space 4 for a total score of
// 990.
// 
// * Player 2 rolls 88+89+90 and moves to space 3 for a total score of
// 745.
// 
// * Player 1 rolls 91+92+93 and moves to space 10 for a final score,
// 1000.
// 
// Since player 1 has at least 1000 points, player 1 wins and the game
// ends. At this point, the losing player had 745 points and the die had
// been rolled a total of 993 times; 745 * 993 = 739785.
// 
// Play a practice game using the deterministic 100-sided die. The moment
// either player wins, what do you get if you multiply the score of the
// losing player by the number of times the die was rolled during the
// game?
//
// --- Part Two ---
// ----------------
// 
// Now that you're warmed up, it's time to play the real game.
// 
// A second compartment opens, this time labeled Dirac dice. Out of it
// falls a single three-sided die.
// 
// As you experiment with the die, you feel a little strange. An
// informational brochure in the compartment explains that this is a
// quantum die: when you roll it, the universe splits into multiple
// copies, one copy for each possible outcome of the die. In this case,
// rolling the die always splits the universe into three copies: one
// where the outcome of the roll was 1, one where it was 2, and one where
// it was 3.
// 
// The game is played the same as before, although to prevent things from
// getting too far out of hand, the game now ends when either player's
// score reaches at least 21.
// 
// Using the same starting positions as in the example above, player 1
// wins in 444356092776315 universes, while player 2 merely wins in
// 341960390180808 universes.
// 
// Using your given starting positions, determine every possible outcome.
// Find the player that wins in more universes; in how many universes
// does that player win?


// http://adventofcode.com/2021/day/21

#include "advent_of_code/2021/day21/day21.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
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
