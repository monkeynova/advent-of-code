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

Point DragPoint(Point head, Point tail) {
  static absl::flat_hash_map<Point, Point> dragdir = {
      // Distance 2.
      {Point{2, 0}, Point{1, 0}},
      {Point{-2, 0}, Point{-1, 0}},
      {Point{0, 2}, Point{0, 1}},
      {Point{0, -2}, Point{0, -1}},
      // Distance sqrt(5).
      {Point{2, 1}, Point{1, 1}},
      {Point{1, 2}, Point{1, 1}},
      {Point{2, -1}, Point{1, -1}},
      {Point{1, -2}, Point{1, -1}},
      {Point{-2, 1}, Point{-1, 1}},
      {Point{-1, 2}, Point{-1, 1}},
      {Point{-2, -1}, Point{-1, -1}},
      {Point{-1, -2}, Point{-1, -1}},
  };
  auto it = dragdir.find(head - tail);
  if (it == dragdir.end()) {
    CHECK(d.dist() < 2);
    return tail;
  }
  return tail + it->second;
}

absl::StatusOr<int> DragRope(absl::Span<absl::string_view> input,
                             int ropesize) {
  std::vector<Point> rope(ropesize, Point{0, 0});
  absl::flat_hash_set<Point> tail_seen = {rope.back()};
  static absl::flat_hash_map<char, Point> char2dir = {
      // Up or down.
      {'U', Cardinal::kNorth},
      {'D', Cardinal::kSouth},
      // Left or right.
      {'L', Cardinal::kWest},
      {'R', Cardinal::kEast},
  };
  for (absl::string_view line : input) {
    char dir_char;
    int dist;
    if (!RE2::FullMatch(line, "([RULD]) (\\d+)", &dir_char, &dist)) {
      return Error("Bad line: ", line);
    }
    CHECK(char2dir.contains(dir_char));
    Point head_dir = char2dir[dir_char];

    for (int i = 0; i < dist; ++i) {
      rope[0] += head_dir;
      for (int i = 1; i < rope.size(); ++i) {
        rope[i] = DragPoint(rope[i - 1], rope[i]);
      }
      tail_seen.insert(rope.back());
    }
  }
  return tail_seen.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2022_09::Part1(
    absl::Span<absl::string_view> input) const {
  return IntReturn(DragRope(input, 2));
}

absl::StatusOr<std::string> Day_2022_09::Part2(
    absl::Span<absl::string_view> input) const {
  return IntReturn(DragRope(input, 10));
}

}  // namespace advent_of_code
