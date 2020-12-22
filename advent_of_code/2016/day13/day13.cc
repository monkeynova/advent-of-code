#include "advent_of_code/2016/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class SparseBoard {
 public:
  SparseBoard(int input) : input_(input) {}

  bool IsWall(Point p) const {
    int64_t discriminant = p.x * p.x + 3 * p.x + 2 * p.x * p.y + p.y + p.y * p.y + input_;
    int on_bits = 0;
    for (int64_t bv = 0; (1<<bv) <= discriminant; ++bv) {
      if (discriminant & (1<<bv)) ++on_bits;
    }
    return on_bits % 2; 
  }

  bool OnBoard(Point p) const {
    return p.x >= 0 && p.y >= 0;
  }

  absl::optional<int> ShortestPath(Point start, Point end) {
    struct Path {
      int num_steps;
      Point cur;
    };
    std::deque<Path> frontier = {{.cur = start, .num_steps = 0}};
    absl::flat_hash_set<Point> hist;
    hist.insert(start);
    while (!frontier.empty()) {
      Path path = frontier.front();
      frontier.pop_front();
      for (Point dir : Cardinal::kFourDirs) {
        Point next = dir + path.cur;
        if (hist.contains(next)) continue;
        hist.insert(next);
        if (next == end) return path.num_steps + 1;
        if (OnBoard(next) && !IsWall(next)) {
          frontier.push_back({.cur = next, .num_steps = path.num_steps + 1});
        }
      }
    }
    return absl::nullopt;
  }
  
 private:
  int input_;
  // absl::flat_hash_map<Point, bool> known_wall_;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day13_2016::Part1(
    absl::Span<absl::string_view> input) const {
  SparseBoard test_board(10);
  absl::optional<int> test_dist = test_board.ShortestPath({1,1}, {7,4});
  if (!test_dist) return Error("Can't find test dist");
  if (*test_dist != 11) return Error("Wrong test dist: ", *test_dist);

  if (input.size() != 1) return Error("Bad input size");
  int v;
  if (!absl::SimpleAtoi(input[0], &v)) return Error("Bad atoi: ", input[0]);
  SparseBoard board(v);
  return IntReturn(board.ShortestPath({1, 1}, {31, 39}));
}

absl::StatusOr<std::vector<std::string>> Day13_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
