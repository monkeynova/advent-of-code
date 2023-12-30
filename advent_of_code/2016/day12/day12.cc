#include "advent_of_code/2016/day12/day12.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2016/assem_bunny.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2016_12::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(AssemBunny vm, AssemBunny::Parse(input));
  RETURN_IF_ERROR(vm.Execute());
  return AdventReturn(vm.registers().a);
}

absl::StatusOr<std::string> Day_2016_12::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(AssemBunny vm, AssemBunny::Parse(input));
  vm.registers().c = 1;
  RETURN_IF_ERROR(vm.Execute());
  return AdventReturn(vm.registers().a);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/12,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_12()); });

}  // namespace advent_of_code
