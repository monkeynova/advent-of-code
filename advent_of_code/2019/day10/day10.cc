// http://adventofcode.com/2019/day/10

#include "advent_of_code/2019/day10/day10.h"

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

namespace advent_of_code {
namespace {

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
  MostVisible ret{{0, 0}, 0};
  for (const Point& coord : board.asteroids) {
    int visible = CountVisible(board, coord);
    if (visible > ret.seen) {
      ret.seen = visible;
      ret.most_visible = coord;
    }
  }
  return ret;
}

absl::StatusOr<Board> ParseBoard(absl::Span<absl::string_view> input) {
  absl::StatusOr<CharBoard> char_board = CharBoard::Parse(input);
  if (!char_board.ok()) return char_board.status();

  Board ret;
  ret.height = char_board->height();
  ret.width = char_board->width();
  for (Point p : char_board->range()) {
    switch ((*char_board)[p]) {
      case '.': {
        break;
      }
      case 'X': {
        ret.asteroids.insert(p);
        break;
      }
      default:
        return absl::InvalidArgumentError("bad token");
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_10::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Board> asteroids = ParseBoard(input);
  if (!asteroids.ok()) return asteroids.status();
  MostVisible most_visible = FindMostVisible(*asteroids);

  return IntReturn(most_visible.seen);
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

absl::StatusOr<std::string> Day_2019_10::Part2(
    absl::Span<absl::string_view> input) const {
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
      theta += 8 * atan2(1, 1) /* 2 * pi */;
    }
    ordered_destruct.push_back(
        {.p = p, .theta = theta, .count = ObscureCount(*board, origin, p)});
  }
  std::sort(ordered_destruct.begin(), ordered_destruct.end());
  int ret = ordered_destruct[199].p.x * 100 + ordered_destruct[199].p.y;
  return IntReturn(ret);
}

}  // namespace advent_of_code
