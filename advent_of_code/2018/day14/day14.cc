#include "advent_of_code/2018/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day_2018_14::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> ints = ParseAsInts(input);
  if (!ints.ok()) return ints.status();
  if (ints->size() != 1) return Error("Bad size");
  int rounds = ints->back();

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

  return std::vector<std::string>{tmp.substr(rounds, 10)};
}

absl::StatusOr<std::vector<std::string>> Day_2018_14::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> ints = ParseAsInts(input);
  if (!ints.ok()) return ints.status();
  if (ints->size() != 1) return Error("Bad size");
  int rounds = ints->back();

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
    if (off != std::string::npos) return IntReturn(off);
  }

  return Error("Left infinite loop");
  ;
}

}  // namespace advent_of_code
