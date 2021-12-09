#include "advent_of_code/2021/day09/day09.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::vector<Point> FindLow(absl::Span<absl::string_view> input) {
  std::vector<Point> ret;
  for (int y = 0; y < input.size(); ++y) {
    for (int x = 0; x < input[y].size(); ++x) {
      Point p{x, y};
      bool low = true;
      for (Point d : Cardinal::kFourDirs) {
        Point p2 = p + d;
        if (p2.x < 0) continue;
        if (p2.y < 0) continue;
        if (p2.x >= input[y].size()) continue;
        if (p2.y >= input.size()) continue;
        if (input[p.y][p.x] >= input[p2.y][p2.x]) {
          low = false;
          break;
        }
      }
      if (low) {
        ret.push_back(p);
      }
    }
  }
  return ret;
}

int64_t BasinSize(absl::Span<absl::string_view> input, Point p) {
  absl::flat_hash_set<Point> history = {p};
  for (std::deque<Point> frontier = {p}; !frontier.empty(); frontier.pop_front()) {
    Point cur = frontier.front();
    for (Point d : Cardinal::kFourDirs) {
      Point n = cur + d;
      if (n.x < 0) continue;
      if (n.y < 0) continue;
      if (n.y >= input.size()) continue;
      if (n.x >= input[n.y].size()) continue;
      if (input[n.y][n.x] == '9') continue;
      if (history.contains(n)) continue;
      history.insert(n);
      frontier.push_back(n);
    }
  }
  return history.size();
}


}  // namespace

absl::StatusOr<std::string> Day_2021_09::Part1(
    absl::Span<absl::string_view> input) const {
  
  std::vector<Point> basins = FindLow(input);
  int64_t sum_risk = 0;
  for (Point p : basins) {
    sum_risk += input[p.y][p.x] + 1 - '0';
  }

  return IntReturn(sum_risk);
}

absl::StatusOr<std::string> Day_2021_09::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Point> basins = FindLow(input);
  if (basins.size() < 3) return Error("Not enough basins");
  std::vector<int64_t> sizes;
  for (Point start : basins) {
    sizes.push_back(BasinSize(input, start));
    VLOG(1) << start << " => " << sizes.back();
  }
  absl::c_sort(sizes);
  absl::c_reverse(sizes);

  return IntReturn(sizes[0] * sizes[1] * sizes[2]);
}

}  // namespace advent_of_code
