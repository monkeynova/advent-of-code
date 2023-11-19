#include "advent_of_code/2016/day23/day23.h"

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

std::optional<int64_t> MatchesDecompiled(absl::Span<std::string_view> input) {
  // Keep the regex strings as a vertical list.
  // clang-format off
  std::vector<std::string_view> decompilable_res = {
    "cpy a b",
    "dec b",
    "cpy a d",
    "cpy 0 a",
    "cpy b c",
    "inc a",
    "dec c",
    "jnz c \\-2",
    "dec d",
    "jnz d \\-5",
    "dec b",
    "cpy b c",
    "cpy c d",
    "dec d",
    "inc c",
    "jnz d \\-2",
    "tgl c",
    "cpy \\-16 c",
    "jnz 1 c",
    "cpy (\\d+) c",
    "jnz (\\d+) d",
    "inc a",
    "inc d",
    "jnz d \\-2",
    "inc c",
    "jnz c \\-5",
  };
  // clang-format off

  std::string full_re = absl::StrJoin(decompilable_res, "\n");

  int64_t p1, p2;
  if (!RE2::FullMatch(absl::StrJoin(input, "\n"), full_re, &p1, &p2)) {
    LOG(WARNING) << "Could not match re=\n" << full_re;
    LOG(WARNING) << "Against input=\n" << absl::StrJoin(input, "\n");
    return std::nullopt;
  }

  int64_t a = 12;
  for (int64_t b = a - 1; b > 0; --b) {
    a *= b;
  }
  a += p1 * p2;
  return a;
}

absl::StatusOr<std::string> Day_2016_23::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(AssemBunny vm, AssemBunny::Parse(input));
  vm.registers().a = 7;
  RETURN_IF_ERROR(vm.Execute());
  return AdventReturn(vm.registers().a);
}

absl::StatusOr<std::string> Day_2016_23::Part2(
    absl::Span<std::string_view> input) const {
  if (std::optional<int64_t> ret = MatchesDecompiled(input)) {
    return AdventReturn(ret);
  }

  LOG(ERROR) << "Falling back to full execution...";

  ASSIGN_OR_RETURN(AssemBunny vm, AssemBunny::Parse(input));
  vm.registers().a = 12;
  RETURN_IF_ERROR(vm.Execute());
  return AdventReturn(vm.registers().a);
}

}  // namespace advent_of_code
