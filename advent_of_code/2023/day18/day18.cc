// http://adventofcode.com/2023/day/18

#include "advent_of_code/2023/day18/day18.h"

#include "absl/algorithm/container.h"
#include "absl/container/btree_set.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

struct VerticalSegment {
  int x;
  int y1;
  int y2;
  bool operator<(const VerticalSegment& o) const {
    return x < o.x;
  }
};

int64_t CountInterior(const std::vector<std::pair<Point, Point>>& loop) {
  absl::flat_hash_map<int, std::vector<VerticalSegment>> y_to_start;
  absl::flat_hash_map<int, std::vector<VerticalSegment>> y_to_end;
  absl::flat_hash_set<int> y_distinct;
  for (const auto& pair : loop) {
    if (pair.first.x != pair.second.x) continue;
    VerticalSegment v = {.x = pair.first.x, .y1 = pair.first.y, .y2 = pair.second.y};
    y_distinct.insert(pair.first.y);
    y_distinct.insert(pair.second.y);
    y_to_start[pair.first.y].push_back(v);
    y_to_end[pair.second.y].push_back(v);
  }
  std::vector<int> y_vals(y_distinct.begin(), y_distinct.end());
  absl::c_sort(y_vals);

  absl::btree_set<VerticalSegment> active;

  auto count_on = [&active](int y) {
    bool in_upper = false;
    bool in_lower = false;
    int last_x = std::numeric_limits<int>::min();

    int64_t row_count = 0;
    for (const auto& [x, y1, y2] : active) {
      bool start_inside = in_upper || in_lower;

      if (y1 == y) in_lower = !in_lower;
      else if (y2 == y) in_upper = !in_upper;
      else if (y1 < y && y2 > y) {
        in_lower = !in_lower;
        in_upper = !in_upper;
      }

      if (start_inside) {
        row_count += x - last_x;
        if (!in_upper && !in_lower) {
          ++row_count;
        }
      }
      last_x = x;
    }
    return row_count;
  };

  int64_t total_count = 0;
  for (int i = 0; i < y_vals.size(); ++i) {
    if (i > 0 && y_vals[i] > y_vals[i - 1] + 1) {
      int64_t height = y_vals[i] - y_vals[i - 1] - 1;
      int y = y_vals[i] - 1;
      total_count += height * count_on(y);
    }
    int y = y_vals[i];
    for (const VerticalSegment& add : y_to_start[y]) {
      active.insert(add);
    }
    total_count += count_on(y);
    for (const VerticalSegment& add : y_to_end[y]) {
      active.erase(add);
    }
  }
  return total_count;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_18::Part1(
    absl::Span<std::string_view> input) const {
  std::vector<std::pair<Point, Point>> loop;
  Point cur = Cardinal::kOrigin;
  for (std::string_view line : input) {
    Tokenizer tok(line);
    std::string_view dir_str = tok.Next();
    ASSIGN_OR_RETURN(int dist, tok.NextInt());
    Point dir;
    if (dir_str == "R") {
      dir = Cardinal::kEast;
    } else if (dir_str == "U") {
      dir = Cardinal::kNorth;
    } else if (dir_str == "D") {
      dir = Cardinal::kSouth;
    } else if (dir_str == "L") {
      dir = Cardinal::kWest;
    } else {
      return Error("Bad dir");
    }
    if (dir == Cardinal::kEast || dir == Cardinal::kSouth) {
      loop.push_back({cur, cur + dir * dist});
    } else {
      loop.push_back({cur + dir * dist, cur});
    }
    cur = cur + dir * dist;
  }
  return AdventReturn(CountInterior(loop));
}

absl::StatusOr<std::string> Day_2023_18::Part2(
    absl::Span<std::string_view> input) const {
  VLOG(1) << "Start";
  std::vector<std::pair<Point, Point>> loop;
  Point cur = Cardinal::kOrigin;
  for (std::string_view str : input) {
    std::string_view hex_dist = str.substr(str.size() - 7, 5);
    int dist = 0;
    for (char c : hex_dist) {
      if (c >= '0' && c <= '9') {
        dist = dist * 16 + c - '0';
      } else if (c >= 'a' && c <= 'f') {
        dist = dist * 16 + c - 'a' + 10;
      } else {
        return Error("Bad hex: \"", hex_dist, "\"");
      }
    }
    Point dir;
    if (str[str.size() - 2] >= '0' && str[str.size() - 2] <= '3') {
      const std::array<Point, 4> kDirs = {Cardinal::kEast, Cardinal::kSouth, Cardinal::kWest, Cardinal::kNorth};
      dir = kDirs[str[str.size() - 2] - '0'];
    } else {
      return Error("Bad dir");
    }
    if (dir == Cardinal::kEast || dir == Cardinal::kSouth) {
      loop.push_back({cur, cur + dir * dist});
    } else {
      loop.push_back({cur + dir * dist, cur});
    }
    cur = cur + dir * dist;
  }
  return AdventReturn(CountInterior(loop));
}

}  // namespace advent_of_code
