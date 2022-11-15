#include "advent_of_code/2018/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Light {
  Point p;
  Point v;
};

}  // namespace

absl::StatusOr<std::string> Day_2018_10::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("No input");
  std::vector<Light> lights;
  for (absl::string_view row : input) {
    Light l;
    if (!RE2::FullMatch(row,
                        "position=<\\s*(-?\\d+,\\s*-?\\d+)> "
                        "velocity=<\\s*(-?\\d+,\\s*-?\\d+)>",
                        l.p.Capture(), l.v.Capture())) {
      return Error("Bad row: ", row);
    }
    lights.push_back(l);
  }
  int last_dist = -1;
  for (int i = 0; true; ++i) {
    PointRectangle r = {{lights[0].p.x, lights[0].p.y},
                        {lights[0].p.x, lights[0].p.y}};
    for (Light l : lights) r.ExpandInclude(l.p);
    if (last_dist != -1 && (r.max - r.min).dist() > last_dist) {
      break;
    }
    last_dist = (r.max - r.min).dist();
    for (Light& l : lights) l.p += l.v;
  }
  // Back up one step.
  for (Light& l : lights) l.p -= l.v;
  PointRectangle r = {{lights[0].p.x, lights[0].p.y},
                      {lights[0].p.x, lights[0].p.y}};
  for (Light l : lights) r.ExpandInclude(l.p);
  CharBoard b(r.max.x - r.min.x + 1, r.max.y - r.min.y + 1);
  for (Light l : lights) b[l.p - r.min] = '#';

  return BoardReturn(b);
}

absl::StatusOr<std::string> Day_2018_10::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.empty()) return Error("No input");
  std::vector<Light> lights;
  for (absl::string_view row : input) {
    Light l;
    if (!RE2::FullMatch(row,
                        "position=<\\s*(-?\\d+,\\s*-?\\d+)> "
                        "velocity=<\\s*(-?\\d+,\\s*-?\\d+)>",
                        l.p.Capture(), l.v.Capture())) {
      return Error("Bad row: ", row);
    }
    lights.push_back(l);
  }
  int last_dist = -1;
  int i;
  for (i = 0; true; ++i) {
    PointRectangle r = {{lights[0].p.x, lights[0].p.y},
                        {lights[0].p.x, lights[0].p.y}};
    for (Light l : lights) r.ExpandInclude(l.p);
    if (last_dist != -1 && (r.max - r.min).dist() > last_dist) {
      break;
    }
    last_dist = (r.max - r.min).dist();
    for (Light& l : lights) l.p += l.v;
  }
  // Back up one step.
  --i;
  for (Light& l : lights) l.p -= l.v;
  PointRectangle r = {{lights[0].p.x, lights[0].p.y},
                      {lights[0].p.x, lights[0].p.y}};
  for (Light l : lights) r.ExpandInclude(l.p);
  CharBoard b(r.max.x - r.min.x + 1, r.max.y - r.min.y + 1);
  for (Light l : lights) b[l.p - r.min] = '#';

  return IntReturn(i);
}

}  // namespace advent_of_code
