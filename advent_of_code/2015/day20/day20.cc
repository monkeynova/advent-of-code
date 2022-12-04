// http://adventofcode.com/2015/day/20

#include "advent_of_code/2015/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

int PresentCount(int house_num) {
  int presents = 1;  // 1st elf.
  if (house_num != 1) {
    // Elf #house_num
    presents += house_num;
  }
  for (int i = 2; i * i <= house_num; ++i) {
    if (house_num % i == 0) {
      presents += i;
      presents += house_num / i;
    }
  }

  return presents * 10;
}

int PresentCountPart2(int house_num) {
  int presents = 0;
  if (house_num <= 50) {
    // 1st elf.
    ++presents;
  }
  if (house_num != 1) {
    // Elf #house_num
    presents += house_num;
  }
  for (int i = 2; i * i <= house_num; ++i) {
    if (house_num % i == 0) {
      if (house_num / i <= 50) {
        presents += i;
      }
      if (i <= 50) {
        presents += house_num / i;
      }
    }
  }

  return presents * 11;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2015_20::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    std::vector<std::pair<int, int>> test_suite = {
        {1, 10}, {2, 30}, {6, 120}, {8, 150}};
    for (const auto& [house, presents] : test_suite) {
      if (PresentCount(house) != presents) {
        return Error("Bad PresentCount(", house, "): ", PresentCount(house),
                     " != ", presents);
      }
    }
  }

  absl::StatusOr<std::vector<int64_t>> present_count_list = ParseAsInts(input);
  if (!present_count_list.ok()) return present_count_list.status();
  if (present_count_list->size() != 1) return Error("Bad count");
  int present_count = (*present_count_list)[0];

  for (int house_num = 1; house_num < present_count / 10; ++house_num) {
    if (PresentCount(house_num) > present_count) return IntReturn(house_num);
  }

  return Error("Not found");
}

absl::StatusOr<std::string> Day_2015_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> present_count_list = ParseAsInts(input);
  if (!present_count_list.ok()) return present_count_list.status();
  if (present_count_list->size() != 1) return Error("Bad count");
  int present_count = (*present_count_list)[0];

  for (int house_num = 1; house_num < present_count / 11; ++house_num) {
    if (PresentCountPart2(house_num) > present_count)
      return IntReturn(house_num);
  }

  return Error("Not found");
}

}  // namespace advent_of_code
