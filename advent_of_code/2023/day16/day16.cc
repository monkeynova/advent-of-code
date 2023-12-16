// http://adventofcode.com/2023/day/16

#include "advent_of_code/2023/day16/day16.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

enum Dir {
  kNorth = 0,
  kSouth = 1,
  kWest = 2,
  kEast = 3,
};

// TODO(@monkeynova): A 'BoardPoint' API might make this a more re-usable
// way to improve CharBoard performance.
int FindEnergized(const ImmutableCharBoard& b, Point p, Dir d) {
  static const std::array<Dir, 4> kSlashLookup = {kEast, kWest, kSouth, kNorth};
  static const std::array<Dir, 4> kBackLookup = {kWest, kEast, kNorth, kSouth};

  const char* base = b.row(0).data();
  int stride = b.row(1).data() - base;

  int max_idx = b.width() * stride;

  std::vector<bool> hist(max_idx * 4, false);
  std::vector<bool> energized(max_idx, false);
  int count_energized = 0;

  auto on_board = [&](int idx) {
    if (idx < 0) return false;
    if (idx >= max_idx) return false;
    if (idx % stride == stride - 1) return false;
    return true;
  };

  int p_idx = p.y * stride + p.x;

  std::vector<std::pair<int, Dir>> queue = {{p_idx, d}};
  for (int i = 0; i < queue.size(); ++i) {
    auto [idx, d] = queue[i];
    while (on_board(idx)) {
      if (hist[idx * 4 + d]) break;
      hist[idx * 4 + d] = true;
      if (!energized[idx]) {
        energized[idx] = true;
        ++count_energized;
      }
      switch (base[idx]) {
        case '.': break;
        case '/': {
          d = kSlashLookup[d];
          break;
        }
        case '\\': {
          d = kBackLookup[d];
          break;
        }
        case '-': {
          if (d == kNorth || d == kSouth) {
            queue.push_back({idx + 1, kEast});
            d = kWest;
          }
          break;
        }
        case '|': {
          if (d == kEast || d == kWest) {
            queue.push_back({idx - stride, kNorth});
            d = kSouth;
          }
          break;
        }
        default: LOG(FATAL) << "Bad board: " << absl::string_view(&base[idx], 1);
      }
      switch (d) {
        case kNorth: idx -= stride; break;
        case kSouth: idx += stride; break;
        case kWest: --idx; break;
        case kEast: ++idx; break;
      }
    }
  }
  return count_energized;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(FindEnergized(b, {0, 0}, kEast));
}

absl::StatusOr<std::string> Day_2023_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  int max = 0;
  for (int x = 0; x < b.width(); ++x) {
    max = std::max<int>(max, FindEnergized(b, {x, 0}, kSouth));
    max = std::max<int>(max, FindEnergized(b, {x, b.height() - 1}, kNorth));
  }
  for (int y = 0; y < b.height(); ++y) {
    max = std::max<int>(max, FindEnergized(b, {0, y}, kEast));
    max = std::max<int>(max, FindEnergized(b, {b.width() - 1, y}, kWest));
  }
  return AdventReturn(max);
}

}  // namespace advent_of_code
