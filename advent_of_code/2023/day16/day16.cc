// http://adventofcode.com/2023/day/16

#include "advent_of_code/2023/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
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

// TODO(@monkeynova): A 'BoardPoint' API might make this a more re-usable way
// to improve CharBoard performance.
int FindEnergized(const ImmutableCharBoard& b, Point p, Dir d) {
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

using SegmentMap = absl::flat_hash_map<std::pair<Point, Point>, Point>;
SegmentMap FindSegments(const ImmutableCharBoard& b) {
  SegmentMap ret;
  for (int y = 0; y < b.height(); ++y) {
    Point left = {-1, y};
    for (int x = 0; x < b.width(); ++x) {
      if (b[{x, y}] != '.') {
        Point right = {x, y};
        ret.insert({{left, Cardinal::kEast}, right});
        ret.insert({{right, Cardinal::kWest}, left});
        left = right;
      }
    }
    Point right = {b.width(), y};
    ret.insert({{left, Cardinal::kEast}, right});
    ret.insert({{right, Cardinal::kWest}, left});
  }
  for (int x = 0; x < b.width(); ++x) {
    Point top = {x, -1};
    for (int y = 0; y < b.height(); ++y) {
      if (b[{x, y}] != '.') {
        Point bottom = {x, y};
        ret.insert({{top, Cardinal::kSouth}, bottom});
        ret.insert({{bottom, Cardinal::kNorth}, top});
        top = bottom;
      }
    }
    Point bottom = {x, b.height()};
    ret.insert({{top, Cardinal::kSouth}, bottom});
    ret.insert({{bottom, Cardinal::kNorth}, top});
  }
  return ret;
}

// TODO(@monkeynova): A 'BoardPoint' API might make this a more re-usable way
// to improve CharBoard performance.
int FindEnergized(const ImmutableCharBoard& b, const SegmentMap& segments, Point p, Point d) {
  std::vector<std::pair<Point, Point>> queue = {{p, d}};
  absl::flat_hash_set<Point> energized;
  absl::flat_hash_set<std::pair<Point, Point>> hist;
  for (int i = 0; i < queue.size(); ++i) {
    std::pair<Point, Point> p_and_d = queue[i];
    while (hist.insert(p_and_d).second) {
      auto it = segments.find(p_and_d);
      CHECK(it != segments.end());
      for (Point p = it->first.first; p != it->second; p += it->first.second) {
        if (b.OnBoard(p)) {
          energized.insert(p);
        }
      }
      p_and_d.first = it->second;
      if (!b.OnBoard(p_and_d.first)) break;
      switch (b[p_and_d.first]) {
        case '/': {
          if (p_and_d.second == Cardinal::kNorth) p_and_d.second = Cardinal::kEast;
          else if (p_and_d.second == Cardinal::kSouth) p_and_d.second = Cardinal::kWest;
          else if (p_and_d.second == Cardinal::kWest) p_and_d.second = Cardinal::kSouth;
          else if (p_and_d.second == Cardinal::kEast) p_and_d.second = Cardinal::kNorth;
          break;
        }
        case '\\': {
          if (p_and_d.second == Cardinal::kNorth) p_and_d.second = Cardinal::kWest;
          else if (p_and_d.second == Cardinal::kSouth) p_and_d.second = Cardinal::kEast;
          else if (p_and_d.second == Cardinal::kWest) p_and_d.second = Cardinal::kNorth;
          else if (p_and_d.second == Cardinal::kEast) p_and_d.second = Cardinal::kSouth;
          break;
        }
        case '|': {
          if (p_and_d.second == Cardinal::kWest || p_and_d.second == Cardinal::kEast) {
            p_and_d.second = Cardinal::kNorth;
            queue.push_back({p_and_d.first, Cardinal::kSouth});
          }
          break;
        }
        case '-': {
          if (p_and_d.second == Cardinal::kNorth || p_and_d.second == Cardinal::kSouth) {
            p_and_d.second = Cardinal::kWest;
            queue.push_back({p_and_d.first, Cardinal::kEast});
          }
          break;
        }
        default: LOG(FATAL) << "Bad segment";
      }
    }
  }
  return energized.size();
}

}  // namespace

#define USE_SEGMENTS 0

absl::StatusOr<std::string> Day_2023_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
#if USE_SEGMENTS
  absl::flat_hash_map<std::pair<Point, Point>, Point> segments = FindSegments(b);
  return AdventReturn(FindEnergized(b, segments, {-1, 0}, Cardinal::kEast));
#else
  return AdventReturn(FindEnergized(b, {0, 0}, kEast));
#endif
}

absl::StatusOr<std::string> Day_2023_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
#if USE_SEGMENTS
  absl::flat_hash_map<std::pair<Point, Point>, Point> segments = FindSegments(b);
  int max = 0;
  for (int x = 0; x < b.width(); ++x) {
    max = std::max(max, FindEnergized(b, segments, {x, -1}, Cardinal::kSouth));
    max = std::max(max, FindEnergized(b, segments, {x, b.height()}, Cardinal::kNorth));
  }
  for (int y = 0; y < b.height(); ++y) {
    max = std::max(max, FindEnergized(b, segments, {-1, y}, Cardinal::kEast));
    max = std::max(max, FindEnergized(b, segments, {b.width(), y}, Cardinal::kWest));
  }
  return AdventReturn(max);
#else
  int max = 0;
  for (int x = 0; x < b.width(); ++x) {
    max = std::max(max, FindEnergized(b, {x, 0}, kSouth));
    max = std::max(max, FindEnergized(b, {x, b.height() - 1}, kNorth));
  }
  for (int y = 0; y < b.height(); ++y) {
    max = std::max(max, FindEnergized(b, {0, y}, kEast));
    max = std::max(max, FindEnergized(b, {b.width() - 1, y}, kWest));
  }
  return AdventReturn(max);
#endif
}

}  // namespace advent_of_code
