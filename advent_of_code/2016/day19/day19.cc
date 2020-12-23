#include "advent_of_code/2016/day19/day19.h"

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

absl::StatusOr<std::vector<std::string>> Day19_2016::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  std::vector<int> cycle(size, 0);
  for (int i = 0; i < cycle.size(); ++i) cycle[i] = i + 1;
  int offset = 0;
  while (cycle.size() > 1) {
    VLOG_IF(1, cycle.size() < 100) << absl::StrJoin(cycle, ",");
    std::vector<int> next_cycle;
    next_cycle.reserve(cycle.size() / 2 + 1);
    for (int i = offset; i < cycle.size(); i += 2) {
      next_cycle.push_back(cycle[i]);
    }
    offset = (offset + cycle.size()) % 2;
    cycle = next_cycle;
  }
  if (cycle.size() != 1) return Error("WTF?");
  return IntReturn(cycle[0]);
}

absl::StatusOr<std::vector<std::string>> Day19_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
