#include "advent_of_code/2016/day15/day15.h"

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

struct Disc {
  int64_t index;
  int64_t start;
  int64_t positions;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_15::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Disc> discs;
  for (absl::string_view str : input) {
    Disc d;
    if (!RE2::FullMatch(str,
                        "Disc #(\\d+) has (\\d+) positions; at time=0, it is "
                        "at position (\\d+).",
                        &d.index, &d.positions, &d.start)) {
      return Error("Bad description: ", str);
    }
    discs.push_back(d);
  }

  int64_t delta = 1;
  int64_t start = 0;
  for (Disc d : discs) {
    VLOG(1) << "Disk: {" << d.index << "," << d.start << "," << d.positions
            << "}";
    VLOG(1) << "  start=" << start;
    while ((start + d.start + d.index) % d.positions != 0) {
      start += delta;
    }
    VLOG(1) << "  start=" << start;
    delta *= d.positions / std::gcd(d.positions, delta);
  }

  return AdventReturn(start);
}

absl::StatusOr<std::string> Day_2016_15::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Disc> discs;
  for (absl::string_view str : input) {
    Disc d;
    if (!RE2::FullMatch(str,
                        "Disc #(\\d+) has (\\d+) positions; at time=0, it is "
                        "at position (\\d+).",
                        &d.index, &d.positions, &d.start)) {
      return Error("Bad description: ", str);
    }
    discs.push_back(d);
  }
  discs.push_back({.index = static_cast<int64_t>(discs.size() + 1),
                   .start = 0,
                   .positions = 11});

  int64_t delta = 1;
  int64_t start = 0;
  for (Disc d : discs) {
    VLOG(1) << "Disk: {" << d.index << "," << d.start << "," << d.positions
            << "}";
    VLOG(1) << "  start=" << start;
    while ((start + d.start + d.index) % d.positions != 0) {
      start += delta;
    }
    VLOG(1) << "  start=" << start;
    delta *= d.positions / std::gcd(d.positions, delta);
  }

  return AdventReturn(start);
}

}  // namespace advent_of_code
