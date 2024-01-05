// http://adventofcode.com/2023/day/18

#include "advent_of_code/2023/day18/day18.h"

#include "absl/log/log.h"
#include "advent_of_code/point.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2023_18::Part1(
    absl::Span<std::string_view> input) const {
  Point cur = Cardinal::kOrigin;
  std::vector<Point> loop;
  loop.push_back(cur);
  for (std::string_view line : input) {
    Tokenizer tok(line);
    std::string_view dir_str = tok.Next();
    ASSIGN_OR_RETURN(int dist, tok.NextInt());
    Point dir;
    if (dir_str == "R") {
      dir = Cardinal::kEast;
    } else if (dir_str == "U") {
      dir = Cardinal::kNorth;
    } else if (dir_str == "D") {
      dir = Cardinal::kSouth;
    } else if (dir_str == "L") {
      dir = Cardinal::kWest;
    } else {
      return Error("Bad dir");
    }
    cur = cur + dir * dist;
    loop.push_back(cur);
  }
  return AdventReturn(LoopArea(loop, /*count_perimeter=*/true));
}

absl::StatusOr<std::string> Day_2023_18::Part2(
    absl::Span<std::string_view> input) const {
  VLOG(1) << "Start";
  Point cur = Cardinal::kOrigin;
  std::vector<Point> loop;
  loop.push_back(cur);
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
      const std::array<Point, 4> kDirs = {Cardinal::kEast, Cardinal::kSouth,
                                          Cardinal::kWest, Cardinal::kNorth};
      dir = kDirs[str[str.size() - 2] - '0'];
    } else {
      return Error("Bad dir");
    }
    cur = cur + dir * dist;
    loop.push_back(cur);
  }
  return AdventReturn(LoopArea(loop, /*count_perimeter=*/true));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2023, /*day=*/18,
    []() { return std::unique_ptr<AdventDay>(new Day_2023_18()); });

}  // namespace advent_of_code
