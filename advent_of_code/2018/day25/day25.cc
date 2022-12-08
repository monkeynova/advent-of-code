#include "advent_of_code/2018/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int CountConstellations(const std::vector<Point4>& points) {
  Graph<Point4> g;

  std::vector<std::string> named;
  for (int i = 0; i < points.size(); ++i) named.push_back(absl::StrCat(i));

  for (int i = 0; i < points.size(); ++i) {
    g.AddNode(named[i], points[i]);
    for (int j = 0; j < i; ++j) {
      if ((points[i] - points[j]).dist() <= 3) {
        g.AddEdge(named[i], named[j]);
      }
    }
  }

  return g.Forest().size();
}

}  // namespace

absl::StatusOr<std::string> Day_2018_25::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Point4> points;
  for (absl::string_view row : input) {
    Point4 p;
    if (!RE2::FullMatch(row, "(-?\\d+,-?\\d+,-?\\d+,-?\\d+)", p.Capture())) {
      return Error("Bad input: ", row);
    }
    points.push_back(p);
  }

  return IntReturn(CountConstellations(points));
}

absl::StatusOr<std::string> Day_2018_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
