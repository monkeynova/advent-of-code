#include "advent_of_code/2018/day09/day09.h"

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
  return AdventReturn(HighScore(players, marbles));
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
  return AdventReturn(HighScore(players, 100 * marbles));
}

}  // namespace advent_of_code
