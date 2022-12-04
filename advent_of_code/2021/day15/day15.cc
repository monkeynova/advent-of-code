// http://adventofcode.com/2021/day/15

#include "advent_of_code/2021/day15/day15.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Route : public BFSInterface<Route, Point> {
 public:
  Route(const CharBoard& b)
      : b_(&b), cur_(b_->range().min), end_(b_->range().max) {}

  Point identifier() const override { return cur_; }

  int min_steps_to_final() const override { return (end_ - cur_).dist(); }

  void AddNextSteps(State* state) const override {
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur_ + d;
      if (!b_->OnBoard(n)) continue;
      Route next = *this;
      next.cur_ = n;
      // Cost of moving to the new point is the value on the board.
      // But we need to subtract the one that gets added for the move by
      // BFSInterface itself
      next.add_steps((*b_)[n] - '0' - 1);
      state->AddNextStep(std::move(next));
    }
  }

  bool IsFinal() const override { return cur_ == end_; }

 private:
  const CharBoard* b_;
  Point cur_;
  Point end_;
};

}  // namespace

absl::StatusOr<std::string> Day_2021_15::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  return IntReturn(Route(*b).FindMinStepsAStar());
}

absl::StatusOr<std::string> Day_2021_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  CharBoard big_board(b->width() * 5, b->height() * 5);
  for (Point p : b->range()) {
    int base = (*b)[p] - '1';
    for (int i = 0; i < 5; i++) {
      Point dx = i * Cardinal::kXHat * b->width();
      for (int j = 0; j < 5; j++) {
        Point dy = j * Cardinal::kYHat * b->height();
        big_board[p + dx + dy] = ((base + i + j) % 9) + '1';
      }
    }
  }

  return IntReturn(Route(big_board).FindMinStepsAStar());
}

}  // namespace advent_of_code
