#include "advent_of_code/2017/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/knot_hash.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day14_2017::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  CharBoard board(128, 128);
  for (Point p : board.range()) board[p] = '.';
  for (int y = 0; y < 128; ++y) {
    std::string hash = KnotHash(absl::StrCat(input[0], "-", y));
    for (int i = 0; i < hash.size(); ++i) {
      char c = hash[i];
      if (c >= 'a' && c <= 'f') c -= 'a' - 10;
      else if (c >= '0' && c <= '9') c -= '0';
      else return Error("Bad hash char: ", hash);
      if (c & 8) board[{i * 4 + 0, y}] = '#';
      if (c & 4) board[{i * 4 + 1, y}] = '#';
      if (c & 2) board[{i * 4 + 2, y}] = '#';
      if (c & 1) board[{i * 4 + 3, y}] = '#';
    }
  }
  VLOG(1) << "Board:\n" << board.DebugString();
  int count = 0;
  for (Point p : board.range()) if (board[p] == '#') ++count;
  return IntReturn(count);
}

absl::StatusOr<std::vector<std::string>> Day14_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
