// http://adventofcode.com/2023/day/17

#include "advent_of_code/2023/day17/day17.h"

#include <queue>

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"

namespace advent_of_code {

namespace {

int MinCartPath(const ImmutableCharBoard& b, int min, int max) {
  FastBoard fb(b);
  // TODO(@monkeynova): heat_map doesn't need to store keys for both North and South...
  FastBoard::PointHalfDirMap<int> heat_map(fb, std::numeric_limits<int>::max());
  FastBoard::PointDirMap<bool> in_queue(fb, false);

  struct HeatMapCmp {
    explicit HeatMapCmp(FastBoard::PointHalfDirMap<int>& heat_map) : heat_map_(heat_map) {}

    bool operator()(FastBoard::PointDir a, FastBoard::PointDir b) {
      return heat_map_.Get(b) < heat_map_.Get(a);
    }

    FastBoard::PointHalfDirMap<int>& heat_map_;
  };
  using QueueType = std::priority_queue<FastBoard::PointDir, std::vector<FastBoard::PointDir>, HeatMapCmp>;
  HeatMapCmp cmp(heat_map);
  QueueType queue(cmp);

  auto add_range = [&](FastBoard::PointDir pd, FastBoard::Dir dir) {
    int heat = heat_map.Get(pd);
    CHECK_NE(heat, std::numeric_limits<int>::max());
    CHECK(pd.d != dir);
    pd.d = dir;
    for (int i = 0; i < max; ++i) {
      if (!pd.MoveAndCheckBoard(fb)) break;
      heat += fb[pd.p] - '0';
      if (i + 1 < min) continue;
  
      if (heat < heat_map.Get(pd)) {
        heat_map.Set(pd, heat);
        if (!in_queue.Get(pd)) {
          in_queue.Set(pd, true);
          queue.push(pd); 
        }
      }
    }
  };

  FastBoard::PointDir start = {.p = fb.From({0, 0}), .d = FastBoard::kNorth};
  for (FastBoard::Dir d : {FastBoard::kEast, FastBoard::kSouth}) {
    heat_map.Set(start, 0);
    add_range(start, d);
  }
  int dequeued = 0;
  for (;!queue.empty(); queue.pop()) {
    ++dequeued;
    FastBoard::PointDir cur = queue.top();
    in_queue.Set(cur, false);
    add_range(cur, FastBoard::kRotateLeft[cur.d]);
    add_range(cur, FastBoard::kRotateRight[cur.d]);
  }
  VLOG(1) << dequeued << "/" << b.width() * b.height() << " ~ " << 1.0 * dequeued / (b.width() * b.height());
  FastBoard::Point end = fb.From({b.width() - 1, b.height() - 1});
  int answer = std::numeric_limits<int>::max();
  answer = std::min(answer, heat_map.Get({.p = end, .d = FastBoard::kEast}));
  answer = std::min(answer, heat_map.Get({.p = end, .d = FastBoard::kSouth}));
  return answer;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_17::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(MinCartPath(b, 1, 3));
}

absl::StatusOr<std::string> Day_2023_17::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(MinCartPath(b, 4, 10));
}

}  // namespace advent_of_code
