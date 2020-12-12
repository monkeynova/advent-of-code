#include "advent_of_code/2015/day02/day02.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day02_2015::Part1(
    const std::vector<absl::string_view>& input) const {
  int need = 0;
  for (absl::string_view dims : input) {
    int l, w, h;
    if (!RE2::FullMatch(dims, "(\\d+)x(\\d+)x(\\d+)", &l, &w, &h)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad input: ", dims));
    }
    need += 2 * l * w + 2 * l * h + 2 * w * h;
    need += std::min(l * w, std::min(l * h, w * h));
  }
  return IntReturn(need);
}

absl::StatusOr<std::vector<std::string>> Day02_2015::Part2(
    const std::vector<absl::string_view>& input) const {
  int need = 0;
  for (absl::string_view dims : input) {
    int l, w, h;
    if (!RE2::FullMatch(dims, "(\\d+)x(\\d+)x(\\d+)", &l, &w, &h)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad input: ", dims));
    }
    need += l * w * h;
    need += 2 * (l + w + h) - 2 * std::max(l, std::max(w, h));
  }
  return IntReturn(need);
}
