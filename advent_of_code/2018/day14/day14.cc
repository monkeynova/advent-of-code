#include "advent_of_code/2018/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2018_14::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> ints, ParseAsInts(input));
  if (ints.size() != 1) return Error("Bad size");
  int rounds = ints[0];

  std::string tmp = "37";
  int elf1_pos = 0;
  int elf2_pos = 1;
  while (tmp.size() < rounds + 10) {
    int sum = tmp[elf1_pos] + tmp[elf2_pos] - 2 * '0';
    if (sum >= 100) return Error("Bad sum!");
    if (sum >= 10) {
      tmp.append(1, (sum / 10) + '0');
    }
    tmp.append(1, (sum % 10) + '0');
    elf1_pos = (elf1_pos + 1 + tmp[elf1_pos] - '0') % tmp.size();
    elf2_pos = (elf2_pos + 1 + tmp[elf2_pos] - '0') % tmp.size();
  }

  return tmp.substr(rounds, 10);
}

absl::StatusOr<std::string> Day_2018_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> ints, ParseAsInts(input));
  if (ints.size() != 1) return Error("Bad size");
  int rounds = ints[0];

  std::string needle = absl::StrCat(rounds);

  std::string tmp = "37";
  int elf1_pos = 0;
  int elf2_pos = 1;
  while (true) {
    int sum = tmp[elf1_pos] + tmp[elf2_pos] - 2 * '0';
    if (sum >= 100) return Error("Bad sum!");
    if (sum >= 10) {
      tmp.append(1, (sum / 10) + '0');
    }
    tmp.append(1, (sum % 10) + '0');
    elf1_pos = (elf1_pos + 1 + tmp[elf1_pos] - '0') % tmp.size();
    elf2_pos = (elf2_pos + 1 + tmp[elf2_pos] - '0') % tmp.size();
    size_t off = tmp.find(needle, tmp.size() - 2 * needle.size());
    if (off != std::string::npos) return AdventReturn(off);
  }

  return Error("Left infinite loop");
  ;
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/14,
    []() { return std::unique_ptr<AdventDay>(new Day_2018_14()); });

}  // namespace advent_of_code
