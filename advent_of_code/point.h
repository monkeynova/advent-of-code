#ifndef ADVENT_OF_CODE_POINT_H
#define ADVENT_OF_CODE_POINT_H

#include <array>
#include <iostream>
#include <numeric>

#include "absl/container/flat_hash_set.h"
#include "absl/hash/hash.h"
#include "absl/log/check.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

struct Point {
  int x = 0;
  int y = 0;

  static bool RE2Parse(const char* str, size_t n, void* dest) {
    std::pair<std::string_view, std::string_view> coord_str = absl::StrSplit(
        std::string_view(str, n), absl::MaxSplits(absl::ByAnyChar(",x"), 2));
    if (!absl::SimpleAtoi(coord_str.first, &((Point*)dest)->x)) return false;
    if (!absl::SimpleAtoi(coord_str.second, &((Point*)dest)->y)) return false;
    return true;
  }

  RE2::Arg Capture() { return RE2::Arg(this, RE2Parse); }

  constexpr Point operator*(int s) const { return {.x = s * x, .y = s * y}; }

  constexpr bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
  constexpr bool operator!=(const Point& other) const {
    return !operator==(other);
  }

  constexpr Point operator-(const Point& other) const {
    return {.x = x - other.x, .y = y - other.y};
  }

  constexpr Point operator+(const Point& other) const {
    return {.x = x + other.x, .y = y + other.y};
  }

  Point& operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Point& operator-=(const Point& other) { return *this += -other; }

  Point min_step() const {
    int gcd = std::gcd(abs(x), abs(y));
    return {.x = x / gcd, .y = y / gcd};
  }

  constexpr Point operator-() const { return {.x = -x, .y = -y}; }

  // Rotate left matrix = [[0, 1], [-1, 0]]
  Point rotate_left() const { return {.x = y, .y = -x}; }

  // Rotate right matrix = [[0, -1], [1, 0]]
  Point rotate_right() const { return {.x = -y, .y = x}; }

  int dist() const { return abs(x) + abs(y); }

  friend Point operator*(int s, Point p) {
    return {.x = s * p.x, .y = s * p.y};
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Point& p) {
    absl::Format(&sink, "{%v,%v}", p.x, p.y);
  }

  template <typename H>
  friend H AbslHashValue(H h, const Point& p) {
    return H::combine(std::move(h), p.x, p.y);
  }
};

struct PointRectangle {
  static PointRectangle Null() {
    return {.min = {.x = std::numeric_limits<int>::max(),
                    .y = std::numeric_limits<int>::max()},
            .max = {.x = std::numeric_limits<int>::min(),
                    .y = std::numeric_limits<int>::min()}};
  }

  template <typename Container>
  static PointRectangle Bounding(const Container& set) {
    PointRectangle ret = Null();
    for (Point p : set) ret.ExpandInclude(p);
    return ret;
  }

  struct const_iterator {
    Point cur = Point{0, 0};
    const PointRectangle* base = nullptr;

    const_iterator operator++() {
      if (++cur.x > base->max.x) {
        cur.x = base->min.x;
        if (++cur.y > base->max.y) {
          base = nullptr;
          cur = Point{0, 0};
        }
      }
      return *this;
    }

    bool operator==(const const_iterator& o) const {
      return base == o.base && cur == o.cur;
    }
    bool operator!=(const const_iterator& o) const { return !operator==(o); }

    Point operator*() const { return cur; }
    const Point* operator->() const { return &cur; }
  };

  Point min;
  Point max;

  int64_t Area() const {
    // 1ll forces the addition to return int64 before multiplying.
    return (max.x - min.x + 1ll) * (max.y - min.y + 1ll);
  }
  bool Contains(Point p) const {
    if (p.x < min.x) return false;
    if (p.x > max.x) return false;
    if (p.y < min.y) return false;
    if (p.y > max.y) return false;
    return true;
  }
  bool FullyContains(const PointRectangle& o) const {
    if (o.min.x < min.x) return false;
    if (o.max.x > max.x) return false;
    if (o.min.y < min.y) return false;
    if (o.max.y > max.y) return false;
    return true;
  }
  bool Overlaps(const PointRectangle& o) const {
    if (max.x < o.min.x) return false;
    if (min.x > o.max.x) return false;
    if (max.y < o.min.y) return false;
    if (min.y > o.max.y) return false;
    return true;
  }

  void ExpandInclude(Point p) {
    min.x = std::min(min.x, p.x);
    min.y = std::min(min.y, p.y);
    max.x = std::max(max.x, p.x);
    max.y = std::max(max.y, p.y);
  }

  const_iterator begin() const {
    return const_iterator{.cur = min, .base = this};
  }
  const_iterator end() const { return const_iterator{}; }

  bool operator==(const PointRectangle& o) const {
    return min == o.min && max == o.max;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const PointRectangle& r) {
    absl::Format(&sink, "%v-%v", r.min, r.max);
  }

  void SetDifference(const PointRectangle& o,
                     std::vector<PointRectangle>* out) const;
};

struct Cardinal {
  static constexpr Point kOrigin{0, 0};
  static constexpr Point kXHat{1, 0};
  static constexpr Point kYHat{0, 1};
  static constexpr Point kNorth = -kYHat;
  static constexpr Point kSouth = kYHat;
  static constexpr Point kWest = -kXHat;
  static constexpr Point kEast = kXHat;
  static constexpr std::array<Point, 4> kFourDirs = {kNorth, kSouth, kWest,
                                                     kEast};
  static constexpr Point kNorthWest = kNorth + kWest;
  static constexpr Point kNorthEast = kNorth + kEast;
  static constexpr Point kSouthWest = kSouth + kWest;
  static constexpr Point kSouthEast = kSouth + kEast;
  static constexpr std::array<Point, 8> kEightDirs = {
      kNorth,     kSouth,     kWest,      kEast,
      kNorthWest, kNorthEast, kSouthWest, kSouthEast};
};

struct PointYThenXLT {
  bool operator()(Point p1, Point p2) const {
    if (p1.y != p2.y) return p1.y < p2.y;
    return p1.x < p2.x;
  }
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_POINT_H
