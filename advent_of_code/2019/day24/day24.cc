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

absl::StatusOr<std::vector<std::string>> Day24_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return IntReturn(-1);
}
