#include "advent_of_code/2017/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day13_2017::Part1(
    absl::Span<absl::string_view> input) const {
  int severity = 0;
  for (absl::string_view row : input) {
    int depth;
    int range;
    if (!RE2::FullMatch(row, "(\\d+): (\\d+)", &depth, &range)) {
      return Error("Bad row: ", row);
    }
    int pos = 0;
    int to_move = depth;
    while (to_move > 0) {
      int delta = std::min(to_move, range - 1);
      if (pos == 0) pos += delta;
      else if (pos == range - 1) pos -= delta;
      else return Error("Consistency error");
      to_move -= delta;
    }
    VLOG(2) << "depth=" << depth << "; range=" << range << "; pos=" << pos;
    if (pos == 0) {
      severity += depth * range;
    }
  }
  return IntReturn(severity);
}

absl::StatusOr<std::vector<std::string>> Day13_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
