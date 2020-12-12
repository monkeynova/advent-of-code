#include "advent_of_code/2020/day12/day12.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day12_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  Point p{0, 0};
  Point dir = Cardinal::kEast;
  for (absl::string_view cmd : input) {
    int v;
    char c = cmd[0];
    if (!absl::SimpleAtoi(cmd.substr(1), &v)) return absl::InvalidArgumentError("Bad parse.");
    switch (c) {
      case 'N': p += Cardinal::kNorth * v; break;
      case 'S': p += Cardinal::kSouth * v; break;
      case 'E': p += Cardinal::kEast * v; break;
      case 'W': p += Cardinal::kWest * v; break;
      case 'F': p += dir * v; break;
      case 'L': {
        switch(v % 360) {
          case 90: dir = Point{dir.y, -dir.x}; break;
          case 180: dir = Point{-dir.x, -dir.y}; break;
          case 270: dir = Point{-dir.y, dir.x}; break;
          case 0: break;
          default: return absl::InvalidArgumentError("Bad rotation");
        }
        break;
      }
      case 'R':  {
        switch(v % 360) {
          case 90: dir = Point{-dir.y, dir.x}; break;
          case 180: dir = Point{-dir.x, -dir.y}; break;
          case 270: dir = Point{dir.y, -dir.x}; break;
          case 0: break;
          default: return absl::InvalidArgumentError("Bad rotation");
        }
        break;
      }
      default: return absl::InvalidArgumentError("Bad command");
    }
  }
  return IntReturn(p.dist());
}

absl::StatusOr<std::vector<std::string>> Day12_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  Point ship{0, 0};
  Point waypoint = ship + (Cardinal::kEast * 10) + Cardinal::kNorth;
  for (absl::string_view cmd : input) {
    int v;
    char c = cmd[0];
    if (!absl::SimpleAtoi(cmd.substr(1), &v)) return absl::InvalidArgumentError("Bad parse.");
    switch (c) {
      case 'N': waypoint += Cardinal::kNorth * v; break;
      case 'S': waypoint += Cardinal::kSouth * v; break;
      case 'E': waypoint += Cardinal::kEast * v; break;
      case 'W': waypoint += Cardinal::kWest * v; break;
      case 'F': {
        Point delta = waypoint - ship;
        ship += delta * v;
        waypoint = ship + delta;
        break;
      }
      case 'L': {
        Point delta = waypoint - ship;
        switch(v % 360) {
          case 90: delta = Point{delta.y, -delta.x}; break;
          case 180: delta = Point{-delta.x, -delta.y}; break;
          case 270: delta = Point{-delta.y, delta.x}; break;
          case 0: break;
          default: return absl::InvalidArgumentError(absl::StrCat("Bad rotation: ", v));
        }
        waypoint = ship + delta;
        break;
      }
      case 'R':  {
        Point delta = waypoint - ship;
        switch(v % 360) {
          case 90: delta = Point{-delta.y, delta.x}; break;
          case 180: delta = Point{-delta.x, -delta.y}; break;
          case 270: delta = Point{delta.y, -delta.x}; break;
          case 0: break;
          default: return absl::InvalidArgumentError(absl::StrCat("Bad rotation: ", v));
        }
        waypoint = ship + delta;
        break;
      }
      default: return absl::InvalidArgumentError("Bad command");
    }
  }
  return IntReturn(ship.dist());
}
