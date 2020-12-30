#include "advent_of_code/2016/day13/day13.h"

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

class SparseBoard {
 public:
  SparseBoard(int input) : input_(input) {}

  bool IsWall(Point p) const {
    int64_t discriminant =
        p.x * p.x + 3 * p.x + 2 * p.x * p.y + p.y + p.y * p.y + input_;
    int on_bits = 0;
    for (int64_t bv = 0; (1 << bv) <= discriminant; ++bv) {
      if (discriminant & (1 << bv)) ++on_bits;
    }
    return on_bits % 2;
  }

  bool OnBoard(Point p) const { return p.x >= 0 && p.y >= 0; }

 private:
  int input_;
  // absl::flat_hash_map<Point, bool> known_wall_;
};

class PathWalkToEnd : public BFSInterface<PathWalkToEnd, Point> {
 public:
  PathWalkToEnd(const SparseBoard& board, Point start, Point end)
      : board_(board), cur_(start), end_(end) {}

  void AddNextSteps(State* state) override {
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = dir + cur_;
      if (board_.OnBoard(next_point) && !board_.IsWall(next_point)) {
        PathWalkToEnd next = *this;
        next.cur_ = next_point;
        state->AddNextStep(std::move(next));
      }
    }
  }

  bool IsFinal() override { return cur_ == end_; }

  Point identifier() const override { return cur_; }

 private:
  const SparseBoard& board_;
  Point cur_;
  Point end_;
};

class PathWalkToDistance : public BFSInterface<PathWalkToDistance, Point> {
 public:
  PathWalkToDistance(const SparseBoard& board, Point start, int distance,
                     int* visited)
      : board_(board), cur_(start), distance_(distance), visited_(visited) {}

  void AddNextSteps(State* state) override {
    ++*visited_;
    if (num_steps() == distance_) {
      return;
    }
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = dir + cur_;
      if (board_.OnBoard(next_point) && !board_.IsWall(next_point)) {
        PathWalkToDistance next = *this;
        next.cur_ = next_point;
        state->AddNextStep(std::move(next));
      }
    }
  }

  Point identifier() const override { return cur_; }

  bool IsFinal() override { return false; }

 private:
  const SparseBoard& board_;
  Point cur_;
  int distance_;
  int* visited_;
};

}  // namespace

absl::StatusOr<std::vector<std::string>> Day13_2016::Part1(
    absl::Span<absl::string_view> input) const {
  SparseBoard test_board(10);
  absl::optional<int> test_dist =
      PathWalkToEnd(test_board, {1, 1}, {7, 4}).FindMinSteps();
  if (!test_dist) return Error("Can't find test dist");
  if (*test_dist != 11) return Error("Wrong test dist: ", *test_dist);

  if (input.size() != 1) return Error("Bad input size");
  int v;
  if (!absl::SimpleAtoi(input[0], &v)) return Error("Bad atoi: ", input[0]);
  SparseBoard board(v);
  return IntReturn(PathWalkToEnd(board, {1, 1}, {31, 39}).FindMinSteps());
}

absl::StatusOr<std::vector<std::string>> Day13_2016::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  int v;
  if (!absl::SimpleAtoi(input[0], &v)) return Error("Bad atoi: ", input[0]);
  SparseBoard board(v);
  int visited = 0;
  // PathWalkToDistance is built to always return 'not found'.
  (void)PathWalkToDistance(board, {1, 1}, 50, &visited).FindMinSteps();
  return IntReturn(visited);
}

}  // namespace advent_of_code
