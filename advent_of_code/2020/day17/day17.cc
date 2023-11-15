#include "advent_of_code/2020/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point4.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

// Conway?

class Part1Conway : public ConwaySet<Point3> {
 public:
  Part1Conway(const CharBoard& char_board) : ConwaySet(ToSet(char_board)) {}

  std::vector<Point3> Neighbors(const Point3& p) const override {
    std::vector<Point3> ret(Cardinal3::kNeighborDirs.begin(),
                            Cardinal3::kNeighborDirs.end());
    for (Point3& r : ret) r += p;
    return ret;
  }

  bool IsLive(bool is_alive, int neighbors) const override {
    if (neighbors == 3) return true;
    if (is_alive && neighbors == 2) return true;
    return false;
  }

 private:
  static absl::flat_hash_set<Point3> ToSet(const CharBoard& char_board) {
    absl::flat_hash_set<Point3> set;
    for (Point p : char_board.Find('#')) {
      set.insert(Point3{p.x, p.y, 0});
    }
    return set;
  }
};

class Part2Conway : public ConwaySet<Point4> {
 public:
  Part2Conway(const CharBoard& char_board) : ConwaySet(ToSet(char_board)) {}

  std::vector<Point4> Neighbors(const Point4& p) const override {
    std::vector<Point4> ret(Cardinal4::kNeighborDirs.begin(),
                            Cardinal4::kNeighborDirs.end());
    for (Point4& r : ret) r += p;
    return ret;
  }

  bool IsLive(bool is_alive, int neighbors) const override {
    if (neighbors == 3) return true;
    if (is_alive && neighbors == 2) return true;
    return false;
  }

 private:
  static absl::flat_hash_set<Point4> ToSet(const CharBoard& char_board) {
    absl::flat_hash_set<Point4> set;
    for (Point p : char_board.Find('#')) {
      set.insert(Point4{p.x, p.y, 0, 0});
    }
    return set;
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2020_17::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> initial = CharBoard::Parse(input);
  if (!initial.ok()) return initial.status();

  Part1Conway conway(*initial);
  conway.AdvanceN(6);
  return AdventReturn(conway.CountLive());
}

absl::StatusOr<std::string> Day_2020_17::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> initial = CharBoard::Parse(input);
  if (!initial.ok()) return initial.status();

  Part2Conway conway(*initial);
  conway.AdvanceN(6);
  return AdventReturn(conway.CountLive());
}

}  // namespace advent_of_code
