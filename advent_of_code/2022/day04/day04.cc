// http://adventofcode.com/2022/day/04

#include "advent_of_code/2022/day04/day04.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2022_04::Part1(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view line : input) {
    int p1s, p1e, p2s, p2e;
    if (!RE2::FullMatch(line, "(\\d+)-(\\d+),(\\d+)-(\\d+)", 
                        &p1s, &p1e, &p2s, &p2e)) {
      return Error("Bad line: ", line);
    }
    if (p1s > p1e) return Error("Bad range (1): ", line);
    if (p2s > p2e) return Error("Bad range (2): ", line);
    if (p1s >= p2s && p1e <= p2e) ++count;
    else if (p2s >= p1s && p2e <= p1e) ++count;
  }

  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2022_04::Part2(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view line : input) {
    int p1s, p1e, p2s, p2e;
    if (!RE2::FullMatch(line, "(\\d+)-(\\d+),(\\d+)-(\\d+)", 
                        &p1s, &p1e, &p2s, &p2e)) {
      return Error("Bad line: ", line);
    }
    if (p1s > p1e) return Error("Bad range (1): ", line);
    if (p2s > p2e) return Error("Bad range (2): ", line);
    if (p1e >= p2s && p2e >= p1s) ++count;
  }

  return IntReturn(count);
}

}  // namespace advent_of_code
