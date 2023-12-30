// http://adventofcode.com/2023/day/16

#include "advent_of_code/2023/day16/day16.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

// TODO(@monkeynova): 'PointDirExtraMap' is an ugly hack. Find a better way.
using SegmentMap = FastBoard::PointDirExtraMap<FastBoard::Point>;

SegmentMap FindSegments(const ImmutableCharBoard& b) {
  FastBoard fb(b);

  const FastBoard::Point kBadPoint = fb.From({-1, -1});
  SegmentMap ret(fb, kBadPoint);

  for (int y = 0; y < b.height(); ++y) {
    FastBoard::Point left = fb.From({-1, y});
    FastBoard::Point right = fb.Add(left, FastBoard::kEast);
    for (int x = 0; x < b.width(); ++x) {
      if (fb[right] != '.') {
        ret.Set({left, FastBoard::kEast}, right);
        ret.Set({right, FastBoard::kWest}, left);
        left = right;
      }
      right = fb.Add(right, FastBoard::kEast);
    }
    ret.Set({left, FastBoard::kEast}, right);
    ret.Set({right, FastBoard::kWest}, left);
    left = fb.Add(left, FastBoard::kSouth);
  }

  for (int x = 0; x < b.width(); ++x) {
    FastBoard::Point top = fb.From({x, -1});
    FastBoard::Point bottom = fb.Add(top, FastBoard::kSouth);
    for (int y = 0; y < b.height(); ++y) {
      if (fb[bottom] != '.') {
        ret.Set({top, FastBoard::kSouth}, bottom);
        ret.Set({bottom, FastBoard::kNorth}, top);
        top = bottom;
      }
      bottom = fb.Add(bottom, FastBoard::kSouth);
    }
    ret.Set({top, FastBoard::kSouth}, bottom);
    ret.Set({bottom, FastBoard::kNorth}, top);
  }
  return ret;
}

int FindEnergized(const ImmutableCharBoard& b, const SegmentMap& segments,
                  Point p, FastBoard::Dir d) {
  static const std::array<FastBoard::Dir, 4> kSlashLookup = {
      FastBoard::kEast, FastBoard::kWest, FastBoard::kSouth, FastBoard::kNorth};
  static const std::array<FastBoard::Dir, 4> kBackLookup = {
      FastBoard::kWest, FastBoard::kEast, FastBoard::kNorth, FastBoard::kSouth};

  FastBoard fb(b);
  FastBoard::PointDirMap<bool> hist(fb, false);
  FastBoard::PointMap<bool> energized(fb, false);
  int count_energized = 0;

  FastBoard::Point start = fb.From(p);
  std::vector<FastBoard::PointDir> queue;
  {
    // Unroll the first segment, and we can avoid doing a number of (admittedly
    // very predictable) conditionals.
    FastBoard::PointDir pd = {.p = start, .d = d};
    FastBoard::Point end = segments.Get(pd);
    if (pd.p != end) {
      for (pd.Move(fb); pd.p != end; pd.Move(fb)) {
        if (!energized.Get(pd.p)) {
          energized.Set(pd.p, true);
          ++count_energized;
        }
      }
    }
    if (!fb.OnBoard(pd.p)) {
      return count_energized;
    }
    switch (fb[pd.p]) {
      case '/': {
        pd.d = kSlashLookup[pd.d];
        break;
      }
      case '\\': {
        pd.d = kBackLookup[pd.d];
        break;
      }
      case '|': {
        if (pd.d == FastBoard::kWest || pd.d == FastBoard::kEast) {
          pd.d = FastBoard::kNorth;
          queue.push_back({.p = pd.p, .d = FastBoard::kSouth});
        }
        break;
      }
      case '-': {
        if (pd.d == FastBoard::kNorth || pd.d == FastBoard::kSouth) {
          pd.d = FastBoard::kWest;
          queue.push_back({.p = pd.p, .d = FastBoard::kEast});
        }
        break;
      }
    }
    queue.push_back(pd);
  }

  for (int i = 0; i < queue.size(); ++i) {
    FastBoard::PointDir pd = queue[i];
    while (true) {
      if (hist.Get(pd)) break;
      hist.Set(pd, true);
      for (FastBoard::Point end = segments.Get(pd); pd.p != end; pd.Move(fb)) {
        if (!energized.Get(pd.p)) {
          energized.Set(pd.p, true);
          ++count_energized;
        }
      }
      if (!fb.OnBoard(pd.p)) break;
      switch (fb[pd.p]) {
        case '/': {
          pd.d = kSlashLookup[pd.d];
          break;
        }
        case '\\': {
          pd.d = kBackLookup[pd.d];
          break;
        }
        case '|': {
          if (pd.d == FastBoard::kWest || pd.d == FastBoard::kEast) {
            pd.d = FastBoard::kNorth;
            queue.push_back({.p = pd.p, .d = FastBoard::kSouth});
          }
          break;
        }
        case '-': {
          if (pd.d == FastBoard::kNorth || pd.d == FastBoard::kSouth) {
            pd.d = FastBoard::kWest;
            queue.push_back({.p = pd.p, .d = FastBoard::kEast});
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
  return AdventReturn(FindEnergized(b, segments, {-1, 0}, FastBoard::kEast));
}

absl::StatusOr<std::string> Day_2023_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  // TODO(@monkeynova): There might be a way, rather than casting every ray, to
  // build up a map of "point+dir -> subsequent activation" that could be
  // constructed with Memo/Dynamic programming.
  SegmentMap segments = FindSegments(b);
  int max = 0;
  for (int x = 0; x < b.width(); ++x) {
    max = std::max(max, FindEnergized(b, segments, {x, -1}, FastBoard::kSouth));
    max = std::max(
        max, FindEnergized(b, segments, {x, b.height()}, FastBoard::kNorth));
  }
  for (int y = 0; y < b.height(); ++y) {
    max = std::max(max, FindEnergized(b, segments, {-1, y}, FastBoard::kEast));
    max = std::max(
        max, FindEnergized(b, segments, {b.width(), y}, FastBoard::kWest));
  }
  return AdventReturn(max);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/16,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_16()); });

}  // namespace advent_of_code
