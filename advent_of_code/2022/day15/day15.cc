// http://adventofcode.com/2022/day/15

#include "advent_of_code/2022/day15/day15.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

ABSL_FLAG(int, advent_day_2022_15_param, -1, "...");

namespace advent_of_code {

namespace {

struct SAndB {
  Point sensor;
  Point beacon;
  int d;
};

absl::StatusOr<std::vector<SAndB>> ParseList(
    absl::Span<absl::string_view> input) {
  static LazyRE2 parse_re =
      {"Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at "
       "x=(-?\\d+), y=(-?\\d+)"};
  std::vector<SAndB> list;
  for (absl::string_view line : input) {
    SAndB add;
    if (!RE2::FullMatch(line, *parse_re, &add.sensor.x, &add.sensor.y,
                        &add.beacon.x, &add.beacon.y)) {
      return Error("Bad line: ", line);
    }
    add.d = (add.beacon - add.sensor).dist();
    list.push_back(add);
  }
  return list;
}

struct Interval1D {
  std::vector<int> x;

  Interval1D() = default;
  Interval1D(int start, int end) : x({start, end}) {}

  int Size() const;

  Interval1D Union(const Interval1D& o) const;
  Interval1D Minus(const Interval1D& o) const;

  Interval1D Merge(const Interval1D& o,
                   absl::FunctionRef<bool(bool, bool)> merge_fn) const;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Interval1D& iv) {
    if (iv.x.empty()) {
      absl::Format(&sink, "∅");
    }
    for (int i = 0; i + 1 < iv.x.size(); i += 2) {
      absl::Format(&sink, "[%d,%d)", iv.x[i], iv.x[i + 1]);
    }
    if (iv.x.size() % 2 != 0) {
      absl::Format(&sink, "[%d,∞)", iv.x.back());
    }
  }

  friend std::ostream& operator<<(std::ostream& out, const Interval1D& i) {
    return out << absl::StreamFormat("%v", i);
  }
};

int Interval1D::Size() const {
  int size = 0;
  for (int i = 0; i < x.size(); i += 2) {
    size += x[i + 1] - x[i];
  }
  return size;
}

Interval1D Interval1D::Union(const Interval1D& o) const {
  return Merge(o, [](bool left, bool right) { return left || right; });
}

Interval1D Interval1D::Minus(const Interval1D& o) const {
  return Merge(o, [](bool left, bool right) { return left && !right; });
}

Interval1D Interval1D::Merge(const Interval1D& o,
                             absl::FunctionRef<bool(bool, bool)> merge_fn) const {
  auto left_it = x.begin();
  bool left_on = false;
  auto right_it = o.x.begin();
  bool right_on = false;
  bool ret_on = false;
  Interval1D ret;
  while (left_it != x.end() && right_it != o.x.end()) {
    int lval = *left_it;
    int rval = *right_it;
    int t;
    if (lval <= rval) {
      t = lval;
      left_on = !left_on;
      ++left_it;
    }
    if (lval >= rval) {
      t = rval;
      right_on = !right_on;
      ++right_it;
    }
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x.push_back(t);
      ret_on = !ret_on;
    }
  }
  while (left_it != x.end()) {
    int t = *left_it;
    left_on = !left_on;
    ++left_it;
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x.push_back(t);
      ret_on = !ret_on;
    }
  }
  while (right_it != o.x.end()) {
    int t = *right_it;
    right_on = !right_on;
    ++right_it;
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x.push_back(t);
      ret_on = !ret_on;
    }
  }
  VLOG(3) << *this << " + " << o << " = " << ret;
  return ret;
}

Interval1D NoCloser(const SAndB& sandb, int y) {
  int dx = sandb.d - abs(sandb.sensor.y - y);
  if (dx < 0) return Interval1D();
  return Interval1D(sandb.sensor.x - dx, sandb.sensor.x + dx + 1);
}

bool HasCloser(const std::vector<SAndB>& list, Point t) {
  bool is_closer = false;
  for (const auto& sandb : list) {
    if ((sandb.sensor - t).dist() <= sandb.d) {
      is_closer = true;
    }
  }
  return is_closer;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_15::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<SAndB>> list = ParseList(input);
  if (!list.ok()) return list.status();

  int y = absl::GetFlag(FLAGS_advent_day_2022_15_param);
  Interval1D no_closer;
  Interval1D beacons;
  for (const auto& sandb : *list) {
    if (sandb.beacon.y == y) {
      beacons = beacons.Union(Interval1D(sandb.beacon.x, sandb.beacon.x + 1));
    }
    no_closer = no_closer.Union(NoCloser(sandb, y));
  }
  VLOG(1) << "Final: " << no_closer.Minus(beacons);
  return IntReturn(no_closer.Minus(beacons).Size());
}

absl::StatusOr<std::string> Day_2022_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<SAndB>> list = ParseList(input);
  if (!list.ok()) return list.status();

  PointRectangle test_area;
  test_area.min = {0, 0};
  int max = absl::GetFlag(FLAGS_advent_day_2022_15_param);
  test_area.max = Point{max, max};
  std::optional<Point> found;
  for (const auto& sandb : *list) {
    int d = (sandb.beacon - sandb.sensor).dist();
    for (int i = 0; i <= d + 1; ++i) {
      std::array<Point, 4> to_test = {
        Point{i, (d + 1) - i}, Point{-i, -(d + 1) + i},
        Point{i, (d + 1) - i}, Point{-i, -(d + 1) + i},
      };
      for (Point delta : to_test) {
        Point t = sandb.sensor + delta;
        if (!test_area.Contains(t)) continue;
        if (!HasCloser(*list, t)) {
          if (found && *found != t) return Error("Duplicate!");
          found = t;
          VLOG(1) << "Found @" << *found << " -> "
                  << found->x * 4000000ll + found->y;
        }
      }
    }
  }
  if (!found) return Error("Not found");
  return IntReturn(found->x * 4000000ll + found->y);
}

}  // namespace advent_of_code
