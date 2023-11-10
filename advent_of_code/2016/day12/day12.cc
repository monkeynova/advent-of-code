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
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();

  if (absl::Status st = vm->Execute(); !st.ok()) return st;

  return AdventReturn(vm->registers().a);
}

absl::StatusOr<std::string> Day_2016_12::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();

  vm->registers().c = 1;
  if (absl::Status st = vm->Execute(); !st.ok()) return st;

  return AdventReturn(vm->registers().a);
}

}  // namespace advent_of_code
