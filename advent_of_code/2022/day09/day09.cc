#include "advent_of_code/2022/day09/day09.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

Point UpdateTail(Point head, Point tail) {
    Point d = head - tail;
    if (d.x == 2 && d.y == 0) ++tail.x;
    else if (d.x == -2 && d.y == 0) --tail.x;
    else if (d.y == 2 && d.x == 0) ++tail.y;
    else if (d.y == -2 && d.x == 0) --tail.y;
    else if (d.dist() > 2) {
      d.x /= abs(d.x);
      d.y /= abs(d.y);
      tail += d;
    }
  return tail;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_09::Part1(
    absl::Span<absl::string_view> input) const {
  Point tail = {0, 0};
  Point head = {0, 0};
  absl::flat_hash_set<Point> tail_seen = {tail};
  for (absl::string_view line : input) {
    absl::string_view dir;
    int dist;
    if (!RE2::FullMatch(line, "([RULD]) (\\d+)", &dir, &dist)) {
      return Error("Bad line: ", line);
    }
    for (int i = 0; i < dist; ++i) {
      if (dir == "U") head = head + Cardinal::kNorth;
      else if (dir == "D") head = head + Cardinal::kSouth;
      else if (dir == "L") head = head + Cardinal::kWest;
      else if (dir == "R") head = head + Cardinal::kEast;
      else return Error("Bad dir!");
  
      tail = UpdateTail(head, tail);
      //VLOG(1) << "H: " << head;
      //VLOG(1) << "T: " << tail;
      tail_seen.insert(tail);
    }
  }
  return IntReturn(tail_seen.size());
}

absl::StatusOr<std::string> Day_2022_09::Part2(
    absl::Span<absl::string_view> input) const {
  Point tail = {0, 0};
  std::vector<Point> mid(8, Point{0, 0});
  Point head = {0, 0};
  absl::flat_hash_set<Point> tail_seen = {Point{0, 0}};
  for (absl::string_view line : input) {
    absl::string_view dir;
    int dist;
    if (!RE2::FullMatch(line, "([RULD]) (\\d+)", &dir, &dist)) {
      return Error("Bad line: ", line);
    }
    for (int i = 0; i < dist; ++i) {
      if (dir == "U") head = head + Cardinal::kNorth;
      else if (dir == "D") head = head + Cardinal::kSouth;
      else if (dir == "L") head = head + Cardinal::kWest;
      else if (dir == "R") head = head + Cardinal::kEast;
      else return Error("Bad dir!");
  
      mid[0] = UpdateTail(head, mid[0]);
      for (int i = 1; i < 8; ++i) {
        mid[i] = UpdateTail(mid[i - 1], mid[i]);
      }
      tail = UpdateTail(mid.back(), tail);
      //VLOG(1) << "H: " << head;
      //VLOG(1) << "T: " << tail;
      tail_seen.insert(tail);
    }
  }
  return IntReturn(tail_seen.size());
}

}  // namespace advent_of_code
