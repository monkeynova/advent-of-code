#include "advent_of_code/2016/day05/day05.h"

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

absl::StatusOr<std::string> Day_2016_05::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::string out;
  for (int i = 0;; ++i) {
    MD5 digest;
    std::string str = absl::StrCat(input[0], i);
    std::string_view md5_result = digest.Digest(str);
    VLOG(2) << "MD5(" << str << "): " << md5_result;
    if (md5_result[0] == 0 && md5_result[1] == 0 && (md5_result[2] >> 4) == 0) {
      int add = md5_result[2] & 0xf;
      if (add >= 10)
        out.append(1, add + 'a' - 10);
      else
        out.append(1, add + '0');
      VLOG(1) << "Adding: " << out.substr(out.size() - 1) << " (" << i << ")";
      if (out.size() == 8) break;
    }
  }

  return out;
}

absl::StatusOr<std::string> Day_2016_05::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::string out;
  out.resize(8, '_');
  int added = 0;
  for (int i = 0;; ++i) {
    MD5 digest;
    std::string str = absl::StrCat(input[0], i);
    std::string_view md5_result = digest.Digest(str);
    VLOG(2) << "MD5(" << str << "): " << md5_result;
    if (md5_result[0] == 0 && md5_result[1] == 0 && (md5_result[2] >> 4) == 0) {
      int pos = md5_result[2] & 0xf;
      char add = (md5_result[3] >> 4) & 0xf;
      if (add >= 10)
        add += 'a' - 10;
      else
        add += '0';
      if (pos < 8 && out[pos] == '_') {
        out[pos] = add;
        VLOG(1) << "Adding: " << pos << ", " << out.substr(pos, 1) << " (" << i
                << ")";
        ++added;
        if (added == 8) break;
      }
    }
  }

  return out;
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/5,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_05()); });

}  // namespace advent_of_code
