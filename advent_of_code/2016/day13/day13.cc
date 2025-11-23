#include "advent_of_code/2016/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/numeric/bits.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point_walk.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class SparseBoard {
 public:
  SparseBoard(int input) : input_(input) {}

  bool IsWall(Point p) const {
    uint64_t discriminant =
        p.x * p.x + 3 * p.x + 2 * p.x * p.y + p.y + p.y * p.y + input_;
    return absl::popcount(discriminant) % 2;
  }

  bool OnBoard(Point p) const { return p.x >= 0 && p.y >= 0; }

 private:
  int input_;
};

}  // namespace

absl::StatusOr<std::string> Day_2016_13::Part1(
    absl::Span<std::string_view> input) const {
  if (run_audit()) {
    SparseBoard test_board(10);
    std::optional<int> test_dist =
        PointWalk({
                      .start = {1, 1},
                      .is_good =
                          [&](Point test, int) {
                            return test_board.OnBoard(test) &&
                                   !test_board.IsWall(test);
                          },
                      .is_final = [&](Point test,
                                      int) { return test == Point{7, 4}; },
                  })
            .FindMinSteps();
    if (!test_dist) return Error("Can't find test dist");
    if (*test_dist != 11) return Error("Wrong test dist: ", *test_dist);
  }

  if (input.size() != 1) return Error("Bad input size");
  int v;
  if (!absl::SimpleAtoi(input[0], &v)) return Error("Bad atoi: ", input[0]);
  SparseBoard board(v);
  return AdventReturn(
      PointWalk({
                    .start = {1, 1},
                    .is_good =
                        [&](Point test, int) {
                          return board.OnBoard(test) && !board.IsWall(test);
                        },
                    .is_final = [&](Point test,
                                    int) { return test == Point{31, 39}; },
                })
          .FindMinSteps());
}

absl::StatusOr<std::string> Day_2016_13::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  int v;
  if (!absl::SimpleAtoi(input[0], &v)) return Error("Bad atoi: ", input[0]);
  SparseBoard board(v);
  return AdventReturn(
      PointWalk({.start = {1, 1},
                 .is_good =
                     [&](Point test, int num_steps) {
                       return num_steps <= 50 && board.OnBoard(test) &&
                              !board.IsWall(test);
                     },
                 .is_final = [&](Point test, int) { return false; }})
          .FindReachable()
          .size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_13()); });

}  // namespace advent_of_code
