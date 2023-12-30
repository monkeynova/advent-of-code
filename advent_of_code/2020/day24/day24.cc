#include "advent_of_code/2020/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class ConwayHex : public ConwaySet<Point> {
 public:
  ConwayHex(absl::flat_hash_set<Point> p) : ConwaySet(p) {}

  std::vector<Point> Neighbors(const Point& p) const override {
    static const std::vector<Point> kDirs = {
        2 * Cardinal::kWest,  2 * Cardinal::kEast,  Cardinal::kSouthWest,
        Cardinal::kSouthEast, Cardinal::kNorthWest, Cardinal::kNorthEast};
    std::vector<Point> ret = kDirs;
    for (Point& r : ret) r += p;
    return ret;
  }

  bool IsLive(bool is_live, int neighbors) const override {
    if (is_live) return neighbors == 1 || neighbors == 2;
    return neighbors == 2;
  }
};

absl::StatusOr<ConwayHex> ParseGrid(absl::Span<std::string_view> input) {
  absl::flat_hash_set<Point> grid;
  for (std::string_view str : input) {
    Point p = {0, 0};
    for (int i = 0; i < str.size(); ++i) {
      if (str[i] == 'w') {
        p += 2 * Cardinal::kWest;
      } else if (str[i] == 'e') {
        p += 2 * Cardinal::kEast;
      } else if (str[i] == 'n') {
        if (i + 1 == str.size()) return Error("Bad sequence (@", i, "): ", str);
        if (str[i + 1] == 'w') {
          p += Cardinal::kNorthWest;
        } else if (str[i + 1] == 'e') {
          p += Cardinal::kNorthEast;
        } else {
          return Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else if (str[i] == 's') {
        if (i + 1 == str.size()) return Error("Bad sequence (@", i, "): ", str);
        if (str[i + 1] == 'w') {
          p += Cardinal::kSouthWest;
        } else if (str[i + 1] == 'e') {
          p += Cardinal::kSouthEast;
        } else {
          return Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else {
        return Error("Bad sequence (@", i, "): ", str);
      }
    }
    auto [it, inserted] = grid.insert(p);
    if (!inserted) grid.erase(p);
  }
  return ConwayHex(std::move(grid));
}

}  // namespace

absl::StatusOr<std::string> Day_2020_24::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ConwayHex grid, ParseGrid(input));

  return AdventReturn(grid.CountLive());
}

absl::StatusOr<std::string> Day_2020_24::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ConwayHex grid, ParseGrid(input));

  for (int i = 0; i < 100; ++i) {
    grid.Advance();
  }

  return AdventReturn(grid.CountLive());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/24,
    []() { return std::unique_ptr<AdventDay>(new Day_2020_24()); });

}  // namespace advent_of_code
