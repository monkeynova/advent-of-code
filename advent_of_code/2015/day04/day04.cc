#include "advent_of_code/2015/day04/day04.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/md5.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_04::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  for (int i = 0;; ++i) {
    VLOG(2) << i;
    MD5 md5;
    std::string str = absl::StrCat(input[0], i);
    if (VLOG_IS_ON(2)) {
      std::string_view hex = md5.DigestHex(str);
      VLOG(2) << "MD5(" << str << "): " << hex;
    }
    std::string_view md5_result = md5.Digest(str);
    if (md5_result[0] == 0 && md5_result[1] == 0 && (md5_result[2] >> 4) == 0) {
      return AdventReturn(i);
    }
  }
  return absl::InvalidArgumentError("Unreachable");
}

absl::StatusOr<std::string> Day_2015_04::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  for (int i = 0;; ++i) {
    VLOG(2) << i;
    MD5 md5;
    std::string str = absl::StrCat(input[0], i);
    if (VLOG_IS_ON(2)) {
      std::string_view hex = md5.DigestHex(str);
      VLOG(2) << "MD5(" << str << "): " << hex;
    }
    std::string_view md5_result = md5.Digest(str);
    if (md5_result[0] == 0 && md5_result[1] == 0 && md5_result[2] == 0) {
      return AdventReturn(i);
    }
  }
  return absl::InvalidArgumentError("Unreachable");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/4,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_04()); });

}  // namespace advent_of_code
