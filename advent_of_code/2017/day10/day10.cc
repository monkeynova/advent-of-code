#include "advent_of_code/2017/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/knot_hash.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::vector<std::string>> Day10_2017::Part1(
    absl::Span<absl::string_view> input) const {
  {
    // TODO(@monkeynova): Move to knot_hash_test.cc?
    std::vector<int> loop(5);
    for (int i = 0; i < loop.size(); ++i) loop[i] = i;
    KnotHashRunLoop({3, 4, 1, 5}, &loop);
    if (loop[0] * loop[1] != 12) {
      return Error("Test case failed");
    }
  }

  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> length_strs = absl::StrSplit(input[0], ",");
  absl::StatusOr<std::vector<int64_t>> lengths = ParseAsInts(length_strs);

  std::vector<int> loop(256);
  for (int i = 0; i < loop.size(); ++i) loop[i] = i;
  KnotHashRunLoop(*lengths, &loop);

  return IntReturn(loop[0] * loop[1]);
}

absl::StatusOr<std::vector<std::string>> Day10_2017::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");

  return std::vector<std::string>{KnotHash(input[0])};
}

}  // namespace advent_of_code
