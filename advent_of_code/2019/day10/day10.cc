#include "advent_of_code/2019/day10/day10.h"

#include <numeric>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

struct Point {
  int x;
  int y;

  bool operator==(const Point& other) const { return x == other.x && y == other.y; }
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
};

  template <typename H>
  H AbslHashValue(H h, const Point& p) {
    return H::combine(std::move(h), p.x, p.y);
  }

std::ostream& operator<<(std::ostream& out, const Point& p) {
  out << "{" << p.x << "," << p.y << "}";
  return out;
}

int CountVisible(const absl::flat_hash_set<Point>& asteroids, int width, int height,
                 Point coord) {
  int visible = 0;
  for (const auto& maybe_visible : asteroids) {
    if (maybe_visible == coord) continue;

    Point delta = maybe_visible - coord;
    Point step = delta.min_step();
    bool obscured = false;
    for (Point check = coord + step; check != maybe_visible; check += step) {
      if (asteroids.contains(check)) {
        obscured = true;
        break;
      }
    }
    if (obscured) continue;
    ++visible;
  }
  return visible;
}

int FindMostVisible(const absl::flat_hash_set<Point>& asteroids, int width, int height) {
  int most_visible = 0;
  for (const Point& coord : asteroids) {
    int visible = CountVisible(asteroids, width, height, coord);
    most_visible = std::max(most_visible, visible);
  }
  return most_visible;
}

absl::StatusOr<std::vector<std::string>> Day10_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  if (input.empty()) return absl::InvalidArgumentError("empty input");
  const int height = input.size();
  const int width = input[0].size();
  absl::flat_hash_set<Point> asteroids;
  for (int y = 0; y < height; ++y) {
    if (input[y].size() != width) return absl::InvalidArgumentError("bad width");
    for (int x = 0; x < width; ++x) {
      switch (input[y][x]) {
        case '.': {
          break;
        }
        case 'X': {
          asteroids.insert({.x = x, .y = y});
          break;
        }
        default: return absl::InvalidArgumentError("bad token");
      }
    }
  }
  int most_visible = FindMostVisible(asteroids, width, height);

  return std::vector<std::string>{absl::StrCat(most_visible)};
}

absl::StatusOr<std::vector<std::string>> Day10_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
