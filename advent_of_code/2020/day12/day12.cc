#include "advent_of_code/2020/day12/day12.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_12::Part1(
    absl::Span<absl::string_view> input) const {
  Point p{0, 0};
  Point dir = Cardinal::kEast;
  for (absl::string_view cmd : input) {
    int v;
    char c = cmd[0];
    if (!absl::SimpleAtoi(cmd.substr(1), &v))
      return absl::InvalidArgumentError("Bad parse.");
    switch (c) {
      case 'N':
        p += Cardinal::kNorth * v;
        break;
      case 'S':
        p += Cardinal::kSouth * v;
        break;
      case 'E':
        p += Cardinal::kEast * v;
        break;
      case 'W':
        p += Cardinal::kWest * v;
        break;
      case 'F':
        p += dir * v;
        break;
      case 'L': {
        switch (v % 360) {
          case 90:
            dir = dir.rotate_left();
            break;
          case 180:
            dir = -dir;
            break;
          case 270:
            dir = dir.rotate_right();
            break;
          case 0:
            break;
          default:
            return absl::InvalidArgumentError("Bad rotation");
        }
        break;
      }
      case 'R': {
        switch (v % 360) {
          case 90:
            dir = dir.rotate_right();
            break;
          case 180:
            dir = -dir;
            break;
          case 270:
            dir = dir.rotate_left();
            break;
          case 0:
            break;
          default:
            return absl::InvalidArgumentError("Bad rotation");
        }
        break;
      }
      default:
        return absl::InvalidArgumentError("Bad command");
    }
  }
  return IntReturn(p.dist());
}

absl::StatusOr<std::string> Day_2020_12::Part2(
    absl::Span<absl::string_view> input) const {
  Point ship{0, 0};
  Point waypoint = 10 * Cardinal::kEast + Cardinal::kNorth;
  for (absl::string_view cmd : input) {
    int v;
    char c = cmd[0];
    if (!absl::SimpleAtoi(cmd.substr(1), &v))
      return absl::InvalidArgumentError("Bad parse.");
    switch (c) {
      case 'N':
        waypoint += Cardinal::kNorth * v;
        break;
      case 'S':
        waypoint += Cardinal::kSouth * v;
        break;
      case 'E':
        waypoint += Cardinal::kEast * v;
        break;
      case 'W':
        waypoint += Cardinal::kWest * v;
        break;
      case 'F': {
        ship += v * waypoint;
        break;
      }
      case 'L': {
        switch (v % 360) {
          case 90:
            waypoint = waypoint.rotate_left();
            break;
          case 180:
            waypoint = -waypoint;
            break;
          case 270:
            waypoint = waypoint.rotate_right();
            break;
          case 0:
            break;
          default:
            return absl::InvalidArgumentError(
                absl::StrCat("Bad rotation: ", v));
        }
        break;
      }
      case 'R': {
        switch (v % 360) {
          case 90:
            waypoint = waypoint.rotate_right();
            break;
          case 180:
            waypoint = -waypoint;
            break;
          case 270:
            waypoint = waypoint.rotate_left();
            break;
          case 0:
            break;
          default:
            return absl::InvalidArgumentError(
                absl::StrCat("Bad rotation: ", v));
        }
        break;
      }
      default:
        return absl::InvalidArgumentError("Bad command");
    }
  }
  return IntReturn(ship.dist());
}

}  // namespace advent_of_code
