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

absl::StatusOr<int64_t> DecodeSnafu(std::string_view line) {
  int64_t num = 0;
  for (char c : line) {
    int next = 0;
    switch (c) {
      case '0':
        next = 0;
        break;
      case '1':
        next = 1;
        break;
      case '2':
        next = 2;
        break;
      case '-':
        next = -1;
        break;
      case '=':
        next = -2;
        break;
      default:
        return Error("Bad snafu encoding: ", line);
    }
    num = num * 5 + next;
  }
  return num;
}

absl::StatusOr<std::string> EncodeSnafu(int64_t total) {
  std::string ret;
  for (int64_t c = total; c; c /= 5) {
    switch (c % 5) {
      case 0:
        ret.append(1, '0');
        break;
      case 1:
        ret.append(1, '1');
        break;
      case 2:
        ret.append(1, '2');
        break;
      case 3:
        ret.append(1, '=');
        c += 5;
        break;
      case 4:
        ret.append(1, '-');
        c += 5;
        break;
      default:
        return Error("MOD 5 not handled");
    }
  }
  absl::c_reverse(ret);
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_25::Part1(
    absl::Span<std::string_view> input) const {
  int64_t total = 0;
  for (std::string_view line : input) {
    absl::StatusOr<int64_t> decoded = DecodeSnafu(line);
    if (!decoded.ok()) return decoded.status();
    total += *decoded;
  }
  return EncodeSnafu(total);
}

absl::StatusOr<std::string> Day_2022_25::Part2(
    absl::Span<std::string_view> input) const {
  return "Merry Christmas!";
}

}  // namespace advent_of_code
