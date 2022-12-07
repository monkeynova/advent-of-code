// https://adventofcode.com/2018/day/9
//
// --- Day 9: Marble Mania ---
// ---------------------------
// 
// You talk to the Elves while you wait for your navigation system to
// initialize. To pass the time, they introduce you to their favorite
// marble game.
// 
// The Elves play this game by taking turns arranging the marbles in a
// circle according to very particular rules. The marbles are numbered
// starting with 0 and increasing by 1 until every marble has a number.
// 
// First, the marble numbered 0 is placed in the circle. At this point,
// while it contains only a single marble, it is still a circle: the
// marble is both clockwise from itself and counter-clockwise from
// itself. This marble is designated the current marble.
// 
// Then, each Elf takes a turn placing the lowest-numbered remaining
// marble into the circle between the marbles that are 1 and 2 marbles
// clockwise of the current marble. (When the circle is large enough,
// this means that there is one marble between the marble that was just
// placed and the current marble.) The marble that was just placed then
// becomes the current marble.
// 
// However, if the marble that is about to be placed has a number which
// is a multiple of 23, something entirely different happens. First, the
// current player keeps the marble they would have placed, adding it to
// their score. In addition, the marble 7 marbles counter-clockwise from
// the current marble is removed from the circle and also added to the
// current player's score. The marble located immediately clockwise of
// the marble that was removed becomes the new current marble.
// 
// For example, suppose there are 9 players. After the marble with value
// 0 is placed in the middle, each player (shown in square brackets)
// takes a turn. The result of each of those turns would produce circles
// of marbles like this, where clockwise is to the right and the
// resulting current marble is in parentheses:
// 
// [-]    (0)   [1]  0    (1)   [2]  0    (2)    1 
// [3]  0  2  1    (3)   [4]  0    (4)    2  1  3 
// [5]  0  4  2    (5)    1  3 
// [6]  0  4  2  5  1    (6)    3 
// [7]  0  4  2  5  1  6  3    (7)   [8]  0    (8)    4  2  5  1  6  3  7 
// [9]  0  8  4    (9)    2  5  1  6  3  7 
// [1]  0  8  4  9  2   (10)    5  1  6  3  7 
// [2]  0  8  4  9  2 10  5   (11)    1  6  3  7 
// [3]  0  8  4  9  2 10  5 11  1   (12)    6  3  7 
// [4]  0  8  4  9  2 10  5 11  1 12  6   (13)    3  7 
// [5]  0  8  4  9  2 10  5 11  1 12  6 13  3   (14)    7 
// [6]  0  8  4  9  2 10  5 11  1 12  6 13  3 14  7   (15)   [7]  0   (16)    8  4  9  2 10  5 11  1 12  6 13  3 14  7 15 
// [8]  0 16  8   (17)    4  9  2 10  5 11  1 12  6 13  3 14  7 15 
// [9]  0 16  8 17  4   (18)    9  2 10  5 11  1 12  6 13  3 14  7 15 
// [1]  0 16  8 17  4 18  9   (19)    2 10  5 11  1 12  6 13  3 14  7 15 
// [2]  0 16  8 17  4 18  9 19  2   (20)   10  5 11  1 12  6 13  3 14  7 15 
// [3]  0 16  8 17  4 18  9 19  2 20 10   (21)    5 11  1 12  6 13  3 14  7 15 
// [4]  0 16  8 17  4 18  9 19  2 20 10 21  5   (22)   11  1 12  6 13  3 14  7 15 
// [5]  0 16  8 17  4 18   (19)    2 20 10 21  5 22 11  1 12  6 13  3 14  7 15 
// [6]  0 16  8 17  4 18 19  2   (24)   20 10 21  5 22 11  1 12  6 13  3 14  7 15 
// [7]  0 16  8 17  4 18 19  2 24 20   (25)   10 21  5 22 11  1 12  6 13  3 14  7 15
// 
// The goal is to be the player with the highest score after the last
// marble is used up. Assuming the example above ends after the marble
// numbered 25, the winning score is 23+9=32 (because player 5 kept
// marble 23 and removed marble 9, while no other player got any points
// in this very short example game).
// 
// Here are a few more examples:
// 
// * 10 players; last marble is worth 1618 points: high score is 8317
// 
// * 13 players; last marble is worth 7999 points: high score is 146373
// 
// * 17 players; last marble is worth 1104 points: high score is 2764
// 
// * 21 players; last marble is worth 6111 points: high score is 54718
// 
// * 30 players; last marble is worth 5807 points: high score is 37305
// 
// What is the winning Elf's score?
//
// --- Part Two ---
// ----------------
// 
// Amused by the speed of your answer, the Elves are curious:
// 
// What would the new winning Elf's score be if the number of the last
// marble were 100 times larger?


#include "advent_of_code/2018/day09/day09.h"

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

struct Marble {
  int64_t score;
  int next;
  int prev;
};

struct GameState {
  int cur_position;
  std::vector<int64_t> scores;
  std::vector<Marble> marbles;

  void AddMarble(int64_t marble) {
    if (marble % 23 == 0) {
      scores[marble % scores.size()] += marble;
      for (int i = 0; i < 7; ++i) {
        cur_position = marbles[cur_position].prev;
      }
      marbles[marbles[cur_position].prev].next = marbles[cur_position].next;
      marbles[marbles[cur_position].next].prev = marbles[cur_position].prev;
      scores[marble % scores.size()] += marbles[cur_position].score;
      cur_position = marbles[cur_position].next;

    } else {
      int next = marbles[cur_position].next;
      int next_next = marbles[marbles[cur_position].next].next;
      marbles.push_back({marble, next_next, next});
      marbles[next].next = marbles.size() - 1;
      marbles[next_next].prev = marbles.size() - 1;
      cur_position = marbles.size() - 1;
    }
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const GameState& s) {
    std::string out;
    int marble_idx = 0;
    for (int j = 0; j < 30; ++j) {
      if (j > 0) absl::Format(&out, ",");
      if (marble_idx == s.cur_position) {
        absl::Format(&sink, "(%d)", s.marbles[marble_idx].score);
      } else {
        absl::Format(&sink, "%d", s.marbles[marble_idx].score);
      }
      if (s.marbles[s.marbles[marble_idx].next].prev != marble_idx) {
        LOG(ERROR) << "Integrity check!";
      }
      marble_idx = s.marbles[marble_idx].next;
      // Looped before size.
      if (marble_idx == 0) break;
    }
    if (marble_idx != 0) absl::Format(&sink, ",...");
  }

  friend std::ostream& operator<<(std::ostream& out, const GameState& s) {
    return out << absl::StreamFormat("%v", s);
  }

  void IntegrityCheck() {
    int marble_idx = cur_position;
    int start = marble_idx;
    int active = 0;
    do {
      VLOG(2) << marble_idx;
      ++active;
      CHECK_EQ(marble_idx, marbles[marbles[marble_idx].next].prev);
      CHECK_EQ(marble_idx, marbles[marbles[marble_idx].prev].next);
      marble_idx = marbles[marble_idx].next;
    } while (marble_idx != start);
    CHECK_EQ(active, marbles.size() - 2 * ((marbles.size() - 1) / 23));
  }
};

int64_t HighScore(int num_players, int num_marbles) {
  GameState state;
  state.cur_position = 0;
  state.scores = std::vector<int64_t>(num_players, 0);
  state.marbles = {{0, 0, 0}};
  for (int64_t i = 1; i <= num_marbles; ++i) {
    VLOG(2) << state;
    // state.IntegrityCheck();
    state.AddMarble(i);
  }
  int64_t max = 0;
  for (int64_t score : state.scores) {
    VLOG(1) << "Score: " << score;
    max = std::max(max, score);
  }
  return max;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_09::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  int players;
  int marbles;
  if (!RE2::FullMatch(input[0],
                      "(\\d+) players; last marble is worth (\\d+) points",
                      &players, &marbles)) {
    return Error("Bad input");
  }
  return IntReturn(HighScore(players, marbles));
}

absl::StatusOr<std::string> Day_2018_09::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  int players;
  int marbles;
  if (!RE2::FullMatch(input[0],
                      "(\\d+) players; last marble is worth (\\d+) points",
                      &players, &marbles)) {
    return Error("Bad input");
  }
  return IntReturn(HighScore(players, 100 * marbles));
}

}  // namespace advent_of_code
