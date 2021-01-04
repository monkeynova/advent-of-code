#include "advent_of_code/2018/day25/day25.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int CountConstellations(const std::vector<Point4>& points) {
  DirectedGraph<Point4> g;

  std::vector<std::string> named;
  for (int i = 0; i < points.size(); ++i) named.push_back(absl::StrCat(i));

  for (int i = 0; i < points.size(); ++i) {
    g.AddNode(named[i], points[i]);
    for (int j = 0; j < i; ++j) {
      if ((points[i] - points[j]).dist() <= 3) {
        g.AddEdge(named[i], named[j]);
        g.AddEdge(named[j], named[i]);
      }
    }
  }

  absl::flat_hash_set<absl::string_view> to_assign = g.nodes();
  int constellations = 0;
  while (!to_assign.empty()) {
    ++constellations;
    std::deque<absl::string_view> frontier = {*to_assign.begin()};
    while (!frontier.empty()) {
      absl::string_view name = frontier.front();
      frontier.pop_front();
      to_assign.erase(name);
      const std::vector<absl::string_view>* incoming = g.Incoming(name);
      if (incoming != nullptr) {
        for (absl::string_view tmp : *incoming) {
          if (to_assign.contains(tmp)) frontier.push_back(tmp);
        }
      }
    }
  }

  return constellations;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day25_2018::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Point4> points;
  for (absl::string_view row : input) {
    Point4 p;
    if (!RE2::FullMatch(row, "(-?\\d+),(-?\\d+),(-?\\d+),(-?\\d+)", &p.x, &p.y,
                        &p.z, &p.w)) {
      return Error("Bad input: ", row);
    }
    points.push_back(p);
  }

  return IntReturn(CountConstellations(points));
}

absl::StatusOr<std::vector<std::string>> Day25_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return std::vector<std::string>{"Merry Christmas!"};
}

}  // namespace advent_of_code
