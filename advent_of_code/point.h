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
    std::pair<absl::string_view, absl::string_view> coord_str = absl::StrSplit(
        absl::string_view(str, n), absl::MaxSplits(absl::ByAnyChar(",x"), 2));
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

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Point& p) {
    absl::Format(&sink, "{%v,%v}", p.x, p.y);
  }

  friend Point operator*(int s, Point p) {
    return {.x = s * p.x, .y = s * p.y};
  }

  template <typename H>
  friend H AbslHashValue(H h, const Point& p) {
    return H::combine(std::move(h), p.x, p.y);
  }

  friend std::ostream& operator<<(std::ostream& out, const Point& p) {
    return out << absl::StreamFormat("%v", p);
  }
};

struct PointRectangle {
  static PointRectangle Null() {
    return {.min = {.x = std::numeric_limits<int>::max(),
                    .y = std::numeric_limits<int>::max()},
            .max = {.x = std::numeric_limits<int>::min(),
                    .y = std::numeric_limits<int>::min()}};
  }

  static PointRectangle Bounding(const absl::flat_hash_set<Point>& set) {
    PointRectangle ret{*set.begin(), *set.begin()};
    for (Point p : set) ret.ExpandInclude(p);
    return ret;
  }

  struct iterator {
    Point cur = Point{0, 0};
    const PointRectangle* base = nullptr;

    iterator operator++() {
      if (++cur.x > base->max.x) {
        cur.x = base->min.x;
        if (++cur.y > base->max.y) {
          base = nullptr;
          cur = Point{0, 0};
        }
      }
      return *this;
    }

    bool operator==(const iterator& o) const {
      return base == o.base && cur == o.cur;
    }
    bool operator!=(const iterator& o) const { return !operator==(o); }

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

  iterator begin() const { return iterator{.cur = min, .base = this}; }
  iterator end() const { return iterator{}; }

  bool operator==(const PointRectangle& o) const {
    return min == o.min && max == o.max;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const PointRectangle& r) {
    absl::Format(&sink, "%v-%v", r.min, r.max);
  }

  friend std::ostream& operator<<(std::ostream& out, const PointRectangle& r) {
    return out << absl::StreamFormat("%v", r);
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
  static constexpr Point kFourDirs[] = {kNorth, kSouth, kWest, kEast};
  static constexpr Point kNorthWest = kNorth + kWest;
  static constexpr Point kNorthEast = kNorth + kEast;
  static constexpr Point kSouthWest = kSouth + kWest;
  static constexpr Point kSouthEast = kSouth + kEast;
  static constexpr Point kEightDirs[] = {kNorth,     kSouth,     kWest,
                                         kEast,      kNorthWest, kNorthEast,
                                         kSouthWest, kSouthEast};
};

struct Point3 {
  int x;
  int y;
  int z;

  static bool RE2Parse(const char* str, size_t n, void* dest) {
    std::vector<absl::string_view> coord_str = absl::StrSplit(
        absl::string_view(str, n), absl::MaxSplits(absl::ByAnyChar(",x"), 3));
    if (coord_str.size() != 3) return false;
    if (!absl::SimpleAtoi(coord_str[0], &((Point3*)dest)->x)) return false;
    if (!absl::SimpleAtoi(coord_str[1], &((Point3*)dest)->y)) return false;
    if (!absl::SimpleAtoi(coord_str[2], &((Point3*)dest)->z)) return false;
    return true;
  }

  RE2::Arg Capture() { return RE2::Arg(this, RE2Parse); }

  constexpr Point3 operator*(int s) const {
    return {.x = s * x, .y = s * y, .z = s * z};
  }

  constexpr Point3 Cross(const Point3& o) const {
    return {
        y * o.z - z * o.y,
        z * o.x - x * o.z,
        x * o.y - y * o.x,
    };
  }

  constexpr int Dot(const Point3& o) const {
    return x * o.x + y * o.y + z * o.z;
  }

  constexpr bool operator==(const Point3& other) const {
    return x == other.x && y == other.y && z == other.z;
  }
  constexpr bool operator!=(const Point3& other) const {
    return !operator==(other);
  }

  constexpr Point3 operator-(const Point3& other) const {
    return {.x = x - other.x, .y = y - other.y, .z = z - other.z};
  }

  constexpr Point3 operator+(const Point3& other) const {
    return {.x = x + other.x, .y = y + other.y, .z = z + other.z};
  }

  Point3& operator+=(const Point3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Point3& operator-=(const Point3& other) { return *this += -other; }

  constexpr Point3 operator-() const { return {.x = -x, .y = -y, .z = -z}; }

  int dist() const { return abs(x) + abs(y) + abs(z); }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Point3& p) {
    absl::Format(&sink, "{%v,%v,%v}", p.x, p.y, p.z);
  }

  friend constexpr Point3 operator*(int s, Point3 p) {
    return {.x = s * p.x, .y = s * p.y, .z = s * p.z};
  }

  template <typename H>
  friend H AbslHashValue(H h, const Point3& p) {
    return H::combine(std::move(h), p.x, p.y, p.z);
  }

  friend std::ostream& operator<<(std::ostream& out, const Point3& p) {
    out << "{" << p.x << "," << p.y << "," << p.z << "}";
    return out;
  }
};

struct Cardinal3 {
  static constexpr Point3 kOrigin{0, 0, 0};
  static constexpr Point3 kXHat{1, 0, 0};
  static constexpr Point3 kYHat{0, 1, 0};
  static constexpr Point3 kZHat{0, 0, 1};
  static constexpr std::array<Point3, 6> kSixDirs = {kXHat,  -kXHat, kYHat,
                                                     -kYHat, kZHat,  -kZHat};
  static constexpr std::array<Point3, 3 * 3 * 3 - 1> kNeighborDirs =
      []() constexpr {
    std::array<Point3, 3 * 3 * 3 - 1> ret{};
    int i = 0;
    for (Point3 xdir :
         {-Cardinal3::kXHat, Cardinal3::kOrigin, Cardinal3::kXHat}) {
      for (Point3 ydir :
           {-Cardinal3::kYHat, Cardinal3::kOrigin, Cardinal3::kYHat}) {
        for (Point3 zdir :
             {-Cardinal3::kZHat, Cardinal3::kOrigin, Cardinal3::kZHat}) {
          Point3 next = xdir + ydir + zdir;
          if (next != Cardinal3::kOrigin) ret[i++] = next;
        }
      }
    }
    return ret;
  }
  ();
};

struct Cube {
  Point3 min;
  Point3 max;

  struct iterator {
    Point3 cur = Point3{0, 0, 0};
    const Cube* base = nullptr;

    iterator operator++() {
      if (++cur.x > base->max.x) {
        cur.x = base->min.x;
        if (++cur.y > base->max.y) {
          cur.y = base->min.y;
          if (++cur.z > base->max.z) {
            base = nullptr;
            cur = Point3{0, 0, 0};
          }
        }
      }
      return *this;
    }

    bool operator==(const iterator& o) const {
      return base == o.base && cur == o.cur;
    }
    bool operator!=(const iterator& o) const { return !operator==(o); }

    Point3 operator*() const { return cur; }
  };

  iterator begin() const { return iterator{.cur = min, .base = this}; }
  iterator end() const { return iterator{}; }

  int64_t Volume() const {
    // 1ll forces the addition to return int64 before multiplying.
    return (max.x - min.x + 1ll) * (max.y - min.y + 1ll) *
           (max.z - min.z + 1ll);
  }

  void ExpandInclude(Point3 p) {
    min.x = std::min(min.x, p.x);
    min.y = std::min(min.y, p.y);
    min.z = std::min(min.z, p.z);
    max.x = std::max(max.x, p.x);
    max.y = std::max(max.y, p.y);
    max.z = std::max(max.z, p.z);
  }
  bool Contains(Point3 p) const {
    if (p.x < min.x) return false;
    if (p.x > max.x) return false;
    if (p.y < min.y) return false;
    if (p.y > max.y) return false;
    if (p.z < min.z) return false;
    if (p.z > max.z) return false;
    return true;
  }
  bool FullyContains(const Cube& o) const {
    if (o.min.x < min.x) return false;
    if (o.max.x > max.x) return false;
    if (o.min.y < min.y) return false;
    if (o.max.y > max.y) return false;
    if (o.min.z < min.z) return false;
    if (o.max.z > max.z) return false;
    return true;
  }
  bool Overlaps(const Cube& o) const {
    if (max.x < o.min.x) return false;
    if (min.x > o.max.x) return false;
    if (max.y < o.min.y) return false;
    if (min.y > o.max.y) return false;
    if (max.z < o.min.z) return false;
    if (min.z > o.max.z) return false;
    return true;
  }

  absl::optional<Cube> Intersect(const Cube& o) const {
    if (!Overlaps(o)) return {};
    return Cube{
        {std::max(min.x, o.min.x), std::max(min.y, o.min.y),
         std::max(min.z, o.min.z)},
        {std::min(max.x, o.max.x), std::min(max.y, o.max.y),
         std::min(max.z, o.max.z)},
    };
  }

  void SetDifference(const Cube& o, std::vector<Cube>* out) const;
};

struct Point4 {
  int x;
  int y;
  int z;
  int w;

  static bool RE2Parse(const char* str, size_t n, void* dest) {
    std::vector<absl::string_view> coord_str = absl::StrSplit(
        absl::string_view(str, n), absl::MaxSplits(absl::ByAnyChar(",x"), 4));
    if (coord_str.size() != 4) return false;
    if (!absl::SimpleAtoi(coord_str[0], &((Point4*)dest)->x)) return false;
    if (!absl::SimpleAtoi(coord_str[1], &((Point4*)dest)->y)) return false;
    if (!absl::SimpleAtoi(coord_str[2], &((Point4*)dest)->z)) return false;
    if (!absl::SimpleAtoi(coord_str[3], &((Point4*)dest)->w)) return false;
    return true;
  }

  RE2::Arg Capture() { return RE2::Arg(this, RE2Parse); }

  constexpr Point4 operator*(int s) const {
    return {.x = s * x, .y = s * y, .z = s * z, .w = s * w};
  }

  constexpr bool operator==(const Point4& other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
  }
  constexpr bool operator!=(const Point4& other) const {
    return !operator==(other);
  }

  constexpr Point4 operator-(const Point4& other) const {
    return {
        .x = x - other.x, .y = y - other.y, .z = z - other.z, .w = w - other.w};
  }

  constexpr Point4 operator+(const Point4& other) const {
    return {
        .x = x + other.x, .y = y + other.y, .z = z + other.z, .w = w + other.w};
  }

  Point4& operator+=(const Point4& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
  }

  Point4& operator-=(const Point4& other) { return *this += -other; }

  constexpr Point4 operator-() const {
    return {.x = -x, .y = -y, .z = -z, .w = -w};
  }

  int dist() const { return abs(x) + abs(y) + abs(z) + abs(w); }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Point4& p) {
    absl::Format(&sink, "{%v,%v,%v,%v}", p.x, p.y, p.z, p.w);
  }

  friend constexpr Point4 operator*(int s, Point4 p) {
    return {.x = s * p.x, .y = s * p.y, .z = s * p.z, .w = s * p.w};
  }

  template <typename H>
  friend H AbslHashValue(H h, const Point4& p) {
    return H::combine(std::move(h), p.x, p.y, p.z, p.w);
  }

  friend std::ostream& operator<<(std::ostream& out, const Point4& p) {
    out << "{" << p.x << "," << p.y << "," << p.z << "," << p.w << "}";
    return out;
  }
};

class Orientation3 {
 public:
  static Orientation3 Aligned() { return Orientation3(); }

  static const std::vector<Orientation3>& All();

  Orientation3() : Orientation3(Cardinal3::kXHat, Cardinal3::kYHat) {}

  Orientation3(Point3 x_hat, Point3 y_hat)
      : x_hat_(x_hat), y_hat_(y_hat), z_hat_(x_hat.Cross(y_hat)) {
    CHECK_EQ(x_hat_.dist(), 1);
    CHECK_EQ(y_hat_.dist(), 1);
    CHECK_EQ(z_hat_.dist(), 1);
    CHECK_EQ(x_hat_.Dot(y_hat_), 0);
    CHECK_EQ(x_hat_.Dot(z_hat_), 0);
    CHECK_EQ(y_hat_.Dot(z_hat_), 0);
  }

  Point3 Apply(Point3 in) const {
    return in.x * x_hat_ + in.y * y_hat_ + in.z * z_hat_;
  }

  Orientation3 Apply(Orientation3 in) const {
    Orientation3 ret;
    ret.x_hat_ = Apply(in.x_hat_);
    ret.y_hat_ = Apply(in.y_hat_);
    ret.z_hat_ = Apply(in.z_hat_);
    return ret;
  }

  friend std::ostream& operator<<(std::ostream& out, const Orientation3& o) {
    return out << o.x_hat_ << "/" << o.y_hat_ << "/" << o.z_hat_;
  }

 private:
  Point3 x_hat_;
  Point3 y_hat_;
  Point3 z_hat_;
};

struct Cardinal4 {
  static constexpr Point4 kOrigin{0, 0, 0, 0};
  static constexpr Point4 kXHat{1, 0, 0, 0};
  static constexpr Point4 kYHat{0, 1, 0, 0};
  static constexpr Point4 kZHat{0, 0, 1, 0};
  static constexpr Point4 kWHat{0, 0, 0, 1};
  static constexpr std::array<Point4, 3 * 3 * 3 * 3 - 1> kNeighborDirs =
      []() constexpr {
    std::array<Point4, 3 * 3 * 3 * 3 - 1> ret{};
    int i = 0;
    for (Point4 xdir :
         {-Cardinal4::kXHat, Cardinal4::kOrigin, Cardinal4::kXHat}) {
      for (Point4 ydir :
           {-Cardinal4::kYHat, Cardinal4::kOrigin, Cardinal4::kYHat}) {
        for (Point4 zdir :
             {-Cardinal4::kZHat, Cardinal4::kOrigin, Cardinal4::kZHat}) {
          for (Point4 wdir :
               {-Cardinal4::kWHat, Cardinal4::kOrigin, Cardinal4::kWHat}) {
            Point4 next = xdir + ydir + zdir + wdir;
            if (next != Cardinal4::kOrigin) ret[i++] = next;
          }
        }
      }
    }
    return ret;
  }
  ();
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_POINT_H
