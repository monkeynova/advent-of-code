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

class GameState {
 public:
  GameState(int num_players)
   : scores_(num_players, 0) {}

  const std::vector<int64_t>& scores() const { return scores_; }

  void AddMarble(int64_t marble) {
    if (marbles_.empty()) {
      CHECK_EQ(marble, 0);
      marbles_.push_back({0, 0, 0});
    }
    if (marble % 23 == 0) {
      scores_[marble % scores_.size()] += marble;
      for (int i = 0; i < 7; ++i) {
        cur_position_ = marbles_[cur_position_].prev;
      }
      marbles_[marbles_[cur_position_].prev].next = marbles_[cur_position_].next;
      marbles_[marbles_[cur_position_].next].prev = marbles_[cur_position_].prev;
      scores_[marble % scores_.size()] += marbles_[cur_position_].score;
      cur_position_ = marbles_[cur_position_].next;

    } else {
      int next = marbles_[cur_position_].next;
      int next_next = marbles_[marbles_[cur_position_].next].next;
      marbles_.push_back({marble, next_next, next});
      marbles_[next].next = marbles_.size() - 1;
      marbles_[next_next].prev = marbles_.size() - 1;
      cur_position_ = marbles_.size() - 1;
    }
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const GameState& s) {
    std::string out;
    int marble_idx = 0;
    for (int j = 0; j < 30; ++j) {
      if (j > 0) absl::Format(&out, ",");
      if (marble_idx == s.cur_position_) {
        absl::Format(&sink, "(%d)", s.marbles_[marble_idx].score);
      } else {
        absl::Format(&sink, "%d", s.marbles_[marble_idx].score);
      }
      if (s.marbles_[s.marbles_[marble_idx].next].prev != marble_idx) {
        LOG(ERROR) << "Integrity check!";
      }
      marble_idx = s.marbles_[marble_idx].next;
      // Looped before size.
      if (marble_idx == 0) break;
    }
    if (marble_idx != 0) absl::Format(&sink, ",...");
  }

 private:
  struct Marble {
    int64_t score;
    int next;
    int prev;
  };

  std::vector<int64_t> scores_;
  std::vector<Marble> marbles_;
  int cur_position_ = 0;
};

int64_t HighScore(int num_players, int num_marbles) {
  GameState state(num_players);
  for (int64_t i = 0; i <= num_marbles; ++i) {
    VLOG(2) << state;
    state.AddMarble(i);
  }
  int64_t max = 0;
  for (int64_t score : state.scores()) {
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
