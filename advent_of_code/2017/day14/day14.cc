#include "advent_of_code/2017/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/knot_hash.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<CharBoard> BuildBoard(std::string_view input) {
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
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  ASSIGN_OR_RETURN(CharBoard board, BuildBoard(input[0]));
  VLOG(1) << "Board:\n" << board;
  return AdventReturn(board.CountChar('.'));
}

absl::StatusOr<std::string> Day_2017_14::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  ASSIGN_OR_RETURN(CharBoard board, BuildBoard(input[0]));

  absl::flat_hash_map<Point, std::string> storage;
  for (auto [p, c] : board) {
    if (c == '.') storage[p] = absl::StrCat(p);
  }
  DirectedGraph<bool> dg;
  for (auto [p, c] : board) {
    if (c != '.') continue;
    dg.AddNode(storage[p], true);
    for (Point d : Cardinal::kFourDirs) {
      if (board[p + d] == '.') {
        dg.AddEdge(storage[p], storage[p + d]);
      }
    }
  }
  return AdventReturn(dg.Forest().size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/14,
    []() { return std::unique_ptr<AdventDay>(new Day_2017_14()); });

}  // namespace advent_of_code
