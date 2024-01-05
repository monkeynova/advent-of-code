// http://adventofcode.com/2023/day/10

#include "advent_of_code/2023/day10/day10.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

inline Point AdjustDir(Point dir, char c) {
  int point_idx = (dir.y + 1) * 3 + (dir.x + 1);
  static const std::array<std::array<Point, 9>, 128> kTransitionTable = []() {
    std::array<std::array<Point, 9>, 128> ret;
    for (int c = 0; c < 128; ++c) {
      ret[c] = {Cardinal::kOrigin, Cardinal::kOrigin, Cardinal::kOrigin,
                Cardinal::kOrigin, Cardinal::kOrigin, Cardinal::kOrigin,
                Cardinal::kOrigin, Cardinal::kOrigin, Cardinal::kOrigin};
    }
    constexpr int kNorthIdx = 1;
    constexpr int kWestIdx = 3;
    constexpr int kEastIdx = 5;
    constexpr int kSouthIdx = 7;

    ret['|'][kNorthIdx] = Cardinal::kNorth;
    ret['|'][kSouthIdx] = Cardinal::kSouth;
    ret['-'][kWestIdx] = Cardinal::kWest;
    ret['-'][kEastIdx] = Cardinal::kEast;
    ret['F'][kNorthIdx] = Cardinal::kEast;
    ret['F'][kWestIdx] = Cardinal::kSouth;
    ret['7'][kNorthIdx] = Cardinal::kWest;
    ret['7'][kEastIdx] = Cardinal::kSouth;
    ret['J'][kSouthIdx] = Cardinal::kWest;
    ret['J'][kEastIdx] = Cardinal::kNorth;
    ret['L'][kSouthIdx] = Cardinal::kEast;
    ret['L'][kWestIdx] = Cardinal::kNorth;
    return ret;
  }();
  return kTransitionTable[c][point_idx];
}

std::optional<int> FindLoopSize(const ImmutableCharBoard& b, Point start) {
  for (Point dir : Cardinal::kFourDirs) {
    int loop_size = 0;
    for (Point test = dir + start; b.OnBoard(test); test += dir) {
      ++loop_size;
      if (test == start) {
        return loop_size;
      }
      dir = AdjustDir(dir, b[test]);
      if (dir == Cardinal::kOrigin) break;
    }
  }
  return {};
}

std::vector<Point> FindLoop(const ImmutableCharBoard& b, Point start) {
  CHECK_LT(b.width(), std::numeric_limits<int16_t>::max());
  for (Point dir : Cardinal::kFourDirs) {
    std::vector<Point> ret = {start};
    for (Point test = dir + start; b.OnBoard(test); test += dir) {
      if (test == start) {
        ret.push_back(test);
        return ret;
      }
      char c = b[test];
      Point new_dir = AdjustDir(dir, c);
      if (new_dir != dir) {
        ret.push_back(test);
        dir = new_dir;
      }
      if (dir == Cardinal::kOrigin) break;
    }
  }
  return {};
}

}  // namespace

absl::StatusOr<std::string> Day_2023_10::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  std::optional<int> loop_size = FindLoopSize(b, start);
  if (!loop_size) return AdventReturn(loop_size);
  return AdventReturn((*loop_size + 1) / 2);
}

absl::StatusOr<std::string> Day_2023_10::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  std::vector<Point> loop = FindLoop(b, start);
  if (loop.empty()) return Error("No loop found");
  return AdventReturn(LoopArea(loop, /*counter_perimter=*/false));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/10,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_10()); });

}  // namespace advent_of_code
