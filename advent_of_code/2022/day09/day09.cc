#include "advent_of_code/2022/day09/day09.h"

#include "absl/algorithm/container.h"
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

namespace {

int Norm2(Point p) { return p.x * p.x + p.y * p.y; }

int Sign(int x) { return x > 0 ? 1 : x < 0 ? -1 : 0; }
Point Sign(Point p) { return {Sign(p.x), Sign(p.y)}; }

Point DragPoint(Point head, Point tail) {
  Point delta = head - tail;
  if (Norm2(delta) < 4) return tail;
  return tail + Sign(delta);
}

absl::StatusOr<int> DragRope(absl::Span<absl::string_view> input,
                             int rope_len) {
  static absl::flat_hash_map<char, Point> char2dir = {
      // Up or down.
      {'U', Cardinal::kNorth},
      {'D', Cardinal::kSouth},
      // Left or right.
      {'L', Cardinal::kWest},
      {'R', Cardinal::kEast},
  };

  std::vector<Point> rope(rope_len, Point{0, 0});
  absl::flat_hash_set<Point> tail_seen = {rope.back()};
  for (absl::string_view line : input) {
    char dir_char;
    int dist;
    static LazyRE2 parse_re = {"([RULD]) (\\d+)"};
    if (!RE2::FullMatch(line, *parse_re, &dir_char, &dist)) {
      return Error("Bad line: ", line);
    }
    if (!char2dir.contains(dir_char)) {
      return Error("No direction for: ", absl::string_view(&dir_char, 1));
    }
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
  return AdventReturn(DragRope(input, /*rope_len=*/2));
}

absl::StatusOr<std::string> Day_2022_09::Part2(
    absl::Span<absl::string_view> input) const {
  return AdventReturn(DragRope(input, /*rope_len=*/10));
}

}  // namespace advent_of_code
