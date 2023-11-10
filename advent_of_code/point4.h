#ifndef ADVENT_OF_CODE_POINT4_H
#define ADVENT_OF_CODE_POINT4_H

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

struct Point4 {
  int x;
  int y;
  int z;
  int w;

  static bool RE2Parse(const char* str, size_t n, void* dest) {
    std::vector<std::string_view> coord_str = absl::StrSplit(
        std::string_view(str, n), absl::MaxSplits(absl::ByAnyChar(",x"), 4));
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

  friend constexpr Point4 operator*(int s, Point4 p) {
    return {.x = s * p.x, .y = s * p.y, .z = s * p.z, .w = s * p.w};
  }

  template <typename H>
  friend H AbslHashValue(H h, const Point4& p) {
    return H::combine(std::move(h), p.x, p.y, p.z, p.w);
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Point4& p) {
    absl::Format(&sink, "{%v,%v,%v,%v}", p.x, p.y, p.z, p.w);
  }
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

#endif  // ADVENT_OF_CODE_POINT4_H
