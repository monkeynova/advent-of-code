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

struct Board {
  absl::flat_hash_set<Point> asteroids;
  int width;
  int height;
};

int ObscureCount(const Board& board, Point start, Point end) {
  int count = 0; 
  Point delta = end - start;
  Point step = delta.min_step();
  for (Point check = start + step; check != end; check += step) {
    if (board.asteroids.contains(check)) {
      ++count;
    }
  }
  return count;
}

int CountVisible(const Board& board, Point coord) {
  int visible = 0;
  for (const auto& maybe_visible : board.asteroids) {
    if (maybe_visible == coord) continue;

    if (ObscureCount(board, coord, maybe_visible) > 0) continue;
    ++visible;
  }
  return visible;
}

struct MostVisible {
  Point most_visible;
  int seen;
};

MostVisible FindMostVisible(const Board& board) {
  MostVisible ret{{0,0}, 0};
  for (const Point& coord : board.asteroids) {
    int visible = CountVisible(board, coord);
    if (visible > ret.seen) {
      ret.seen = visible;
      ret.most_visible = coord;
    }
  }
  return ret;
}

absl::StatusOr<Board> ParseBoard(const std::vector<absl::string_view>& input) {
  if (input.empty()) return absl::InvalidArgumentError("empty input");
  Board ret;
  ret.height = input.size();
  ret.width = input[0].size();
  for (int y = 0; y < ret.height; ++y) {
    if (input[y].size() != ret.width) return absl::InvalidArgumentError("bad width");
    for (int x = 0; x < ret.width; ++x) {
      switch (input[y][x]) {
        case '.': {
          break;
        }
        case 'X': {
          ret.asteroids.insert({.x = x, .y = y});
          break;
        }
        default: return absl::InvalidArgumentError("bad token");
      }
    }
  }
  return ret;
}

absl::StatusOr<std::vector<std::string>> Day10_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<Board> asteroids = ParseBoard(input);
  if (!asteroids.ok()) return asteroids.status();
  MostVisible most_visible = FindMostVisible(*asteroids);

  return std::vector<std::string>{absl::StrCat(most_visible.seen)};
}

struct OrderedDestruct {
  Point p;
  double theta;
  int count;
  bool operator<(const OrderedDestruct& other) const {
    if (count < other.count) return true;
    if (count > other.count) return false;
    if (theta < other.theta) return true;
    if (theta > other.theta) return false;
    // Equal.
    return false;
  }
};

absl::StatusOr<std::vector<std::string>> Day10_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<Board> board = ParseBoard(input);
  if (!board.ok()) return board.status();
  if (board->asteroids.size() < 201) {
    return absl::InvalidArgumentError("There aren't 200 asteroids to destroy");
  }
  MostVisible most_visible = FindMostVisible(*board);

  std::vector<OrderedDestruct> ordered_destruct;
  Point origin = most_visible.most_visible;
  for (const Point& p : board->asteroids) {
    if (p == origin) continue;
    Point delta = p - origin;
    // atan2(y,x) gives the angle from the x-axis, towards the y-axis.
    // We want angle from the -y-axis towards the +x-axis, so the args
    // are swapped and y is negated.
    double theta = atan2(delta.x, -delta.y);
    if (theta < 0) {
      // We want angles from [0, 2*PI), but atan2 returns [-PI, PI).
      theta += 8 * atan2(1,1) /* 2 * pi */;
    }
    ordered_destruct.push_back({
      .p = p,
      .count = ObscureCount(*board, origin, p),
      .theta = theta
    });
  }
  std::sort(ordered_destruct.begin(), ordered_destruct.end());
  int ret = ordered_destruct[199].p.x * 100 + ordered_destruct[199].p.y;
  return std::vector<std::string>{absl::StrCat(ret)};
}
