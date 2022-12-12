// http://adventofcode.com/2022/day/12

#include "advent_of_code/2022/day12/day12.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class HeightBfs : public BFSInterface<HeightBfs, Point> {
 public:
  HeightBfs(const CharBoard& b, Point c) : b_(b), cur_(c) {}

  bool CanStep(Point from, Point to) const {
    if (b_[from] == 'S') return true;
    if (b_[to] == 'E') return b_[from] >= 'y';
    return b_[to] <= b_[from] + 1;
  }

  Point identifier() const { return cur_; }
  void AddNextSteps(State* state) const {
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur_ + d;
      if (b_.OnBoard(n) && CanStep(cur_, n)) {
        HeightBfs next = *this;
        next.cur_ = n;
        state->AddNextStep(next);
      }
    }
  }
  bool IsFinal() const {
    return b_[cur_] == 'E';
  }

 private:
  const CharBoard& b_;
  Point cur_;
};

class ReverseHeightBfs : public BFSInterface<ReverseHeightBfs, Point> {
 public:
  ReverseHeightBfs(const CharBoard& b, Point c) : b_(b), cur_(c) {}

  bool CanStep(Point from, Point to) const {
    if (b_[from] == 'E') return b_[to] >= 'y';
    return b_[to] >= b_[from] - 1;
  }

  Point identifier() const { return cur_; }
  void AddNextSteps(State* state) const {
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur_ + d;
      if (b_.OnBoard(n) && CanStep(cur_, n)) {
        ReverseHeightBfs next = *this;
        next.cur_ = n;
        state->AddNextStep(next);
      }
    }
  }
  bool IsFinal() const {
    return b_[cur_] == 'a';
  }

 private:
  const CharBoard& b_;
  Point cur_;
};

}  // namespace

absl::StatusOr<std::string> Day_2022_12::Part1(
    absl::Span<absl::string_view> input) const {
  auto board = ParseAsBoard(input); 
  if (!board.ok()) return board.status();
  Point start;
  for (Point p : board->range()) {
    if ((*board)[p] == 'S') start = p;
  }
  VLOG(1) << "Start: " << start;

  return IntReturn(HeightBfs(*board, start).FindMinSteps());
}

absl::StatusOr<std::string> Day_2022_12::Part2(
    absl::Span<absl::string_view> input) const {
  auto board = ParseAsBoard(input); 
  if (!board.ok()) return board.status();
  Point start;
  for (Point p : board->range()) {
    if ((*board)[p] == 'E') start = p;
  }
  VLOG(1) << "Start: " << start;

  return IntReturn(ReverseHeightBfs(*board, start).FindMinSteps());
}

}  // namespace advent_of_code
