#include "advent_of_code/2018/day09/day09.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/splice_ring.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class GameState {
 public:
  GameState(int num_players, int num_marbles)
   : scores_(num_players, 0) {
    marbles_.reserve(num_marbles);
  }

  const std::vector<int64_t>& scores() const { return scores_; }

  void AddMarble(int64_t marble, int player) {
    if (marbles_.empty()) {
      CHECK_EQ(marble, 0);
      cur_position_ = marbles_.InsertFirst(0);
      return;
    }
    if (marble % 23 == 0) {
      scores_[player] += marble;
      cur_position_ -= 7;
      scores_[player] += *cur_position_;
      cur_position_ = marbles_.Erase(cur_position_);

    } else {
      cur_position_ += 2;
      marbles_.InsertBefore(cur_position_, marble);
      --cur_position_;
    }
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const GameState& s) {
    AbslStringify(sink, s.marbles_, /*limit=*/30);
  }

 private:
  std::vector<int64_t> scores_;
  SpliceRing<int> marbles_;
  SpliceRing<int>::const_iterator cur_position_;
};

int64_t HighScore(int num_players, int num_marbles) {
  GameState state(num_players, num_marbles);
  for (int64_t i = 0; i <= num_marbles; ++i) {
    VLOG(2) << state;
    state.AddMarble(i, i % num_players);
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
