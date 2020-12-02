#ifndef ADVENT_OF_CODE_2019_POINT_H
#define ADVENT_OF_CODE_2019_POINT_H

#include <iostream>
#include <numeric>

#include "absl/hash/hash.h"

struct Point {
  int x;
  int y;

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

  int dist() const { return abs(x) + abs(y); }
  int dist(Point o) const { return abs(x - o.x) + abs(y - o.y); }
};

template <typename H>
H AbslHashValue(H h, const Point& p) {
  return H::combine(std::move(h), p.x, p.y);
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
  out << "{" << p.x << "," << p.y << "}";
  return out;
}

#endif  //  ADVENT_OF_CODE_2019_POINT_H