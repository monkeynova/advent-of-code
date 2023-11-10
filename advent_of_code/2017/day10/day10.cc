#include "advent_of_code/2017/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/knot_hash.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2017_10::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<std::string_view> length_strs = absl::StrSplit(input[0], ",");
  absl::StatusOr<std::vector<int64_t>> lengths_long = ParseAsInts(length_strs);

  std::string lengths;
  lengths.resize(lengths_long->size());
  for (int i = 0; i < lengths_long->size(); ++i) {
    if ((*lengths_long)[i] > 255) return Error("Bad length");
    if ((*lengths_long)[i] < 0) return Error("Bad length");
    lengths[i] = (*lengths_long)[i];
  }
  std::vector<unsigned char> loop(256);
  for (int i = 0; i < loop.size(); ++i) loop[i] = i;
  KnotHash().RunLoop(lengths, &loop);

  return AdventReturn(loop[0] * loop[1]);
}

absl::StatusOr<std::string> Day_2017_10::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");

  return KnotHash().DigestHex(input[0]);
}

}  // namespace advent_of_code
