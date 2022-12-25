// http://adventofcode.com/2022/day/25

#include "advent_of_code/2022/day25/day25.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int64_t Snafu(absl::string_view line) {
    int64_t num = 0;
    for (char c : line) {
      int next = 0;
      switch (c) {
        case '0': next = 0; break;
        case '1': next = 1; break;
        case '2': next = 2; break;
        case '-': next = -1; break;
        case '=': next = -2; break;
        default: LOG(FATAL) << "Bad line: " << line;
      }
      num = num * 5 + next;
    }
  return num;
}

std::string UnSnafu(int64_t total) {
  std::string ret;
  for (int64_t c = total; c; c /= 5) {
    ret.append(1, (c % 5) + '0');
  }
  for (int i = 0; i < ret.size(); ++i) {
    VLOG(1) << ret;
    if (ret[i] > '2') {
      if (i == ret.size() - 1) {
        ret.append(1, '0');
      }
      for (int j = i + 1; ++ret[j] == '5'; ++j) {
        VLOG(1) << "  " << ret;
        ret[j] = '0';
        if (j == ret.size() - 1) {
          ret.append(1, '0');
        }
      }
      VLOG(1) << "  " << ret;
      if (ret[i] == '3') {
        ret[i] = '=';
      } else if (ret[i] == '4') {
        ret[i] = '-';
      } else {
        LOG(FATAL) << "Can't foo " << ret;
      }
    }
  }
  absl::c_reverse(ret);
return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_25::Part1(
    absl::Span<absl::string_view> input) const {
  int64_t total = 0;
  for (absl::string_view line : input) {
    total += Snafu(line);
  }
  CHECK_EQ(Snafu(UnSnafu(total)), total);
  return UnSnafu(total);
}

absl::StatusOr<std::string> Day_2022_25::Part2(
    absl::Span<absl::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
