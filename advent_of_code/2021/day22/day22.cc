#include "advent_of_code/2021/day22/day22.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/point3.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::Status Audit() {
  Cube c1({{10, 10, 10}, {12, 12, 12}});
  Cube c2({{11, 11, 11}, {13, 13, 13}});
  std::vector<Cube> removed;
  c1.SetDifference(c2, &removed);
  for (Point3 p : c1) {
    bool should_in = c1.Contains(p) && !c2.Contains(p);
    int in_count = 0;
    for (const Cube& r : removed) {
      if (r.Contains(p)) {
        ++in_count;
      }
    }
    if (in_count != should_in ? 1 : 0) {
      VLOG(1) << "Bad point: " << p
              << "; should=" << (should_in ? "in" : "out");
      return Error("Bad point");
    }
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_22::Part1(
    absl::Span<std::string_view> input) const {
  Cube bound{{-50, -50, -50}, {50, 50, 50}};
  CubeSet s;
  std::optional<absl::flat_hash_set<Point3>> map;
  if (run_audit()) {
    map = absl::flat_hash_set<Point3>{};
  }
  for (std::string_view line : input) {
    std::string_view type;
    int x0, x1, y0, y1, z0, z1;
    if (!RE2::FullMatch(
            line,
            "(on|off) "
            "x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)",
            &type, &x0, &x1, &y0, &y1, &z0, &z1)) {
      return Error("Bad line: ", line);
    }

    std::optional<Cube> c = bound.Intersect(Cube{{x0, y0, z0}, {x1, y1, z1}});
    if (!c) continue;
    if (type == "on") {
      if (map)
        for (Point3 p : *c) map->insert(p);
      s.Union(*c);
    } else if (type == "off") {
      if (map)
        for (Point3 p : *c) map->erase(p);
      s.SetDifference(*c);
    } else {
      return Error("Neither on nor off?");
    }
  }
  if (map) {
    if (map->size() != s.Volume()) {
      return Error("map->size() != s.Volumne(): ", map->size(),
                   " != ", s.Volume());
    }
  }
  return AdventReturn(s.Volume());
}

absl::StatusOr<std::string> Day_2021_22::Part2(
    absl::Span<std::string_view> input) const {
  if (run_audit()) {
    if (auto st = Audit(); !st.ok()) return st;
  }
  CubeSet s;
  for (std::string_view line : input) {
    std::string_view type;
    int x0, x1, y0, y1, z0, z1;
    if (!RE2::FullMatch(
            line,
            "(on|off) "
            "x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)",
            &type, &x0, &x1, &y0, &y1, &z0, &z1)) {
      return Error("Bad line: ", line);
    }
    Cube c{{x0, y0, z0}, {x1, y1, z1}};
    if (type == "on") {
      s.Union(c);
    } else if (type == "off") {
      s.SetDifference(c);
    } else {
      return Error("Neither on nor off?");
    }
  }
  return AdventReturn(s.Volume());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/22, []() {
  return std::unique_ptr<AdventDay>(new Day_2021_22());
});

}  // namespace advent_of_code
