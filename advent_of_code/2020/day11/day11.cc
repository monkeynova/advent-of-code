#include "advent_of_code/2020/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

CharBoard Update(CharBoard in) {
  CharBoard out(in);
  for (Point c : in.range()) {
    int neighbors = 0;
    for (Point dir : Cardinal::kEightDirs) {
      Point n = c + dir;
      if (!in.OnBoard(n)) continue;
      if (in.at(n) == '#') ++neighbors;
    }
    if (in.at(c) == 'L') {
      if (neighbors == 0) out.set(c, '#');
    } else if (in.at(c) == '#') {
      if (neighbors >= 4) out.set(c, 'L');
    }
  }
  return out;
}

int CountSeats(CharBoard b) {
  int count = 0;
  for (Point c : b.range()) {
    if (b.at(c) == '#') ++count;
  }
  return count;
}

using VisMap = absl::flat_hash_map<std::pair<Point, Point>, Point>;

VisMap ComputeVismap(CharBoard in) {
  VisMap vis_map;
  for (Point c : in.range()) {
    if (in.at(c) == '.') continue;
    for (Point dir : Cardinal::kEightDirs) {
      Point n = c + dir;
      while (in.OnBoard(n)) {
        if (in.at(n) != '.') {
          vis_map.emplace(std::make_pair(c, dir), n);
          break;
        }
        n += dir;
      }
    }
  }
  return vis_map;
}

CharBoard Update2(CharBoard in, const VisMap& vis_map) {
  CharBoard out = in;
  for (Point c : in.range()) {
    int neighbors = 0;
    if (in.at(c) != '.') {
      for (Point dir : Cardinal::kEightDirs) {
        auto it = vis_map.find(std::make_pair(c, dir));
        if (it != vis_map.end()) {
          Point d = it->second;
          if (in.at(d) == '#') ++neighbors;
        }
      }
    }
    if (in.at(c) == 'L') {
      if (neighbors == 0) out.set(c, '#');
    } else if (in.at(c) == '#') {
      if (neighbors >= 5) out.set(c, 'L');
    }
  }
  return out;
}

absl::StatusOr<std::vector<std::string>> Day11_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<CharBoard> parsed = CharBoard::Parse(input);
  if (!parsed.ok()) return parsed.status();

  CharBoard cur = *parsed;
  for (int i = 0;; ++i) {
    VLOG(1) << "Board:\n" << cur << "\n";
    CharBoard next = Update(cur);
    if (next == cur) break;
    cur = next;
  }
  return IntReturn(CountSeats(cur));
}

absl::StatusOr<std::vector<std::string>> Day11_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<CharBoard> parsed = CharBoard::Parse(input);
  if (!parsed.ok()) return parsed.status();

  CharBoard cur = *parsed;
  VisMap vis_map = ComputeVismap(cur);
  for (int i = 0;; ++i) {
    VLOG(1) << "Board:\n" << cur << "\n";
    CharBoard next = Update2(cur, vis_map);
    if (next == cur) break;
    cur = next;
  }
  return IntReturn(CountSeats(cur));
}
