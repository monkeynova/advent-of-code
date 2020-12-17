#ifndef ADVENT_OF_CODE_2019_POINT_H
#define ADVENT_OF_CODE_2019_POINT_H

#include <iostream>
#include <numeric>

#include "absl/hash/hash.h"
#include "absl/strings/str_cat.h"

struct Point {
  int x;
  int y;

  Point operator*(int s) const { return {.x = s * x, .y = s * y}; }

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Point& other) const { return !operator==(other); }

  Point operator-(const Point& other) const {
    return {.x = x - other.x, .y = y - other.y};
  }

  Point operator+(const Point& other) const {
    return {.x = x + other.x, .y = y + other.y};
  }

  Point& operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Point min_step() const {
    int gcd = std::gcd(abs(x), abs(y));
    return {.x = x / gcd, .y = y / gcd};
  }

  Point operator-() const { return {.x = -x, .y = -y}; }

  // Rotate left matrix = [[0, 1], [-1, 0]]
  Point rotate_left() const { return {.x = y, .y = -x}; }

  // Rotate right matrix = [[0, -1], [1, 0]]
  Point rotate_right() const { return {.x = -y, .y = x}; }

  int dist() const { return abs(x) + abs(y); }
  int dist(Point o) const { return abs(x - o.x) + abs(y - o.y); }

  std::string DebugString() const { return absl::StrCat("{", x, ",", y, "}"); }
};

Point operator*(int s, Point p) { return {.x = s * p.x, .y = s * p.y}; }

template <typename H>
H AbslHashValue(H h, const Point& p) {
  return H::combine(std::move(h), p.x, p.y);
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
  out << "{" << p.x << "," << p.y << "}";
  return out;
}

struct PointRectangle {
  static PointRectangle Null() {
    return {.min = {.x = std::numeric_limits<int>::max(),
                    .y = std::numeric_limits<int>::max()},
            .max = {.x = std::numeric_limits<int>::min(),
                    .y = std::numeric_limits<int>::min()}};
  }

  struct iterator {
    Point cur = Point{0, 0};
    PointRectangle* base = nullptr;

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
  };

  Point min;
  Point max;

  void ExpandInclude(Point p) {
    min.x = std::min(min.x, p.x);
    min.y = std::min(min.y, p.y);
    max.x = std::max(max.x, p.x);
    max.y = std::max(max.y, p.y);
  }

  iterator begin() { return iterator{.cur = min, .base = this}; }
  iterator end() { return iterator{}; }
};

std::ostream& operator<<(std::ostream& out, const PointRectangle& r) {
  out << r.min << "-" << r.max;
  return out;
}

struct Cardinal {
  static constexpr Point kOrigin{0, 0};
  static constexpr Point kNorth{0, -1};
  static constexpr Point kSouth{0, 1};
  static constexpr Point kWest{-1, 0};
  static constexpr Point kEast{1, 0};
  static constexpr Point kFourDirs[] = {kNorth, kSouth, kWest, kEast};
  static constexpr Point kNorthWest{-1, -1};
  static constexpr Point kNorthEast{1, -1};
  static constexpr Point kSouthWest{-1, 1};
  static constexpr Point kSouthEast{1, 1};
  static constexpr Point kEightDirs[] = {kNorth,     kSouth,     kWest,
                                         kEast,      kNorthWest, kNorthEast,
                                         kSouthWest, kSouthEast};
};

struct Point3 {
  int x;
  int y;
  int z;

  Point3 operator*(int s) const { return {.x = s * x, .y = s * y, .z = s * z}; }

  bool operator==(const Point3& other) const {
    return x == other.x && y == other.y && z == other.z;
  }
  bool operator!=(const Point3& other) const { return !operator==(other); }

  Point3 operator-(const Point3& other) const {
    return {.x = x - other.x, .y = y - other.y, .z = z - other.z};
  }

  Point3 operator+(const Point3& other) const {
    return {.x = x + other.x, .y = y + other.y, .z = z + other.z};
  }

  Point3& operator+=(const Point3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Point3 operator-() const { return {.x = -x, .y = -y, .z = -z}; }

  int dist() const { return abs(x) + abs(y) + abs(z); }

  std::string DebugString() const {
    return absl::StrCat("{", x, ",", y, ",", z, "}");
  }
};

Point3 operator*(int s, Point3 p) {
  return {.x = s * p.x, .y = s * p.y, .z = s * p.z};
}

template <typename H>
H AbslHashValue(H h, const Point3& p) {
  return H::combine(std::move(h), p.x, p.y, p.z);
}

std::ostream& operator<<(std::ostream& out, const Point3& p) {
  out << "{" << p.x << "," << p.y << "," << p.z << "}";
  return out;
}

struct Cardinal3 {
  static constexpr Point3 kOrigin{0, 0, 0};
  static constexpr Point3 xHat{1, 0, 0};
  static constexpr Point3 yHat{0, 1, 0};
  static constexpr Point3 zHat{0, 0, 1};
};

struct Point4 {
  int x;
  int y;
  int z;
  int w;

  Point4 operator*(int s) const {
    return {.x = s * x, .y = s * y, .z = s * z, .w = s * w};
  }

  bool operator==(const Point4& other) const {
    return x == other.x && y == other.y && z == other.z && w == other.w;
  }
  bool operator!=(const Point4& other) const { return !operator==(other); }

  Point4 operator-(const Point4& other) const {
    return {
        .x = x - other.x, .y = y - other.y, .z = z - other.z, .w = w - other.w};
  }

  Point4 operator+(const Point4& other) const {
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

  Point4 operator-() const { return {.x = -x, .y = -y, .z = -z, .w = -w}; }

  int dist() const { return abs(x) + abs(y) + abs(z) + abs(w); }

  std::string DebugString() const {
    return absl::StrCat("{", x, ",", y, ",", z, ",", w, "}");
  }
};

Point4 operator*(int s, Point4 p) {
  return {.x = s * p.x, .y = s * p.y, .z = s * p.z, .w = s * p.w};
}

template <typename H>
H AbslHashValue(H h, const Point4& p) {
  return H::combine(std::move(h), p.x, p.y, p.z, p.w);
}

std::ostream& operator<<(std::ostream& out, const Point4& p) {
  out << "{" << p.x << "," << p.y << "," << p.z << "," << p.w << "}";
  return out;
}

struct Cardinal4 {
  static constexpr Point4 kOrigin{0, 0, 0, 0};
  static constexpr Point4 xHat{1, 0, 0, 0};
  static constexpr Point4 yHat{0, 1, 0, 0};
  static constexpr Point4 zHat{0, 0, 1, 0};
  static constexpr Point4 wHat{0, 0, 0, 1};
};

#endif  //  ADVENT_OF_CODE_2019_POINT_H