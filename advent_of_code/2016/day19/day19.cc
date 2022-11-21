// http://adventofcode.com/2016/day/19

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

absl::StatusOr<std::string> Day_2016_19::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  std::vector<int> cycle(size, 0);
  for (int i = 0; i < cycle.size(); ++i) cycle[i] = i + 1;
  int offset = 0;
  while (cycle.size() > 1) {
    VLOG_IF(2, cycle.size() < 100) << absl::StrJoin(cycle, ",");
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

absl::StatusOr<std::string> Day_2016_19::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  std::vector<int> cycle(size, 0);
  for (int i = 0; i < cycle.size(); ++i) cycle[i] = i + 1;
  int offset = cycle.size() / 2;
  int pos = cycle.size() % 2;
  while (cycle.size() > 1) {
    VLOG_IF(2, cycle.size() < 100) << absl::StrJoin(cycle, ",");
    std::vector<int> next_cycle;
    for (int i = 0; i < offset; ++i) {
      next_cycle.push_back(cycle[i]);
    }
    for (int i = offset; i < cycle.size(); ++i) {
      if (pos == 2) next_cycle.push_back(cycle[i]);
      pos = (pos + 1) % 3;
    }
    offset = 0;
    if (next_cycle.empty()) {
      cycle = {cycle.back()};
    } else {
      cycle = next_cycle;
    }
  }
  if (cycle.size() != 1) return Error("WTF?");
  return IntReturn(cycle[0]);
}

}  // namespace advent_of_code
