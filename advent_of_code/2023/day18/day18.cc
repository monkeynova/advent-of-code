// http://adventofcode.com/2023/day/18

#include "advent_of_code/2023/day18/day18.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int64_t CountInterior(const std::vector<std::pair<Point, Point>>& loop) {
  absl::flat_hash_set<int> distinct_y;
  for (const auto& [p1, p2] : loop) {
    distinct_y.insert(p1.y);
    distinct_y.insert(p2.y);
  }
  std::vector<int> y_vals(distinct_y.begin(), distinct_y.end());
  absl::c_sort(y_vals);
  std::vector<std::pair<Point, Point>> verts;
  for (const auto& pair : loop) {
    if (pair.first.x == pair.second.x) {
      verts.push_back(pair);
    }
  }
  absl::c_sort(
    verts,
    [](const std::pair<Point, Point>& a, const std::pair<Point, Point>& b) {
      return a.first.x < b.first.x;
    });

  auto count_on = [verts](int y) {
    enum Bits {
      kUpper = 1,
      kLower = 2,
    };
    absl::flat_hash_map<int, int> x_to_type;
    bool in_upper = false;
    bool in_lower = false;
    int last_x = std::numeric_limits<int>::min();

    int64_t row_count = 0;
    for (const auto& [p1, p2] : verts) {
      bool start_inside = in_upper || in_lower;

      if (p1.y == y) in_lower = !in_lower;
      else if (p2.y == y) in_upper = !in_upper;
      else if (p1.y < y && p2.y > y) {
        in_lower = !in_lower;
        in_upper = !in_upper;
      }

      int x = p1.x;
      if (start_inside) {
        row_count += p1.x - last_x;
        if (!in_upper && !in_lower) {
          ++row_count;
        }
      }
      last_x = x;
    }
    return row_count;
  };

  int64_t total_count = 0;
  for (int i = 0; i < y_vals.size(); ++i) {
    if (i > 0 && y_vals[i] > y_vals[i - 1] + 1) {
      int64_t height = y_vals[i] - y_vals[i - 1] - 1;
      int y = y_vals[i] - 1;
      total_count += height * count_on(y);
    }
    int y = y_vals[i];
    total_count += count_on(y);
  }
  return total_count;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_18::Part1(
    absl::Span<std::string_view> input) const {
  Point cur = {0, 0};
  absl::flat_hash_map<Point, char> draw;
  Point prev_dir = Cardinal::kOrigin;
  Point first_dir;
  for (std::string_view line : input) {
    Tokenizer tok(line);
    std::string_view dir = tok.Next();
    ASSIGN_OR_RETURN(int dist, tok.NextInt());
    Point d;
    if (dir == "R") {
      d = Cardinal::kEast;
    } else if (dir == "U") {
      d = Cardinal::kNorth;
    } else if (dir == "D") {
      d = Cardinal::kSouth;
    } else if (dir == "L") {
      d = Cardinal::kWest;
    } else {
      return Error("Bad dir");
    }
    if (prev_dir == Cardinal::kOrigin) {
      first_dir = d;
      draw[cur] = 'S';
    } else if (prev_dir == Cardinal::kNorth) {
      if (d == Cardinal::kWest) {
        draw[cur] = '7';
      } else if (d == Cardinal::kEast) {
        draw[cur] = 'F';
      } else {
        return Error("Unhandled turn");
      }
    } else if (prev_dir == Cardinal::kSouth) {
      if (d == Cardinal::kWest) {
        draw[cur] = 'J';
      } else if (d == Cardinal::kEast) {
        draw[cur] = 'L';
      } else {
        return Error("Unhandled turn");
      }
    } else if (prev_dir == Cardinal::kEast) {
      if (d == Cardinal::kNorth) {
        draw[cur] = 'J';
      } else if (d == Cardinal::kSouth) {
        draw[cur] = '7';
      } else {
        return Error("Unhandled turn");
      }
    } else if (prev_dir == Cardinal::kWest) {
      if (d == Cardinal::kNorth) {
        draw[cur] = 'L';
      } else if (d == Cardinal::kSouth) {
        draw[cur] = 'F';
      } else {
        return Error("Unhandled turn");
      }
    }
    cur += d;
    for (int i = 1; i < dist; ++i) {
      if (d == Cardinal::kWest || d == Cardinal::kEast) {
        draw[cur] = '-';
      }
      if (d == Cardinal::kNorth || d == Cardinal::kSouth) {
        draw[cur] = '|';
      }
      cur += d;
    }
    prev_dir = d;
  }
    if (prev_dir == Cardinal::kNorth) {
      if (first_dir == Cardinal::kWest) {
        draw[cur] = '7';
      } else if (first_dir == Cardinal::kEast) {
        draw[cur] = 'F';
      } else {
        return Error("Unhandled turn");
      }
    } else if (prev_dir == Cardinal::kSouth) {
      if (first_dir == Cardinal::kWest) {
        draw[cur] = 'J';
      } else if (first_dir == Cardinal::kEast) {
        draw[cur] = 'L';
      } else {
        return Error("Unhandled turn");
      }
    } else if (prev_dir == Cardinal::kEast) {
      if (first_dir == Cardinal::kNorth) {
        draw[cur] = 'J';
      } else if (first_dir == Cardinal::kSouth) {
        draw[cur] = '7';
      } else {
        return Error("Unhandled turn");
      }
    } else if (prev_dir == Cardinal::kWest) {
      if (first_dir == Cardinal::kNorth) {
        draw[cur] = 'L';
      } else if (first_dir == Cardinal::kSouth) {
        draw[cur] = 'F';
      } else {
        return Error("Unhandled turn");
      }
    }
  
  PointRectangle bounds = PointRectangle::Null();
  for (const auto& [p, c] : draw) bounds.ExpandInclude(p);
  CharBoard b(bounds);
  for (const auto& [p, c] : draw) b[p - bounds.min] = c;
  VLOG(2) << b;

  bool inside_upper = false;
  bool inside_lower = false;
  int count = 0;
  for (const auto [p, c] : b) {
    int start_inside = inside_upper || inside_lower;
    switch (c) {
      case '-': case '.': break;
      case '|': {
        inside_upper = !inside_upper;
        inside_lower = !inside_lower;
        break;
      }
      case 'F': case '7': {
        inside_lower = !inside_lower;
        break;
      }
      case 'J': case 'L': {
        inside_upper = !inside_upper;
        break;
      }
      default: {
        return Error("Bad draw");
      }
    }
    if (inside_upper || inside_lower || start_inside) {
      ++count;
    }
  }
  return AdventReturn(count);
}

absl::StatusOr<std::string> Day_2023_18::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<std::pair<Point, Point>> loop;
  Point cur = Cardinal::kOrigin;
  for (std::string_view str : input) {
    std::string_view hex_dist = str.substr(str.size() - 7, 5);
    int dist = 0;
    for (char c : hex_dist) {
      if (c >= '0' && c <= '9') {
        dist = dist * 16 + c - '0';
      } else if (c >= 'a' && c <= 'f') {
        dist = dist * 16 + c - 'a' + 10;
      } else {
        return Error("Bad hex: \"", hex_dist, "\"");
      }
    }
    Point dir;
    if (str[str.size() - 2] >= '0' && str[str.size() - 2] <= '3') {
      const std::array<Point, 4> kDirs = {Cardinal::kEast, Cardinal::kSouth, Cardinal::kWest, Cardinal::kNorth};
      dir = kDirs[str[str.size() - 2] - '0'];
    } else {
      return Error("Bad dir");
    }
    if (dir == Cardinal::kEast || dir == Cardinal::kSouth) {
      loop.push_back({cur, cur + dir * dist});
    } else {
      loop.push_back({cur + dir * dist, cur});
    }
    cur = cur + dir * dist;
  }
  return AdventReturn(CountInterior(loop));
}

}  // namespace advent_of_code
