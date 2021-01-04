#include "advent_of_code/2018/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Map {
 public:
  Map(int depth, Point target) : depth_(depth), target_(target) {}

  CharBoard ToCharBoard() {
    CharBoard ret(target_.x + 1, target_.y + 1);
    for (Point p : ret.range()) {
      int el = ErosionLevel(p);
      switch (el % 3) {
        case 0: ret[p] = '.'; break;
        case 1: ret[p] = '='; break;
        case 2: ret[p] = '|'; break;
        default: LOG(FATAL) << "Bad mod";
      }
    }
    return ret;
  }

  int GeologicIndex(Point p) {
    if (p == Point{0, 0}) return 0;
    if (p == target_) return 0;
    if (p.y == 0) return p.x * 16807;
    if (p.x == 0) return p.y * 48271;
    return ErosionLevel(p + Cardinal::kWest) * ErosionLevel(p + Cardinal::kNorth);
  }

  int ErosionLevel(Point p) {
    if (auto it = erosion_.find(p); it != erosion_.end()) {
      return it->second;
    }
    int ret = (GeologicIndex(p) + depth_) % 20183;
    erosion_.emplace(p, ret);
    return ret;
  }

 private:
  int depth_;
  Point target_;
  absl::flat_hash_map<Point, int> erosion_;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day22_2018::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad size");
  int depth;
  if (!RE2::FullMatch(input[0], "depth: (\\d+)", &depth)) {
    return Error("No depth");
  }
  Point target;
  if (!RE2::FullMatch(input[1], "target: (\\d+),(\\d+)", &target.x, &target.y)) {
    return Error("No target");
  }
  Map map(depth, target);
  CharBoard b = map.ToCharBoard();
  VLOG(1) << "Map:\n" << b;
  int risk = 0;
  for (Point p : b.range()) {
   if (b[p] == '=') ++risk;
   if (b[p] == '|') risk += 2;
  }
  return IntReturn(risk);
}

absl::StatusOr<std::vector<std::string>> Day22_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
