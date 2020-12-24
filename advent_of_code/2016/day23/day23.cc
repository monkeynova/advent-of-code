#include "advent_of_code/2016/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2016/assem_bunny.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::vector<std::string>> Day23_2016::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->registers().a = 7;
  if (absl::Status st = vm->Execute(); !st.ok()) return st;
  return IntReturn(vm->registers().a);
}

absl::StatusOr<std::vector<std::string>> Day23_2016::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<AssemBunny> vm = AssemBunny::Parse(input);
  if (!vm.ok()) return vm.status();
  vm->registers().a = 12;
  if (absl::Status st = vm->Execute(); !st.ok()) return st;
  return IntReturn(vm->registers().a);
}

}  // namespace advent_of_code
