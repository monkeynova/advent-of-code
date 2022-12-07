// https://adventofcode.com/2021/day/4
//
// --- Day 4: Giant Squid ---
// --------------------------
//
// You're already almost 1.5km (almost a mile) below the surface of the
// ocean, already so deep that you can't see any sunlight. What you can
// see, however, is a giant squid that has attached itself to the outside
// of your submarine.
//
// Maybe it wants to play bingo?
//
// Bingo is played on a set of boards each consisting of a 5x5 grid of
// numbers. Numbers are chosen at random, and the chosen number is marked
// on all boards on which it appears. (Numbers may not appear on all
// boards.) If all numbers in any row or any column of a board are
// marked, that board wins. (Diagonals don't count.)
//
// The submarine has a bingo subsystem to help passengers (currently, you
// and the giant squid) pass the time. It automatically generates a
// random order in which to draw numbers and a random set of boards (your
// puzzle input). For example:
//
// 7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1
//
// 22 13 17 11  0
// 8  2 23  4 24
// 21  9 14 16  7
// 6 10  3 18  5
// 1 12 20 15 19
//
// 3 15  0  2 22
// 9 18 13 17  5
// 19  8  7 25 23
// 20 11 10 24  4
// 14 21 16 12  6
//
// 14 21 17 24  4
// 10 16 15  9 19
// 18  8 23 26 20
// 22 11 13  6  5
// 2  0 12  3  7
//
// After the first five numbers are drawn (7, 4, 9, 5, and 11), there are
// no winners, but the boards are marked as follows (shown here adjacent
// to each other to save space):
//
// 22 13 17 11  0         3 15  0  2 22        14 21 17 24  4
//  8  2 23  4 24         9 18 13 17  5        10 16 15  9 19
// 21  9 14 16  7        19  8  7 25 23        18  8 23 26 20
//  6 10  3 18  5        20 11 10 24  4        22 11 13  6  5
//  1 12 20 15 19        14 21 16 12  6         2  0 12  3  7
//
// After the next six numbers are drawn (17, 23, 2, 0, 14, and 21), there
// are still no winners:
//
// 22 13 17 11  0         3 15  0  2 22        14 21 17 24  4
//  8  2 23  4 24         9 18 13 17  5        10 16 15  9 19
// 21  9 14 16  7        19  8  7 25 23        18  8 23 26 20
//  6 10  3 18  5        20 11 10 24  4        22 11 13  6  5
//  1 12 20 15 19        14 21 16 12  6         2  0 12  3  7
//
// Finally, 24 is drawn:
//
// 22 13 17 11  0         3 15  0  2 22        14 21 17 24  4
//  8  2 23  4 24         9 18 13 17  5        10 16 15  9 19
// 21  9 14 16  7        19  8  7 25 23        18  8 23 26 20
//  6 10  3 18  5        20 11 10 24  4        22 11 13  6  5
//  1 12 20 15 19        14 21 16 12  6         2  0 12  3  7
//
// At this point, the third board wins because it has at least one
// complete row or column of marked numbers (in this case, the entire top
// row is marked: 14 21 17 24 4).
//
// The score of the winning board can now be calculated. Start by finding
// the sum of all unmarked numbers on that board; in this case, the sum
// is 188. Then, multiply that sum by the number that was just called
// when the board won, 24, to get the final score, 188 * 24 = 4512.
//
// To guarantee victory against the giant squid, figure out which board
// will win first. What will your final score be if you choose that
// board?
//
// --- Part Two ---
// ----------------
//
// On the other hand, it might be wise to try a different strategy: let
// the giant squid win.
//
// You aren't sure how many bingo boards a giant squid could play at
// once, so rather than waste time counting its arms, the safe thing to
// do is to figure out which board will win last and choose that one.
// That way, no matter which boards it picks, it will win for sure.
//
// In the above example, the second board is the last to win, which
// happens after 13 is eventually called and its middle column is
// completely marked. If you were to keep playing until this point, the
// second board would have a sum of unmarked numbers equal to 148 for a
// final score of 148 * 13 = 1924.
//
// Figure out which board will win last. Once it wins, what would its
// final score be?

#include "advent_of_code/2021/day04/day04.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Board {
 public:
  static absl::StatusOr<Board> Parse(absl::Span<absl::string_view> data) {
    Board build;
    if (data.size() != 5) return Error("Bad size");
    static const RE2 line_re(
        "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*");
    for (int i = 0; i < 5; ++i) {
      if (!RE2::FullMatch(data[i], line_re, &build.board_[i][0],
                          &build.board_[i][1], &build.board_[i][2],
                          &build.board_[i][3], &build.board_[i][4])) {
        return Error("Bad board line: ", data[i]);
      }
      for (int j = 0; j < 5; ++j) {
        build.selected_[i][j] = false;
      }
    }
    return build;
  }

  friend std::ostream& operator<<(std::ostream& o, const Board& b) {
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        o << b.board_[i][j] << " ";
        if (b.selected_[i][j]) {
          o << "* ";
        }
      }
      o << "\n";
    }
    return o;
  }

  int64_t Score() const {
    int64_t score = 0;
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        if (selected_[i][j]) continue;
        score += board_[i][j];
      }
    }
    return score;
  }

  absl::Status Mark(int64_t val) {
    bool found = false;
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        if (board_[i][j] == val) {
          if (found) return Error("Multi-value");
          found = true;
          selected_[i][j] = true;
        }
      }
    }
    return absl::OkStatus();
  }

  bool IsWin() const {
    static constexpr std::array<int, 5> kIdxList = {0, 1, 2, 3, 4};
    for (int i : kIdxList) {
      if (absl::c_all_of(kIdxList, [&](int j) { return selected_[i][j]; })) {
        return true;
      }
      if (absl::c_all_of(kIdxList, [&](int j) { return selected_[j][i]; })) {
        return true;
      }
    }
    // diagonals don't count.
    return false;
  }

 private:
  Board() = default;

  std::array<std::array<int64_t, 5>, 5> board_;
  std::array<std::array<bool, 5>, 5> selected_;
};

}  // namespace

absl::StatusOr<std::string> Day_2021_04::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() % 6 != 1) return Error("Bad boards: ", input.size());
  std::vector<int64_t> numbers;
  for (absl::string_view num_str : absl::StrSplit(input[0], ",")) {
    int64_t num;
    if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
    numbers.push_back(num);
  }

  std::vector<Board> boards;
  for (int64_t lidx = 2; lidx < input.size(); lidx += 6) {
    if (!input[lidx - 1].empty()) return Error("Bad spacer");
    absl::StatusOr<Board> board = Board::Parse(input.subspan(lidx, 5));
    if (!board.ok()) return board.status();
    boards.push_back(*board);
  }

  for (int64_t num : numbers) {
    for (Board& b : boards) {
      if (auto st = b.Mark(num); !st.ok()) return st;
    }
    for (const Board& b : boards) {
      if (b.IsWin()) {
        return IntReturn(b.Score() * num);
      }
    }
  }

  return Error("No board found");
}

absl::StatusOr<std::string> Day_2021_04::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() % 6 != 1) return Error("Bad boards: ", input.size());
  std::vector<int64_t> numbers;
  for (absl::string_view num_str : absl::StrSplit(input[0], ",")) {
    int64_t num;
    if (!absl::SimpleAtoi(num_str, &num)) return Error("Bad num: ", num_str);
    numbers.push_back(num);
  }

  std::vector<Board> boards;
  for (int64_t lidx = 2; lidx < input.size(); lidx += 6) {
    if (!input[lidx - 1].empty()) return Error("Bad spacer");
    absl::StatusOr<Board> board = Board::Parse(input.subspan(lidx, 5));
    if (!board.ok()) return board.status();
    boards.push_back(*board);
  }

  for (int64_t num : numbers) {
    for (Board& b : boards) {
      if (auto st = b.Mark(num); !st.ok()) return st;
    }
    auto it = std::remove_if(boards.begin(), boards.end(),
                             [](const Board& b) { return b.IsWin(); });
    if (it == boards.begin()) {
      if (boards.size() != 1) return Error("Non unique");
      return IntReturn(boards[0].Score() * num);
    }
    boards.erase(it, boards.end());
  }

  return Error("No board found");
}

}  // namespace advent_of_code
