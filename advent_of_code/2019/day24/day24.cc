#include "advent_of_code/2019/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

std::vector<std::string> StepGameOfLine(std::vector<std::string> input) {
  std::vector<std::string> out;
  for (int y = 0; y < input.size(); ++y) {
    std::string cur_row(input[y]);
    for (int x = 0; x < input[y].size(); ++x) {
      Point p{x, y};
      int neighbors = 0;
      for (Point dir : Cardinal::kAll) {
        Point n = dir + p;
        if (n.x < 0) continue;
        if (n.x >= input[y].size()) continue;
        if (n.y < 0) continue;
        if (n.y >= input[y].size()) continue;
        if (input[n.y][n.x] == '#') ++neighbors;
      }
      if (input[p.y][p.x] == '#') {
        if (neighbors != 1) cur_row[x] = '.';
      } else {
        if (neighbors == 1) cur_row[x] = '#';
        if (neighbors == 2) cur_row[x] = '#';
      }
    }
    out.push_back(cur_row);
  }
  return out;
}

int64_t BioDiversity(std::vector<std::string> input) {
  int64_t ret = 0;
  for (int y = 0; y < input.size(); ++y) {
    std::string cur_row(input[y]);
    for (int x = 0; x < input[y].size(); ++x) {
      if (input[y][x] == '#') {
        ret |= (1ll << (y * input[y].size() + x));
      }
    }
  }
  return ret;
}

absl::StatusOr<std::vector<std::string>> Day24_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  if (input.empty()) return absl::InvalidArgumentError("Empty board");
  int width = input[0].size();
  std::vector<std::string> cur;
  for (absl::string_view str : input) {
    if (str.size() != width) return absl::InvalidArgumentError("Bad board");
    cur.push_back(std::string(str));
  }

  absl::flat_hash_set<std::vector<std::string>> hist;
  while (!hist.contains(cur)) {
    hist.insert(cur);
    VLOG(2) << "Cur Board:\n" << absl::StrJoin(cur, "\n");
    std::vector<std::string> next = StepGameOfLine(cur);
    cur = std::move(next);
  }
  VLOG(1) << "Dupe Board:\n" << absl::StrJoin(cur, "\n");
  return IntReturn(BioDiversity(cur));
}

absl::optional<std::vector<std::string>> NewBoardPlusOneLevel(
  const std::vector<std::string>& in_board) {
  std::string empty_row(in_board[2].size(), '.');
  std::vector<std::string> new_board(in_board.size(), empty_row);
  bool added = false;
  if (in_board[1][2] == '#') {
    for (int sub_x = 0; sub_x < new_board[0].size(); ++sub_x) {
      new_board[0][sub_x] = '#';
      added = true;
    }
  }
  if (in_board[3][2] == '#') {
    for (int sub_x = 0; sub_x < new_board[4].size(); ++sub_x) {
      new_board[4][sub_x] = '#';
      added = true;
    }
  }
  if (in_board[2][1] == '#') {
    for (int sub_y = 0; sub_y < new_board.size(); ++sub_y) {
      new_board[sub_y][0] = '#';
      added = true;
    }
  }
  if (in_board[2][3] == '#') {
    for (int sub_y = 0; sub_y < new_board.size(); ++sub_y) {
      new_board[sub_y][4] = '#';
      added = true;
    }
  }
  if (!added) return absl::nullopt;
  return new_board;
}

absl::optional<std::vector<std::string>> NewBoardMinusOneLevel(
  const std::vector<std::string>& in_board) {
  std::string empty_row(in_board[0].size(), '.');
  std::vector<std::string> new_board(in_board.size(), empty_row);
  bool added = false;
  int count = 0;
  for (int x = 0; x < in_board[0].size(); ++x) {
    if (in_board[0][x] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[1][2] = '#';
    added = true;
  }
  count = 0;
  for (int x = 0; x < in_board[4].size(); ++x) {
    if (in_board[4][x] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[3][2] = '#';
    added = true;
  }
  count = 0;
  for (int y = 0; y < in_board.size(); ++y) {
    if (in_board[y][0] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[2][1] = '#';
    added = true;
  }
  count = 0;
  for (int y = 0; y < in_board.size(); ++y) {
    if (in_board[y][4] == '#') ++count;
  }
  if (count == 1 || count == 2) {
    new_board[2][3] = '#';
    added = true;
  }
  if (!added) return absl::nullopt;
  return new_board;
}

int CountNeighbors(const std::vector<std::string>& in_board, Point p,
                   absl::optional<std::vector<std::string>> board_minus_one,
                   absl::optional<std::vector<std::string>> board_plus_one) {
  int neighbors = 0;
  for (Point dir : Cardinal::kAll) {
    Point n = dir + p;
    if (n.x < 0) {
      if (board_minus_one) {
        if ((*board_minus_one)[2][1] == '#') ++neighbors;
      }
    } else if (n.x >= in_board[0].size()) {
      if (board_minus_one) {
        if ((*board_minus_one)[2][3] == '#') ++neighbors;
      }
    } else if (n.y < 0) {
      if (board_minus_one) {
        if ((*board_minus_one)[1][2] == '#') ++neighbors;
      }
    } else if (n.y >= in_board.size()) {
      if (board_minus_one) {
        if ((*board_minus_one)[3][2] == '#') ++neighbors;
      }
    } else if (n == Point{2, 2}) {
      if (board_plus_one) {
        if (p == Point{1, 2}) {
          for (int sub_y = 0; sub_y < board_plus_one->size(); ++sub_y) {
            if ((*board_plus_one)[sub_y][0] == '#') ++neighbors; 
          }
        } else if (p == Point{3, 2}) {
          for (int sub_y = 0; sub_y < board_plus_one->size(); ++sub_y) {
            if ((*board_plus_one)[sub_y][4] == '#') ++neighbors; 
          }
        } else if (p == Point{2, 1}) {
          for (int sub_x = 0; sub_x < board_plus_one->size(); ++sub_x) {
            if ((*board_plus_one)[0][sub_x] == '#') ++neighbors; 
          }
        } else if (p == Point{2, 3}) {
          for (int sub_x = 0; sub_x < board_plus_one->size(); ++sub_x) {
            if ((*board_plus_one)[4][sub_x] == '#') ++neighbors; 
          }
        } else {
          LOG(DFATAL) << "Can't get here";
        }
      }
    } else if (in_board[n.y][n.x] == '#') {
      ++neighbors;
    }
  }
  return neighbors;
}

absl::flat_hash_map<int, std::vector<std::string>> StepGameOfLineRecursive(
      absl::flat_hash_map<int, std::vector<std::string>> depth_to_board) {
  absl::flat_hash_map<int, std::vector<std::string>> out;
  for (const auto& depth_and_board : depth_to_board) {
    int depth = depth_and_board.first;
    const std::vector<std::string>& in_board = depth_and_board.second;
    std::vector<std::string> out_board;
    absl::optional<std::vector<std::string>> board_minus_one = absl::nullopt;
    if (auto it = depth_to_board.find(depth - 1); it != depth_to_board.end()) {
      board_minus_one = it->second;
    }
    absl::optional<std::vector<std::string>> board_plus_one = absl::nullopt;
    if (auto it = depth_to_board.find(depth + 1); it != depth_to_board.end()) {
      board_plus_one = it->second;
    }
    for (int y = 0; y < in_board.size(); ++y) {
      std::string cur_row(in_board[y]);
      for (int x = 0; x < in_board[y].size(); ++x) {
        Point p{x, y};
        if (p == Point{2,2}) {
          if (!board_plus_one) {
            absl::optional<std::vector<std::string>> new_board =
                NewBoardPlusOneLevel(in_board);
            if (new_board) {
              out.emplace(depth + 1, std::move(*new_board));
            }
          }
          cur_row[x] = '?';
        } else {
          int neighbors = CountNeighbors(in_board, p, board_minus_one, board_plus_one);
          if (in_board[p.y][p.x] == '#') {
            if (neighbors != 1) cur_row[x] = '.';
          } else {
            if (neighbors == 1) cur_row[x] = '#';
            if (neighbors == 2) cur_row[x] = '#';
          }
        }
      }
      if (!board_minus_one) {
        absl::optional<std::vector<std::string>> new_board =
           NewBoardMinusOneLevel(in_board);
        if (new_board) {
          out.emplace(depth - 1, std::move(*new_board));
        }
      }
      out_board.push_back(cur_row);
    }
    out.emplace(depth, std::move(out_board));
  }
  return out;
}

std::string DebugBoards(
  absl::flat_hash_map<int, std::vector<std::string>> depth_to_board) {
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
        absl::StrAppend(&out, it->second[y], " ");
      }
    }
    absl::StrAppend(&out, "\n");
  }
  return out;
}

int64_t CountBugs(absl::flat_hash_map<int, std::vector<std::string>> depth_to_board) {
  int64_t bugs = 0;
  for (const auto& pair : depth_to_board) {
    for (const std::string& str : pair.second) {
      for (int i = 0; i < str.size(); ++i) {
        if (str[i] == '#') ++bugs;
      }
    }
  }
  return bugs;
}

absl::StatusOr<std::vector<std::string>> Day24_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  if (input.empty()) return absl::InvalidArgumentError("Empty board");
  int width = input[0].size();
  std::vector<std::string> cur;
  for (absl::string_view str : input) {
    if (str.size() != width) return absl::InvalidArgumentError("Bad board");
    cur.push_back(std::string(str));
  }

  absl::flat_hash_map<int, std::vector<std::string>> level_to_board;
  level_to_board[0] = cur;

  for (int i = 0; i < 200; ++i) {
    VLOG(2) << "Boards[" << i << "]:\n" << DebugBoards(level_to_board);
    absl::flat_hash_map<int, std::vector<std::string>> next =
       StepGameOfLineRecursive(level_to_board);
    level_to_board = std::move(next);
  }
  return IntReturn(CountBugs(level_to_board));
}
