#include "advent_of_code/2021/day04/day04.h"

#include "absl/algorithm/container.h"
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

class Board {
 public:
  static absl::StatusOr<Board> Parse(absl::Span<absl::string_view> data) {
    Board build;
    if (data.size() != 5) return AdventDay::Error("Bad size");
    static const RE2 line_re(
        "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s*");
    for (int i = 0; i < 5; ++i) {
      if (!RE2::FullMatch(data[i], line_re, &build.board_[i][0],
                          &build.board_[i][1], &build.board_[i][2],
                          &build.board_[i][3], &build.board_[i][4])) {
        return AdventDay::Error("Bad board line: ", data[i]);
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
          if (found) return AdventDay::Error("Multi-value");
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
