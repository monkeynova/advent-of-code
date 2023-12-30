#include "advent_of_code/2019/day01/day01.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2019_01::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> costs, ParseAsInts(input));

  int total_fuel = 0;

  for (const auto& module_cost : costs) {
    total_fuel += module_cost / 3 - 2;
  }

  return AdventReturn(total_fuel);
}

absl::StatusOr<std::string> Day_2019_01::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> costs, ParseAsInts(input));

  int total_fuel = 0;

  for (const auto& module_cost : costs) {
    int module_fuel = module_cost / 3 - 2;
    int fuel_fuel = module_fuel;
    while (fuel_fuel / 3 - 2 > 0) {
      fuel_fuel = fuel_fuel / 3 - 2;
      module_fuel += fuel_fuel;
    }
    total_fuel += module_fuel;
  }

  return AdventReturn(total_fuel);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/1,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_01()); });

}  // namespace advent_of_code
