#include "advent_of_code/2018/day09/day09.h"

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

struct Marble {
  int score;
  int next;
  int prev;
};

struct GameState {
  int cur_position;
  std::vector<int> scores;
  std::vector<Marble> marbles;

  void AddMarble(int marble) {
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

int HighScore(int num_players, int num_marbles) {
  GameState state;
  state.cur_position = 0;
  state.scores = std::vector<int>(num_players, 0);
  state.marbles = {{0, 0, 0}};
  for (int i = 1; i <= num_marbles; ++i) {
    if (VLOG_IS_ON(1)) {
      std::string out;
      int marble_idx = 0;
      for (int j = 0; j < 30; ++j) {
        if (j > 0) absl::StrAppend(&out, ",");
        if (marble_idx == state.cur_position) {
          absl::StrAppend(&out, "(", state.marbles[marble_idx].score, ")");
        } else {
          absl::StrAppend(&out, state.marbles[marble_idx].score);
        }
        if (state.marbles[state.marbles[marble_idx].next].prev != marble_idx) {
          LOG(ERROR) << "Integrity check!";
        }
        marble_idx = state.marbles[marble_idx].next;
        // Looped before size.
        if (marble_idx == 0) break;
      }
      VLOG(1) << out;
    }
    // state.IntegrityCheck();
    state.AddMarble(i);
  }
  int max = 0;
  for (int score : state.scores) max = std::max(max, score);
  return max;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day09_2018::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  int players;
  int marbles;
  if (!RE2::FullMatch(input[0], "(\\d+) players; last marble is worth (\\d+) points", &players, &marbles)) {
    return Error("Bad input");
  }
  return IntReturn(HighScore(players, marbles));
}

absl::StatusOr<std::vector<std::string>> Day09_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
