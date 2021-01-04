#include "advent_of_code/2018/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct NanoBot {
  Point3 p;
  int64_t r;
};

absl::StatusOr<std::vector<NanoBot>> Parse(absl::Span<absl::string_view> input) {
  std::vector<NanoBot> ret;
  for (absl::string_view row : input) {
    NanoBot nb;
    if (!RE2::FullMatch(row, "pos=<(-?\\d+),(-?\\d+),(-?\\d+)>, r=(-?\\d+)",
                        &nb.p.x, &nb.p.y, &nb.p.z, &nb.r)) {
      return AdventDay::Error("Bad input: ", row);
    }
    ret.push_back(nb);
  }
  return ret;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day23_2018::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<NanoBot>> bots = Parse(input);
  if (!bots.ok()) return bots.status();
  if (bots->empty()) return Error("No bots");
 
  NanoBot strongest = *bots->begin();
  for (const NanoBot& nb : *bots) {
    if (nb.r > strongest.r) strongest = nb;
  }
  int in_range = 0;
  for (const NanoBot& nb : *bots) {
    if ((nb.p - strongest.p).dist() <= strongest.r) {
      ++in_range;
    }
  }

  return IntReturn(in_range);
}

absl::StatusOr<std::vector<std::string>> Day23_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
