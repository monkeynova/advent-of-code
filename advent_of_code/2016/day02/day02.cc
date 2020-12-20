#include "advent_of_code/2016/day02/day02.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day02_2016::Part1(
    absl::Span<absl::string_view> input) const {
  std::string code;
  Point p = {1, 1};
  absl::flat_hash_map<Point, std::string> codes = {
    {{0,0}, "1"}, {{1, 0}, "2"}, {{2, 0}, "3"},
    {{0,1}, "4"}, {{1, 1}, "5"}, {{2, 1}, "6"},
    {{0,2}, "7"}, {{1, 2}, "8"}, {{2, 2}, "9"},
  };
  for (absl::string_view ins : input) {
    for (char c : ins) {
      Point dir;
      switch (c) {
        case 'U': dir = Cardinal::kNorth; break;
        case 'D': dir = Cardinal::kSouth; break;
        case 'L': dir = Cardinal::kWest; break;
        case 'R': dir = Cardinal::kEast; break;
        default: return Error("Bad instruction: ", ins);
      }
      Point next = p + dir;
      if (next.x >= 0 && next.y >= 0 && next.x <= 2 && next.y <= 2) {
        p = next;
      }
    }
    auto it = codes.find(p);
    if (it == codes.end()) return Error("Bad point: ", p.DebugString());
    code += it->second;
  }
  return std::vector<std::string>{code};
}

absl::StatusOr<std::vector<std::string>> Day02_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
