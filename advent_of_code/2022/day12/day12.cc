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
  HeightBfs(const CharBoard& b, Point c, bool reverse)
   : b_(b), cur_(c), reverse_(reverse) {}

  Point identifier() const override { return cur_; }
  void AddNextSteps(State* state) const override {
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur_ + d;
      if (b_.OnBoard(n) && CanStep(cur_, n)) {
        HeightBfs next = *this;
        next.cur_ = n;
        state->AddNextStep(next);
      }
    }
  }
  bool IsFinal() const override {
    return b_[cur_] == (reverse_ ? 'a' : 'E');
  }

  bool CanStep(Point from, Point to) const {
    if (reverse_) {
      std::swap(from, to);
    }
    if (b_[from] == 'S') return true;
    if (b_[to] == 'E') return b_[from] >= 'y';
    return b_[to] <= b_[from] + 1;
  }

 private:
  const CharBoard& b_;
  Point cur_;
  bool reverse_;
};

}  // namespace

absl::StatusOr<std::string> Day_2022_12::Part1(
    absl::Span<absl::string_view> input) const {
  auto board = ParseAsBoard(input); 
  if (!board.ok()) return board.status();
  absl::flat_hash_set<Point> starts = board->Find('S');
  if (starts.size() != 1) return Error("Bad start");

  HeightBfs search(*board, *starts.begin(), /*reverse=*/false);
  return IntReturn(search.FindMinSteps());
}

absl::StatusOr<std::string> Day_2022_12::Part2(
    absl::Span<absl::string_view> input) const {
  auto board = ParseAsBoard(input); 
  if (!board.ok()) return board.status();
  absl::flat_hash_set<Point> starts = board->Find('E');
  if (starts.size() != 1) return Error("Bad start");

  HeightBfs search(*board, *starts.begin(), /*reverse=*/true);
  return IntReturn(search.FindMinSteps());
}

}  // namespace advent_of_code
