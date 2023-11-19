#include "advent_of_code/2021/day16/day16.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2021/day16/bits_expr.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2021_16::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  ASSIGN_OR_RETURN(BitsExpr expr, BitsExpr::Parse(input[0]));
  return AdventReturn(expr.SumVersions());
}

absl::StatusOr<std::string> Day_2021_16::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  ASSIGN_OR_RETURN(BitsExpr expr, BitsExpr::Parse(input[0]));
  return AdventReturn(expr.Evaluate());
}

}  // namespace advent_of_code
