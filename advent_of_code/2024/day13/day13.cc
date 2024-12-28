// http://adventofcode.com/2024/day/13

#include "advent_of_code/2024/day13/day13.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class PrizeWalk : public BFSInterface<PrizeWalk, Point> {
 public:
  PrizeWalk(Point a, Point b, Point prize)
      : a_(a), b_(b), prize_(prize) {}

  Point identifier() const override { return cur_; }
  bool IsFinal() const override { return cur_ == prize_; }
  void AddNextSteps(State* state) const override {
    Point t = cur_ + a_;
    if (t.x <= prize_.x && t.y <= prize_.y) {
      PrizeWalk next = *this;
      next.cur_ = t;
      next.add_steps(2);
      state->AddNextStep(next);
    }
    t = cur_ + b_;
    if (t.x <= prize_.x && t.y <= prize_.y) {
      PrizeWalk next = *this;
      next.cur_ = t;
      next.cost_ += 1;
      state->AddNextStep(next);
    }
  }

 private:
  int cost_ = 0;
  Point cur_ = {0, 0};
  Point a_;
  Point b_;
  Point prize_;
};

std::optional<int64_t> FindCost(Point a, Point b, Point prize) {
  // a_c * a_x + b_c * b_x = p_x
  // a_c * a_y + b_c * b_y = p_y

  // bc * (bx * ay - by * ax) = px * ay - py * ax
  // bc = px * ay - py * ax / (bx * ay - by * ax)
  
  // ac * (ax*by - ay *bx) = px * by - py * bx
  // ac = px * by - py * bx / (ax*by - ay *bx)

  int64_t b_c = prize.x * a.y - prize.y * a.x;
  int64_t b_c_d = b.x * a.y - b.y * a.x;
  if (b_c % b_c_d != 0) return std::nullopt;
  b_c /= b_c_d;
  if (b_c < 0) return std::nullopt;

  int64_t a_c = prize.x * b.y - prize.y * b.x;
  int64_t a_c_d = a.x * b.y - a.y * b.x;
  if (a_c % a_c_d != 0) return std::nullopt;
  a_c /= a_c_d;
  if (a_c < 0) return std::nullopt;

  return 3 * a_c + b_c;
}

std::optional<int64_t> FindCost2(Point a, Point b, Point prize) {
  const int64_t kDelta = 10000000000000;

  int64_t b_c = (prize.x + kDelta) * a.y - (prize.y + kDelta) * a.x;
  int64_t b_c_d = b.x * a.y - b.y * a.x;
  if (b_c % b_c_d != 0) return std::nullopt;
  b_c /= b_c_d;
  if (b_c < 0) return std::nullopt;

  int64_t a_c = (prize.x + kDelta) * b.y - (prize.y + kDelta) * b.x;
  int64_t a_c_d = a.x * b.y - a.y * b.x;
  if (a_c % a_c_d != 0) return std::nullopt;
  a_c /= a_c_d;
  if (a_c < 0) return std::nullopt;

  return 3 * a_c + b_c;
}


}  // namespace

absl::StatusOr<std::string> Day_2024_13::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() % 4 != 3) return absl::InvalidArgumentError("% 4");

  int64_t total_cost = 0;
  for (int i = 0; i < input.size(); i += 4) {
    Point a;
    Point b;
    Point prize;
    if (!RE2::FullMatch(input[i], "Button A: X\\+(\\d+), Y\\+(\\d+)", &a.x, &a.y)) {
      return absl::InvalidArgumentError("parse a");
    }
    if (!RE2::FullMatch(input[i + 1], "Button B: X\\+(\\d+), Y\\+(\\d+)", &b.x, &b.y)) {
      return absl::InvalidArgumentError("parse b");
    }
    if (!RE2::FullMatch(input[i + 2], "Prize: X=(\\d+), Y=(\\d+)", &prize.x, &prize.y)) {
      return absl::InvalidArgumentError("parse prize");
    }
    std::optional<int64_t> cost = FindCost(a, b, prize); //PrizeWalk(a, b, prize).FindMinStepsAStar();
    if (cost) total_cost += *cost;
  }
  return AdventReturn(total_cost);
}

absl::StatusOr<std::string> Day_2024_13::Part2(
    absl::Span<std::string_view> input) const {
  int64_t total_cost = 0;
  for (int i = 0; i < input.size(); i += 4) {
    Point a;
    Point b;
    Point prize;
    if (!RE2::FullMatch(input[i], "Button A: X\\+(\\d+), Y\\+(\\d+)", &a.x, &a.y)) {
      return absl::InvalidArgumentError("parse a");
    }
    if (!RE2::FullMatch(input[i + 1], "Button B: X\\+(\\d+), Y\\+(\\d+)", &b.x, &b.y)) {
      return absl::InvalidArgumentError("parse b");
    }
    if (!RE2::FullMatch(input[i + 2], "Prize: X=(\\d+), Y=(\\d+)", &prize.x, &prize.y)) {
      return absl::InvalidArgumentError("parse prize");
    }
    std::optional<int64_t> cost = FindCost2(a, b, prize);
    if (cost) total_cost += *cost;
  }
  return AdventReturn(total_cost);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_13()); });

}  // namespace advent_of_code
