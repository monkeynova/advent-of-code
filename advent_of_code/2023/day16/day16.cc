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

using SegmentMap = std::vector<int>;
SegmentMap FindSegments(const ImmutableCharBoard& b) {
  const char* base = b.row(0).data();
  int stride = b.row(1).data() - base;
  CHECK_EQ(stride, b.width() + 1);
  
  SegmentMap ret((b.height() + 2) * stride * 4, -1);
  int segment_off = stride;

  for (int y = 0; y < b.height(); ++y) {
    int left_idx = y * stride + -1;
    int right_idx = left_idx + 1;
    for (int x = 0; x < b.width(); ++x, ++right_idx) {
      if (base[right_idx] != '.') {
        ret[left_idx * 4 + kEast + stride] = right_idx;
        ret[right_idx * 4 + kWest + stride] = left_idx;
        left_idx = right_idx;
      }
    }
    ret[left_idx * 4 + kEast + stride] = right_idx;
    ret[right_idx * 4 + kWest + stride] = left_idx;
  }
  for (int x = 0; x < b.width(); ++x) {
    int top_idx = -1 * stride + x;
    int bottom_idx = top_idx + stride;
    for (int y = 0; y < b.height(); ++y, bottom_idx += stride) {
      if (base[bottom_idx] != '.') {
        ret[top_idx * 4 + kSouth + stride] = bottom_idx;
        ret[bottom_idx * 4 + kNorth + stride] = top_idx;
        top_idx = bottom_idx;
      }
    }
    ret[top_idx * 4 + kSouth + stride] = bottom_idx;
    ret[bottom_idx * 4 + kNorth + stride] = top_idx;
  }
  return ret;
}

// TODO(@monkeynova): A 'BoardPoint' API might make this a more re-usable way
// to improve CharBoard performance.
int FindEnergized(const ImmutableCharBoard& b, const SegmentMap& segments, Point p, Dir d) {
  static const std::array<Dir, 4> kSlashLookup = {kEast, kWest, kSouth,
                                                  kNorth};
  static const std::array<Dir, 4> kBackLookup = {kWest, kEast, kNorth, kSouth};

  const char* base = b.row(0).data();
  int stride = b.row(1).data() - base;

  int max_idx = b.width() * stride;

  std::vector<bool> hist(max_idx * 4, false);
  std::vector<bool> energized(max_idx, false);
  int count_energized = 0;

  std::vector<bool> on_board_lookup(max_idx, true);
  for (int i = stride - 1; i < max_idx; i += stride) {
    on_board_lookup[i] = false;
  }
  auto on_board = [&](int idx) -> bool {
    if (idx < 0) return false;
    if (idx >= max_idx) return false;
    return on_board_lookup[idx];
  };

  int idx = p.y * stride + p.x;
  std::vector<std::pair<int, Dir>> queue = {{idx, d}};

  for (int i = 0; i < queue.size(); ++i) {
    std::pair<int, Dir> idx_and_d = queue[i];
    while (true) {
      if (on_board(idx_and_d.first)) {
        if (hist[idx_and_d.first * 4 + idx_and_d.second]) break;
        hist[idx_and_d.first * 4 + idx_and_d.second] = true;
      }
      int end_idx = segments[idx_and_d.first * 4 + idx_and_d.second + stride];
      CHECK_NE(end_idx, -1);
      int delta = std::array<int, 4>{-stride, stride, -1, +1}[idx_and_d.second];
      for (int idx2 = idx_and_d.first; idx2 != end_idx; idx2 += delta) {
        if (idx2 != idx) {
          if (!energized[idx2]) {
            energized[idx2] = true;
            ++count_energized;
          }
        }
      }
      if (!on_board(end_idx)) break;
      idx_and_d.first = end_idx;
      switch (base[idx_and_d.first]) {
        case '/': {
          idx_and_d.second = kSlashLookup[idx_and_d.second];
          break;
        }
        case '\\': {
          idx_and_d.second = kBackLookup[idx_and_d.second];
          break;
        }
        case '|': {
          if (idx_and_d.second == kWest || idx_and_d.second == kEast) {
            idx_and_d.second = kNorth;
            queue.push_back({idx_and_d.first, kSouth});
          }
          break;
        }
        case '-': {
          if (idx_and_d.second == kNorth || idx_and_d.second == kSouth) {
            idx_and_d.second = kWest;
            queue.push_back({idx_and_d.first, kEast});
          }
          break;
        }
      }
    }
  }
  return count_energized;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  SegmentMap segments = FindSegments(b);
  return AdventReturn(FindEnergized(b, segments, {-1, 0}, kEast));
}

absl::StatusOr<std::string> Day_2023_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  SegmentMap segments = FindSegments(b);
  int max = 0;
  for (int x = 0; x < b.width(); ++x) {
    max = std::max(max, FindEnergized(b, segments, {x, -1}, kSouth));
    max = std::max(max, FindEnergized(b, segments, {x, b.height()}, kNorth));
  }
  for (int y = 0; y < b.height(); ++y) {
    max = std::max(max, FindEnergized(b, segments, {-1, y}, kEast));
    max = std::max(max, FindEnergized(b, segments, {b.width(), y}, kWest));
  }
  return AdventReturn(max);
}

}  // namespace advent_of_code
