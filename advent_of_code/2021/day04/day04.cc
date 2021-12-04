#include "advent_of_code/2021/day04/day04.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Board {
  Board() {
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        board[i][j] = -1;
        selected[i][j] = false;
      }
    }
  }

  std::array<std::array<int64_t, 5>, 5> board;
  std::array<std::array<bool, 5>, 5> selected;

  friend std::ostream& operator<<(std::ostream& o, const Board& b) {
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        o << b.board[i][j] << " ";
        if (b.selected[i][j]) {
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
        if (selected[i][j]) continue;
        score += board[i][j];
      }
    }
    return score;
  }

  void Mark(int64_t val) {
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        if (board[i][j] == val) {
          selected[i][j] = true;
        }
      }
    }
  }

  bool IsWin() const {
    for (int i = 0; i < 5; ++i) {
      bool all_row = true;
      for (int j = 0; j < 5; ++j) {
        if (!selected[i][j]) {
          all_row = false;
          break;
        }
      }
      if (all_row) return true;
      bool all_col = true;
      for (int j = 0; j < 5; ++j) {
        if (!selected[j][i]) {
          all_col = false;
          break;
        }
      }
      if (all_col) return true;
    }
    return false;
    // diagonals don't count.
    bool all_diag = true;
    for (int i = 0; i < 5; ++i) {
      if (!selected[i][i]) {
        all_diag = false;
        break;
      }
    }
    if (all_diag) return true;
    all_diag = true;
    for (int i = 0; i < 5; ++i) {
      if (!selected[i][4 - i]) {
        all_diag = false;
        break;
      }
    }
    return all_diag;
  }
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
    Board build;
    for (int i = 0; i < 5; ++i) {
      if (!RE2::FullMatch(input[lidx + i], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*",
                          &build.board[i][0], &build.board[i][1], &build.board[i][2], &build.board[i][3], &build.board[i][4])) {
        return Error("Bad board line: ", input[lidx]);
      }
    }
    boards.push_back(build);
  }

  for (int64_t num : numbers) {
    for (Board& b : boards) {
      b.Mark(num);
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
    Board build;
    for (int i = 0; i < 5; ++i) {
      if (!RE2::FullMatch(input[lidx + i], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*",
                          &build.board[i][0], &build.board[i][1], &build.board[i][2], &build.board[i][3], &build.board[i][4])) {
        return Error("Bad board line: ", input[lidx]);
      }
    }
    boards.push_back(build);
  }

  for (int64_t num : numbers) {
    for (Board& b : boards) {
      b.Mark(num);
    }
    std::vector<Board> new_boards;
    for (const Board& b : boards) {
      if (!b.IsWin()) {
        new_boards.push_back(b);
      }
    }
    if (new_boards.empty()) {
      if (boards.size() != 1) return Error("Non unique");
      return IntReturn(boards[0].Score() * num);
    }
    boards = std::move(new_boards);
  }


  return Error("No board found");
}

}  // namespace advent_of_code
