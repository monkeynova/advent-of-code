#ifndef ADVENT_OF_CODE_2019_POINT_H
#define ADVENT_OF_CODE_2019_POINT_H

#include <array>
#include <iostream>
#include <numeric>

#include "absl/hash/hash.h"
#include "absl/strings/str_cat.h"

struct Point {
  int x;
  int y;

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

  constexpr Point3 operator*(int s) const {
    return {.x = s * x, .y = s * y, .z = s * z};
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

  constexpr Point3 operator-() const { return {.x = -x, .y = -y, .z = -z}; }

  int dist() const { return abs(x) + abs(y) + abs(z); }

  std::string DebugString() const {
    return absl::StrCat("{", x, ",", y, ",", z, "}");
  }
};

constexpr Point3 operator*(int s, Point3 p) {
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
  static constexpr Point3 kXHat{1, 0, 0};
  static constexpr Point3 kYHat{0, 1, 0};
  static constexpr Point3 kZHat{0, 0, 1};
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

struct Point4 {
  int x;
  int y;
  int z;
  int w;

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

  constexpr Point4 operator-() const {
    return {.x = -x, .y = -y, .z = -z, .w = -w};
  }

  int dist() const { return abs(x) + abs(y) + abs(z) + abs(w); }

  std::string DebugString() const {
    return absl::StrCat("{", x, ",", y, ",", z, ",", w, "}");
  }
};

constexpr Point4 operator*(int s, Point4 p) {
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

#endif  //  ADVENT_OF_CODE_2019_POINT_H