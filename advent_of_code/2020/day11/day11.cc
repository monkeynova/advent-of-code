#include "advent_of_code/2020/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

using Board = std::vector<std::string>;

Board Update(Board in) {
  Board out;
  for (int y = 0; y < in.size(); ++y) {
    out.push_back(in[y]);
    for (int x = 0; x < in[y].size(); ++x) {
      Point c{x, y};
      int neighbors = 0;
      for (Point dir : Cardinal::kAllPlus) {
        Point n = c + dir;
        if (n.x < 0) continue;
        if (n.x >= in[y].size()) continue;
        if (n.y < 0) continue;
        if (n.y >= in.size()) continue;
        if (in[n.y][n.x] == '#') ++neighbors;
      }
      if (in[c.y][c.x] == 'L') {
        if (neighbors == 0) out[c.y][c.x] = '#';
      } else if (in[c.y][c.x] == '#') {
        if (neighbors >= 4) out[c.y][c.x] = 'L';
      }
    }
  }
  return out;
}

int CountSeats(Board b) {
  int count = 0;
  for (int y = 0; y < b.size(); ++y) {
    for (int x = 0; x < b[y].size(); ++x) {
      if (b[y][x] == '#') ++count;
    }
  }
  return count;
}

absl::StatusOr<std::vector<std::string>> Day11_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  Board cur;
  for (absl::string_view str : input) {
    cur.push_back(std::string(str));
  }
  for (int i = 0;; ++i) {
    LOG(WARNING) << "Board:\n" << absl::StrJoin(cur, "\n") << "\n";
    Board next = Update(cur);
    if (next == cur) break;
    cur = next;
  }
  return IntReturn(CountSeats(cur));
}

absl::StatusOr<std::vector<std::string>> Day11_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}
