#include "advent_of_code/2019/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/opt_cmp.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
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
          LOG(DFATAL) << "Can't get here";
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
    absl::optional<CharBoard> board_minus_one = opt_find(depth_to_board, depth - 1);
    absl::optional<CharBoard> board_plus_one = opt_find(depth_to_board, depth + 1);
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
    for (Point p : pair.second.range()) {
      if (pair.second[p] == '#') ++bugs;
    }
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
