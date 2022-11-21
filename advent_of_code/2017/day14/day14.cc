// http://adventofcode.com/2017/day/14

#include "advent_of_code/2017/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/knot_hash.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class PathWalk : public BFSInterface<PathWalk, Point> {
 public:
  explicit PathWalk(const CharBoard& board) : board_(board) {}

  int CountGroups() {
    absl::flat_hash_set<Point> to_find;
    for (Point p : board_.range()) {
      if (board_[p] == '.') to_find.insert(p);
    }
    to_find_ = &to_find;
    int groups = 0;
    while (!to_find.empty()) {
      ++groups;
      cur_ = *to_find.begin();
      FindMinSteps();
    }
    return groups;
  }

  Point identifier() const override { return cur_; }

  bool IsFinal() const override { return false; }

  void AddNextSteps(State* state) const override {
    to_find_->erase(cur_);
    for (Point dir : Cardinal::kFourDirs) {
      Point next_cur = cur_ + dir;
      if (!board_.OnBoard(next_cur)) continue;
      if (board_[next_cur] == '#') continue;

      PathWalk next = *this;
      next.cur_ = next_cur;
      state->AddNextStep(next);
    }
  }

 private:
  const CharBoard& board_;
  Point cur_;
  absl::flat_hash_set<Point>* to_find_ = nullptr;
};

absl::StatusOr<CharBoard> BuildBoard(absl::string_view input) {
  CharBoard board(128, 128);
  for (Point p : board.range()) board[p] = '#';
  KnotHash digest;
  for (int y = 0; y < 128; ++y) {
    std::string hash = digest.DigestHex(absl::StrCat(input, "-", y));
    for (int i = 0; i < hash.size(); ++i) {
      char c = hash[i];
      if (c >= 'a' && c <= 'f')
        c -= 'a' - 10;
      else if (c >= '0' && c <= '9')
        c -= '0';
      else
        return Error("Bad hash char: ", hash);
      if (c & 8) board[{i * 4 + 0, y}] = '.';
      if (c & 4) board[{i * 4 + 1, y}] = '.';
      if (c & 2) board[{i * 4 + 2, y}] = '.';
      if (c & 1) board[{i * 4 + 3, y}] = '.';
    }
  }
  return board;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_14::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  absl::StatusOr<CharBoard> board = BuildBoard(input[0]);
  if (!board.ok()) return board.status();
  VLOG(1) << "Board:\n" << *board;
  return IntReturn(board->CountChar('.'));
}

absl::StatusOr<std::string> Day_2017_14::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  absl::StatusOr<CharBoard> board = BuildBoard(input[0]);
  if (!board.ok()) return board.status();
  return IntReturn(PathWalk(*board).CountGroups());
}

}  // namespace advent_of_code
