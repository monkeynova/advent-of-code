// http://adventofcode.com/2022/day/17

#include "advent_of_code/2022/day17/day17.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class DropState {
 public:
  struct Rock {
    Point off;
    std::vector<Point> delta;
  };

  struct SummaryState {
    int w_idx;
    int r_idx;
    std::vector<Point> bounds;

    CharBoard Draw() const;

    bool operator==(const SummaryState& o) const {
      return w_idx == o.w_idx && r_idx == o.r_idx && bounds == o.bounds;
    }

    template <typename H>
    friend H AbslHashValue(H h, const SummaryState& s) {
      return H::combine(std::move(h), s.w_idx, s.r_idx, s.bounds);
    }
  };

  DropState() = default;

  int height() const { return height_; }

  absl::Status ParseWind(absl::string_view wind);
  absl::Status DropNextRock(int i);
  SummaryState Summarize() const;

 private:
  CharBoard Draw(int height) const;
  CharBoard AddPiece(CharBoard b, Rock r) const;

  absl::Status InsertRock(const Rock& r);
  bool TryMove(Rock& r, Point d) {
    for (Point p : r.delta) {
      Point t = p + r.off + d;
      if (!kBounds.Contains(t)) return false;
      if (stopped_.contains(t)) return false;
    }
    r.off += d;
    return true;
  }

  static const std::vector<Rock> kRockTypes;
  static const PointRectangle kBounds;

  std::vector<Point> wind_;
  int w_idx_ = -1;
  int r_idx_ = -1;
  int height_ = 0;
  absl::flat_hash_set<Point> stopped_;
};

const PointRectangle DropState::kBounds = {
    {0, 0}, {6, std::numeric_limits<int>::max()}};

const std::vector<DropState::Rock> DropState::kRockTypes = {
    Rock{.off = {0, 0}, .delta = {{0, 0}, {1, 0}, {2, 0}, {3, 0}}},
    Rock{.off = {0, 0}, .delta = {{2, 1}, {1, 0}, {1, 1}, {1, 2}, {0, 1}}},
    Rock{.off = {0, 0}, .delta = {{2, 2}, {2, 1}, {0, 0}, {1, 0}, {2, 0}}},
    Rock{.off = {0, 0}, .delta = {{0, 3}, {0, 2}, {0, 1}, {0, 0}}},
    Rock{.off = {0, 0}, .delta = {{1, 0}, {1, 1}, {0, 0}, {0, 1}}},
};

absl::Status DropState::ParseWind(absl::string_view wind_str) {
  for (char c : wind_str) {
    if (c == '<')
      wind_.push_back(Cardinal::kWest);
    else if (c == '>')
      wind_.push_back(Cardinal::kEast);
    else
      return Error("Bad wind");
  }
  return absl::OkStatus();
}

absl::Status DropState::InsertRock(const Rock& r) {
  for (Point p : r.delta) {
    Point t = p + r.off;
    if (!stopped_.insert(t).second) return Error("Double insert!");
    height_ = std::max(height_, t.y + 1);
  }
  return absl::OkStatus();
}

absl::Status DropState::DropNextRock(int i) {
  r_idx_ = (r_idx_ + 1) % kRockTypes.size();
  Rock drop = kRockTypes[r_idx_];
  drop.off = {2, height_ + 3};
  if (!kBounds.Contains(drop.off + Point{4, 4})) return Error("Bad bounds");
  while (true) {
    w_idx_ = (w_idx_ + 1) % wind_.size();
    Point d = wind_[w_idx_];
    VLOG(2) << "Drop: " << drop.off << "; Wind: " << d;
    if (i == 13) {
      VLOG(2) << "Board (" << i << "):\n" << AddPiece(Draw(height_ + 10), drop);
    }
    TryMove(drop, d);
    if (!TryMove(drop, Cardinal::kNorth)) {
      break;
    }
  }
  if (absl::Status st = InsertRock(drop); !st.ok()) return st;
  VLOG(2) << "Board (" << i << "):\n" << Draw(height_);
  return absl::OkStatus();
}

CharBoard DropState::Draw(int height) const {
  CharBoard b(7, height);
  for (Point p : stopped_) b[p] = '#';
  return b;
}

CharBoard DropState::AddPiece(CharBoard b, DropState::Rock r) const {
  for (Point p : r.delta) {
    b[p + r.off] = '@';
  }
  return b;
}

CharBoard DropState::SummaryState::Draw() const {
  PointRectangle r = {*bounds.begin(), *bounds.begin()};
  for (Point p : bounds) r.ExpandInclude(p);
  CharBoard b(r);
  for (Point p : bounds) {
    b[p - r.min] = '#';
  }
  return b;
}

DropState::SummaryState DropState::Summarize() const {
  SummaryState ret = {.w_idx = w_idx_, .r_idx = r_idx_, .bounds = {}};

  PointRectangle bounds = {{0, 0}, {6, height_}};

  Point start = {0, height_};
  absl::flat_hash_set<Point> visited = {start};
  absl::flat_hash_set<Point> edge;
  for (std::deque queue = {{start}}; !queue.empty(); queue.pop_front()) {
    for (Point d : Cardinal::kFourDirs) {
      Point t = d + queue.front();
      if (!bounds.Contains(t)) continue;
      if (stopped_.contains(t)) {
        edge.insert(t - start);
        continue;
      }
      if (visited.insert(t).second) {
        queue.push_back(t);
      }
    }
  }
  ret.bounds = std::vector<Point>(edge.begin(), edge.end());
  auto point_x_then_y = [](Point a, Point b) {
    return a.x != b.x ? a.x < b.x : a.y < b.y;
  };
  absl::c_sort(ret.bounds, point_x_then_y);
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_17::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");

  DropState ds;
  if (absl::Status st = ds.ParseWind(input[0]); !st.ok()) return st;

  for (int i = 0; i < 2022; ++i) {
    if (absl::Status st = ds.DropNextRock(i); !st.ok()) return st;
  }
  return IntReturn(ds.height());
}

absl::StatusOr<std::string> Day_2022_17::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");

  DropState ds;
  if (absl::Status st = ds.ParseWind(input[0]); !st.ok()) return st;

  absl::flat_hash_map<DropState::SummaryState, int> state_to_idx;
  std::vector<int> heights;
  for (int i = 0; true; ++i) {
    if (absl::Status st = ds.DropNextRock(i); !st.ok()) return st;

    heights.push_back(ds.height());
    auto [it, inserted] = state_to_idx.emplace(ds.Summarize(), i);
    VLOG(2) << "Summary:\n" << it->first.Draw();
    if (!inserted) {
      VLOG(1) << "Repeat @" << i << " == " << it->second;
      int64_t find = 1000000000000 - 1;  // Problem is 1-indexed.
      int prev_i = it->second;

      int64_t cycles = (find - prev_i) / (i - prev_i);
      int64_t offset = (find - prev_i) % (i - prev_i);

      int64_t ret_height =
          (heights[i] - heights[prev_i]) * cycles + heights[offset + prev_i];

      VLOG(1) << "Return " << ret_height;

      return IntReturn(ret_height);
    }
  }
  return Error("Left infinite loop");
}

}  // namespace advent_of_code