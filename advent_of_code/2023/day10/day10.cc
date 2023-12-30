// http://adventofcode.com/2023/day/10

#include "advent_of_code/2023/day10/day10.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

char StartType(Point orig_dir, Point dir) {
  if (orig_dir == Cardinal::kEast && dir == Cardinal::kEast)
    return '-';
  else if (orig_dir == Cardinal::kWest && dir == Cardinal::kWest)
    return '-';
  else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kSouth)
    return '|';
  else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kNorth)
    return '|';
  else if (orig_dir == Cardinal::kWest && dir == Cardinal::kNorth)
    return '7';
  else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kEast)
    return '7';
  else if (orig_dir == Cardinal::kWest && dir == Cardinal::kSouth)
    return 'J';
  else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kEast)
    return 'J';
  else if (orig_dir == Cardinal::kNorth && dir == Cardinal::kWest)
    return 'L';
  else if (orig_dir == Cardinal::kEast && dir == Cardinal::kSouth)
    return 'L';
  else if (orig_dir == Cardinal::kEast && dir == Cardinal::kNorth)
    return 'F';
  else if (orig_dir == Cardinal::kSouth && dir == Cardinal::kWest)
    return 'F';
  return '?';
}

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

struct XAndC {
  int16_t x;
  char c;

  bool operator<(const XAndC& o) const { return x < o.x; }
};

std::vector<std::vector<XAndC>> FindLoop(const ImmutableCharBoard& b,
                                         Point start) {
  CHECK_LT(b.width(), std::numeric_limits<int16_t>::max());
  for (Point dir : Cardinal::kFourDirs) {
    Point orig_dir = dir;
    std::vector<std::vector<XAndC>> ret(b.height());
    for (Point test = dir + start; b.OnBoard(test); test += dir) {
      if (test == start) {
        ret[test.y].push_back(
            {.x = static_cast<int16_t>(test.x), .c = StartType(orig_dir, dir)});
        return ret;
      }
      char c = b[test];
      ret[test.y].push_back({.x = static_cast<int16_t>(test.x), .c = c});
      dir = AdjustDir(dir, c);
      if (dir == Cardinal::kOrigin) break;
    }
  }
  return {};
}

// TODO(@monkeynova): 2023/18 has a different approach here that might work
// better.
absl::StatusOr<int> InsideSpace(const ImmutableCharBoard& b,
                                std::vector<std::vector<XAndC>> loop) {
  int count_inside = 0;
  // Maps from one inside_state which is two bits of 'inside' for the upper and
  // lower halves of the cell to another.
  // That is insde_state = (upper_inside << 1) | lower_inside and
  // kInsideTransition describes how that changed with a given char on the
  // board.
  static const std::array<std::array<char, 4>, 128> kInsideTransition = []() {
    constexpr char kBad = static_cast<char>(-1);
    std::array<std::array<char, 4>, 128> ret;
    for (int i = 0; i < ret.size(); ++i) {
      ret[i] = std::array<char, 4>{kBad, kBad, kBad, kBad};
    }
    ret['|'] = std::array<char, 4>{3, kBad, kBad, 0};
    ret['-'] = std::array<char, 4>{0, 1, 2, 3};
    ret['J'] = std::array<char, 4>{0, 3, 0, 3};
    ret['7'] = std::array<char, 4>{0, 0, 3, 3};
    ret['L'] = std::array<char, 4>{2, kBad, kBad, 1};
    ret['F'] = std::array<char, 4>{1, kBad, kBad, 2};
    return ret;
  }();

  for (int y = 0; y < loop.size(); ++y) {
    if (loop[y].empty()) continue;
    absl::c_sort(loop[y]);
    int inside_state = 0;
    bool prev_inside = false;
    for (int i = 0; i < loop[y].size(); ++i) {
      auto [x, c] = loop[y][i];
      inside_state = kInsideTransition[c][inside_state];
      if (inside_state == -1) return Error("Bad transition");
      if (inside_state != 3) {
        if (prev_inside) {
          count_inside += x - loop[y][i - 1].x - 1;
        }
        prev_inside = false;
      } else {
        prev_inside = true;
      }
    }
  }
  return count_inside;
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
  std::vector<std::vector<XAndC>> loop = FindLoop(b, start);
  if (loop.empty()) return Error("No loop found");
  return AdventReturn(InsideSpace(b, std::move(loop)));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/10,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_10()); });

}  // namespace advent_of_code
