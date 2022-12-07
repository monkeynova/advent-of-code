// https://adventofcode.com/2016/day/13
//
// --- Day 13: A Maze of Twisty Little Cubicles ---
// ------------------------------------------------
// 
// You arrive at the first floor of this new building to discover a much
// less welcoming environment than the shiny atrium of the last one.
// Instead, you are in a maze of twisty little cubicles, all alike.
// 
// Every location in this area is addressed by a pair of non-negative
// integers (x,y). Each such coordinate is either a wall or an open
// space. You can't move diagonally. The cube maze starts at 0,0 and
// seems to extend infinitely toward positive x and y; negative values
// are invalid, as they represent a location outside the building. You
// are in a small waiting area at 1,1.
// 
// While it seems chaotic, a nearby morale-boosting poster explains, the
// layout is actually quite logical. You can determine whether a given
// x,y coordinate will be a wall or an open space using a simple system:
// 
// * Find x*x + 3*x + 2*x*y + y + y*y.
// 
// * Add the office designer's favorite number (your puzzle input).
// 
// * Find the binary representation of that sum; count the number of
// bits that are 1.
// 
// * If the number of bits that are 1 is even, it's an open space.
// 
// * If the number of bits that are 1 is odd, it's a wall.
// 
// For example, if the office designer's favorite number were 10, drawing
// walls as # and open spaces as ., the corner of the building containing
// 0,0 would look like this:
// 
// 0123456789
// 0 .#.####.##
// 1 ..#..#...#
// 2 #....##...
// 3 ###.#.###.
// 4 .##..#..#.
// 5 ..##....#.
// 6 #...##.###
// 
// Now, suppose you wanted to reach 7,4. The shortest route you could
// take is marked as O:
// 
// 0123456789
// 0 .#.####.##
// 1 .O#..#...#
// 2 #OOO.##...
// 3 ###O#.###.
// 4 .##OO#OO#.
// 5 ..##OOO.#.
// 6 #...##.###
// 
// Thus, reaching 7,4 would take a minimum of 11 steps (starting from
// your current location, 1,1).
// 
// What is the fewest number of steps required for you to reach 31,39?
//
// --- Part Two ---
// ----------------
// 
// How many locations (distinct x,y coordinates, including your starting
// location) can you reach in at most 50 steps?


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
#include "absl/log/log.h"
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

  void AddNextSteps(State* state) const override {
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = dir + cur_;
      if (board_.OnBoard(next_point) && !board_.IsWall(next_point)) {
        PathWalkToEnd next = *this;
        next.cur_ = next_point;
        state->AddNextStep(std::move(next));
      }
    }
  }

  bool IsFinal() const override { return cur_ == end_; }

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

  void AddNextSteps(State* state) const override {
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

  bool IsFinal() const override { return false; }

 private:
  const SparseBoard& board_;
  Point cur_;
  int distance_;
  int* visited_;
};

}  // namespace

absl::StatusOr<std::string> Day_2016_13::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    SparseBoard test_board(10);
    absl::optional<int> test_dist =
        PathWalkToEnd(test_board, {1, 1}, {7, 4}).FindMinSteps();
    if (!test_dist) return Error("Can't find test dist");
    if (*test_dist != 11) return Error("Wrong test dist: ", *test_dist);
  }

  if (input.size() != 1) return Error("Bad input size");
  int v;
  if (!absl::SimpleAtoi(input[0], &v)) return Error("Bad atoi: ", input[0]);
  SparseBoard board(v);
  return IntReturn(PathWalkToEnd(board, {1, 1}, {31, 39}).FindMinSteps());
}

absl::StatusOr<std::string> Day_2016_13::Part2(
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
