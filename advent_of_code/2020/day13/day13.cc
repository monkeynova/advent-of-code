#include "advent_of_code/2020/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day13_2020::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int start;
  if (!absl::SimpleAtoi(input[0], &start)) return Error("Bad input");
  int min_delta = std::numeric_limits<int>::max();
  int min_id;
  for (absl::string_view piece : absl::StrSplit(input[1], ",")) {
    if (piece == "x") continue;
    int id;
    if (!absl::SimpleAtoi(piece, &id)) return Error("Bad id:", id);
    int delta = id - (start % id);
    if (delta < min_delta) {
      min_delta = delta;
      min_id = id;
    }
  }
  return IntReturn(min_delta * min_id);
}

absl::StatusOr<std::vector<std::string>> Day13_2020::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}
