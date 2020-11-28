#include "advent_of_code/2019/day1/day1.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"

absl::StatusOr<std::vector<std::string>> Day1_2019::Part1(const std::vector<absl::string_view>& input) {
  int total_fuel = 0;

  for (const auto& module_str : input) {
      if (module_str.empty()) continue;
      int module_cost;
      if (!absl::SimpleAtoi(module_str, &module_cost)) {
        return absl::InvalidArgumentError(absl::StrCat("Cannot parse as int: ", module_str));
      }
      total_fuel += module_cost / 3 - 2;
  }

  return std::vector<std::string>{absl::StrCat(total_fuel)};
}

absl::StatusOr<std::vector<std::string>> Day1_2019::Part2(const std::vector<absl::string_view>& input) {
  int total_fuel = 0;

  for (const auto& module_str : input) {
      if (module_str.empty()) continue;
      int module_cost;
      if (!absl::SimpleAtoi(module_str, &module_cost)) {
        return absl::InvalidArgumentError(absl::StrCat("Cannot parse as int: ", module_str));
      }
      int module_fuel = module_cost / 3 - 2;
      int fuel_fuel = module_fuel;
      while (fuel_fuel / 3 - 2 > 0) {
          fuel_fuel = fuel_fuel / 3 - 2;
          module_fuel += fuel_fuel;
      }
      total_fuel += module_fuel;
  }

  return std::vector<std::string>{absl::StrCat(total_fuel)};
}
