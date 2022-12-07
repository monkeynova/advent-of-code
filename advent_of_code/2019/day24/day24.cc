// https://adventofcode.com/2019/day/24
//
// --- Day 24: Planet of Discord ---
// ---------------------------------
//
// You land on Eris, your last stop before reaching Santa. As soon as you
// do, your sensors start picking up strange life forms moving around:
// Eris is infested with bugs! With an over 24-hour roundtrip for
// messages between you and Earth, you'll have to deal with this problem
// on your own.
//
// Eris isn't a very large place; a scan of the entire area fits into a
// 5x5 grid (your puzzle input). The scan shows bugs (#) and empty spaces
// (.).
//
// Each minute, The bugs live and die based on the number of bugs in the
// four adjacent tiles:
//
// * A bug dies (becoming an empty space) unless there is exactly one
// bug adjacent to it.
//
// * An empty space becomes infested with a bug if exactly one or two
// bugs are adjacent to it.
//
// Otherwise, a bug or empty space remains the same. (Tiles on the edges
// of the grid have fewer than four adjacent tiles; the missing tiles
// count as empty space.) This process happens in every location
// simultaneously; that is, within the same minute, the number of
// adjacent bugs is counted for every tile first, and then the tiles are
// updated.
//
// Here are the first few minutes of an example scenario:
//
// Initial state:
// ....#
// #..#.
// #..##
// ..#..
// #....
//
// After 1 minute:
// #..#.
// ####.
// ###.#
// ##.##
// .##..
//
// After 2 minutes:
// #####
// ....#
// ....#
// ...#.
// #.###
//
// After 3 minutes:
// #....
// ####.
// ...##
// #.##.
// .##.#
//
// After 4 minutes:
// ####.
// ....#
// ##..#
// .....
// ##...
//
// To understand the nature of the bugs, watch for the first time a
// layout of bugs and empty spaces matches any previous layout. In the
// example above, the first layout to appear twice is:
//
// .....
// .....
// .....
// #....
// .#...
//
// To calculate the biodiversity rating for this layout, consider each
// tile left-to-right in the top row, then left-to-right in the second
// row, and so on. Each of these tiles is worth biodiversity points equal
// to increasing powers of two: 1, 2, 4, 8, 16, 32, and so on. Add up the
// biodiversity points for tiles with bugs; in this example, the 16th
// tile (32768 points) and 22nd tile (2097152 points) have bugs, a total
// biodiversity rating of 2129920.
//
// What is the biodiversity rating for the first layout that appears
// twice?
//
// --- Part Two ---
// ----------------
//
// After careful analysis, one thing is certain: you have no idea where
// all these bugs are coming from.
//
// Then, you remember: Eris is an old Plutonian settlement! Clearly, the
// bugs are coming from recursively-folded space.
//
// This 5x5 grid is only one level in an infinite number of recursion
// levels. The tile in the middle of the grid is actually another 5x5
// grid, the grid in your scan is contained as the middle tile of a
// larger 5x5 grid, and so on. Two levels of grids look like this:
//
// |     |         |     |
// |     |         |     |
// |     |         |     |
// -----+-----+---------+-----+-----
// |     |         |     |
// |     |         |     |
// |     |         |     |
// -----+-----+---------+-----+-----
// |     | | | | | |     |
// |     |-+-+-+-+-|     |
// |     | | | | | |     |
// |     |-+-+-+-+-|     |
// |     | | |?| | |     |
// |     |-+-+-+-+-|     |
// |     | | | | | |     |
// |     |-+-+-+-+-|     |
// |     | | | | | |     |
// -----+-----+---------+-----+-----
// |     |         |     |
// |     |         |     |
// |     |         |     |
// -----+-----+---------+-----+-----
// |     |         |     |
// |     |         |     |
// |     |         |     |
//
// (To save space, some of the tiles are not drawn to scale.) Remember,
// this is only a small part of the infinitely recursive grid; there is a
// 5x5 grid that contains this diagram, and a 5x5 grid that contains that
// one, and so on. Also, the ? in the diagram contains another 5x5 grid,
// which itself contains another 5x5 grid, and so on.
//
// The scan you took (your puzzle input) shows where the bugs are on a
// single level of this structure. The middle tile of your scan is empty
// to accommodate the recursive grids within it. Initially, no other
// levels contain bugs.
//
// Tiles still count as adjacent if they are directly up, down, left, or
// right of a given tile. Some tiles have adjacent tiles at a recursion
// level above or below its own level. For example:
//
// |     |         |     |
// 1  |  2  |    3    |  4  |  5
// |     |         |     |
// -----+-----+---------+-----+-----
// |     |         |     |
// 6  |  7  |    8    |  9  |  10
// |     |         |     |
// -----+-----+---------+-----+-----
// |     |A|B|C|D|E|     |
// |     |-+-+-+-+-|     |
// |     |F|G|H|I|J|     |
// |     |-+-+-+-+-|     |
// 11  | 12  |K|L|?|N|O|  14 |  15
// |     |-+-+-+-+-|     |
// |     |P|Q|R|S|T|     |
// |     |-+-+-+-+-|     |
// |     |U|V|W|X|Y|     |
// -----+-----+---------+-----+-----
// |     |         |     |
// 16  | 17  |    18   |  19 |  20
// |     |         |     |
// -----+-----+---------+-----+-----
// |     |         |     |
// 21  | 22  |    23   |  24 |  25
// |     |         |     |
//
// * Tile 19 has four adjacent tiles: 14, 18, 20, and 24.
//
// * Tile G has four adjacent tiles: B, F, H, and L.
//
// * Tile D has four adjacent tiles: 8, C, E, and I.
//
// * Tile E has four adjacent tiles: 8, D, 14, and J.
//
// * Tile 14 has eight adjacent tiles: 9, E, J, O, T, Y, 15, and 19.
//
// * Tile N has eight adjacent tiles: I, O, S, and five tiles within
// the sub-grid marked ?.
//
// The rules about bugs living and dying are the same as before.
//
// For example, consider the same initial state as above:
//
// ....#
// #..#.
// #.?##
// ..#..
// #....
//
// The center tile is drawn as ? to indicate the next recursive grid.
// Call this level 0; the grid within this one is level 1, and the grid
// that contains this one is level -1. Then, after ten minutes, the grid
// at each level would look like this:
//
// Depth -5:
// ..#..
// .#.#.
// ..?.#
// .#.#.
// ..#..
//
// Depth -4:
// ...#.
// ...##
// ..?..
// ...##
// ...#.
//
// Depth -3:
// #.#..
// .#...
// ..?..
// .#...
// #.#..
//
// Depth -2:
// .#.##
// ....#
// ..?.#
// ...##
// .###.
//
// Depth -1:
// #..##
// ...##
// ..?..
// ...#.
// .####
//
// Depth 0:
// .#...
// .#.##
// .#?..
// .....
// .....
//
// Depth 1:
// .##..
// #..##
// ..?.#
// ##.##
// #####
//
// Depth 2:
// ###..
// ##.#.
// #.?..
// .#.##
// #.#..
//
// Depth 3:
// ..###
// .....
// #.?..
// #....
// #...#
//
// Depth 4:
// .###.
// #..#.
// #.?..
// ##.#.
// .....
//
// Depth 5:
// ####.
// #..#.
// #.?#.
// ####.
// .....
//
// In this example, after 10 minutes, a total of 99 bugs are present.
//
// Starting with your scan, how many bugs are present after 200 minutes?

#include "advent_of_code/2019/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/opt_cmp.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

absl::StatusOr<CharBoard> ParseBoard(absl::Span<absl::string_view> input) {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board;
  if (board->height() != 5) {
    return absl::InvalidArgumentError("Board isn't of height 5");
  }
  if (board->width() != 5) {
    return absl::InvalidArgumentError("Board isn't of width 5");
  }
  return board;
}

CharBoard StepGameOfLine(const CharBoard& input) {
  CharBoard output = input;
  for (Point p : input.range()) {
    int neighbors = 0;
    for (Point dir : Cardinal::kFourDirs) {
      Point n = dir + p;
      if (!input.OnBoard(n)) continue;
      if (input[n] == '#') ++neighbors;
    }
    if (input[p] == '#') {
      if (neighbors != 1) output[p] = '.';
    } else {
      if (neighbors == 1) output[p] = '#';
      if (neighbors == 2) output[p] = '#';
    }
  }
  return output;
}

int64_t BioDiversity(const CharBoard& input) {
  int64_t ret = 0;
  for (Point p : input.range()) {
    if (input[p] == '#') {
      ret |= (1ll << (p.y * input.width() + p.x));
    }
  }
  return ret;
}

absl::optional<CharBoard> NewBoardPlusOneLevel(const CharBoard& in_board) {
  CharBoard new_board(in_board.width(), in_board.height());
  bool added = false;
  if (in_board[{2, 1}] == '#') {
    for (int sub_x = 0; sub_x < new_board.width(); ++sub_x) {
      new_board[{sub_x, 0}] = '#';
      added = true;
    }
  }
  if (in_board[{2, 3}] == '#') {
    for (int sub_x = 0; sub_x < new_board.width(); ++sub_x) {
      new_board[{sub_x, 4}] = '#';
      added = true;
    }
  }
  if (in_board[{1, 2}] == '#') {
    for (int sub_y = 0; sub_y < new_board.width(); ++sub_y) {
      new_board[{0, sub_y}] = '#';
      added = true;
    }
  }
  if (in_board[{3, 2}] == '#') {
    for (int sub_y = 0; sub_y < new_board.width(); ++sub_y) {
      new_board[{4, sub_y}] = '#';
      added = true;
    }
  }
  if (!added) return absl::nullopt;
  return new_board;
}

absl::optional<CharBoard> NewBoardMinusOneLevel(const CharBoard& in_board) {
  CharBoard new_board(in_board.width(), in_board.height());
  bool added = false;
  int count = 0;
  for (int x = 0; x < new_board.width(); ++x) {
    if (in_board[{x, 0}] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[{2, 1}] = '#';
    added = true;
  }
  count = 0;
  for (int x = 0; x < new_board.width(); ++x) {
    if (in_board[{x, 4}] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[{2, 3}] = '#';
    added = true;
  }
  count = 0;
  for (int y = 0; y < new_board.height(); ++y) {
    if (in_board[{0, y}] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[{1, 2}] = '#';
    added = true;
  }
  count = 0;
  for (int y = 0; y < new_board.height(); ++y) {
    if (in_board[{4, y}] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[{3, 2}] = '#';
    added = true;
  }
  if (!added) return absl::nullopt;
  return new_board;
}

int CountNeighborsRecursive(const CharBoard& in_board, Point p,
                            absl::optional<CharBoard> board_minus_one,
                            absl::optional<CharBoard> board_plus_one) {
  int neighbors = 0;
  for (Point dir : Cardinal::kFourDirs) {
    Point n = dir + p;
    if (n.x < 0) {
      if (board_minus_one) {
        if ((*board_minus_one)[{1, 2}] == '#') ++neighbors;
      }
    } else if (n.x >= in_board.width()) {
      if (board_minus_one) {
        if ((*board_minus_one)[{3, 2}] == '#') ++neighbors;
      }
    } else if (n.y < 0) {
      if (board_minus_one) {
        if ((*board_minus_one)[{2, 1}] == '#') ++neighbors;
      }
    } else if (n.y >= in_board.height()) {
      if (board_minus_one) {
        if ((*board_minus_one)[{2, 3}] == '#') ++neighbors;
      }
    } else if (n == Point{2, 2}) {
      if (board_plus_one) {
        if (p == Point{1, 2}) {
          for (int sub_y = 0; sub_y < board_plus_one->height(); ++sub_y) {
            if ((*board_plus_one)[{0, sub_y}] == '#') ++neighbors;
          }
        } else if (p == Point{3, 2}) {
          for (int sub_y = 0; sub_y < board_plus_one->height(); ++sub_y) {
            if ((*board_plus_one)[{4, sub_y}] == '#') ++neighbors;
          }
        } else if (p == Point{2, 1}) {
          for (int sub_x = 0; sub_x < board_plus_one->width(); ++sub_x) {
            if ((*board_plus_one)[{sub_x, 0}] == '#') ++neighbors;
          }
        } else if (p == Point{2, 3}) {
          for (int sub_x = 0; sub_x < board_plus_one->width(); ++sub_x) {
            if ((*board_plus_one)[{sub_x, 4}] == '#') ++neighbors;
          }
        } else {
          DLOG(FATAL) << "Can't get here";
        }
      }
    } else if (in_board[n] == '#') {
      ++neighbors;
    }
  }
  return neighbors;
}

absl::flat_hash_map<int, CharBoard> StepGameOfLineRecursive(
    absl::flat_hash_map<int, CharBoard> depth_to_board) {
  absl::flat_hash_map<int, CharBoard> out;
  for (const auto& depth_and_board : depth_to_board) {
    int depth = depth_and_board.first;
    const CharBoard& in_board = depth_and_board.second;
    CharBoard out_board = in_board;
    absl::optional<CharBoard> board_minus_one =
        opt_find(depth_to_board, depth - 1);
    absl::optional<CharBoard> board_plus_one =
        opt_find(depth_to_board, depth + 1);
    for (Point p : in_board.range()) {
      if (p == Point{2, 2}) {
        if (!board_plus_one) {
          absl::optional<CharBoard> new_board = NewBoardPlusOneLevel(in_board);
          if (new_board) {
            out.emplace(depth + 1, std::move(*new_board));
          }
        }
        out_board[p] = '?';
      } else {
        int neighbors = CountNeighborsRecursive(in_board, p, board_minus_one,
                                                board_plus_one);
        if (in_board[p] == '#') {
          if (neighbors != 1) out_board[p] = '.';
        } else {
          if (neighbors == 1) out_board[p] = '#';
          if (neighbors == 2) out_board[p] = '#';
        }
      }
      if (!board_minus_one) {
        absl::optional<CharBoard> new_board = NewBoardMinusOneLevel(in_board);
        if (new_board) {
          out.emplace(depth - 1, std::move(*new_board));
        }
      }
    }
    out.emplace(depth, std::move(out_board));
  }
  return out;
}

std::string DebugBoards(absl::flat_hash_map<int, CharBoard> depth_to_board) {
  int min = std::numeric_limits<int>::max();
  int max = std::numeric_limits<int>::min();
  for (const auto& pair : depth_to_board) {
    min = std::min(min, pair.first);
    max = std::max(max, pair.first);
  }
  std::string out;
  for (int depth = min; depth <= max; ++depth) {
    std::string hdr = absl::StrCat("D:", depth);
    hdr.resize(6, ' ');
    absl::StrAppend(&out, hdr);
  }
  absl::StrAppend(&out, "\n");
  for (int y = 0; y < 5; ++y) {
    for (int depth = min; depth <= max; ++depth) {
      auto it = depth_to_board.find(depth);
      if (it == depth_to_board.end()) {
        absl::StrAppend(&out, "       ");
      } else {
        for (int x = 0; x < 5; ++x) {
          out.append(1, it->second[{x, y}]);
        }
        out.append(1, ' ');
      }
    }
    absl::StrAppend(&out, "\n");
  }
  return out;
}

int64_t CountBugs(absl::flat_hash_map<int, CharBoard> depth_to_board) {
  int64_t bugs = 0;
  for (const auto& pair : depth_to_board) {
    bugs += pair.second.CountOn();
  }
  return bugs;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseBoard(input);
  if (!board.ok()) return board.status();

  CharBoard cur = *board;
  absl::flat_hash_set<CharBoard> hist;
  while (!hist.contains(cur)) {
    hist.insert(cur);
    VLOG(2) << "Cur Board:\n" << cur;
    cur = StepGameOfLine(cur);
  }
  VLOG(1) << "Dupe Board:\n" << cur;
  return IntReturn(BioDiversity(cur));
}

absl::StatusOr<std::string> Day_2019_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> cur = ParseBoard(input);
  if (!cur.ok()) return cur.status();

  absl::flat_hash_map<int, CharBoard> level_to_board;
  level_to_board.emplace(0, *cur);

  for (int i = 0; i < 200; ++i) {
    VLOG(2) << "Boards[" << i << "]:\n" << DebugBoards(level_to_board);
    absl::flat_hash_map<int, CharBoard> next =
        StepGameOfLineRecursive(level_to_board);
    level_to_board = std::move(next);
  }
  return IntReturn(CountBugs(level_to_board));
}

}  // namespace advent_of_code
