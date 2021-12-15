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
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Route : public BFSInterface<Route, Point> {
 public:
  Route() = default;
  Route(const CharBoard* b) : b_(b), cur_(b_->range().min), end_(b_->range().max) {}

  Point identifier() const override { return cur_; }

  int min_steps_to_final() const override { return (end_ - cur_).dist(); }

  void AddNextSteps(State* state) override {
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur_ + d;
      if (!b_->OnBoard(n)) continue;
      Route next = *this;
      next.cur_ = n;
      next.add_steps((*b_)[n] - '0' - 1);
      state->AddNextStep(next);
    }
  }

  bool IsFinal() override {
    return cur_ == end_;
  }
  

 private:
  const CharBoard* b_;
  Point cur_;
  Point end_;
};

}  // namespace

absl::StatusOr<std::string> Day_2021_15::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  return IntReturn(Route(&*b).FindMinStepsAStar());
}

absl::StatusOr<std::string> Day_2021_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  CharBoard big_board(b->width() * 5, b->height() * 5);
  for (Point p : b->range()) {
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        Point p2 = p + i * Cardinal::kXHat * b->width() + j * Cardinal::kYHat * b->height();
        big_board[p2] = ((((*b)[p] + i + j) - '1') % 9) + '1';
      }
    }
  }

  return IntReturn(Route(&big_board).FindMinStepsAStar());
}

}  // namespace advent_of_code
