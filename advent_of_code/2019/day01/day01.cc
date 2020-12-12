#include "advent_of_code/2019/day01/day01.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"

absl::StatusOr<std::vector<std::string>> Day01_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> costs = ParseAsInts(input);
  if (!costs.ok()) return costs.status();

  int total_fuel = 0;

  for (const auto& module_cost : *costs) {
    total_fuel += module_cost / 3 - 2;
  }

  return IntReturn(total_fuel);
}

absl::StatusOr<std::vector<std::string>> Day01_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> costs = ParseAsInts(input);
  if (!costs.ok()) return costs.status();

  int total_fuel = 0;

  for (const auto& module_cost : *costs) {
    int module_fuel = module_cost / 3 - 2;
    int fuel_fuel = module_fuel;
    while (fuel_fuel / 3 - 2 > 0) {
      fuel_fuel = fuel_fuel / 3 - 2;
      module_fuel += fuel_fuel;
    }
    total_fuel += module_fuel;
  }

  return IntReturn(total_fuel);
}
