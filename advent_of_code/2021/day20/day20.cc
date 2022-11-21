// http://adventofcode.com/2021/day/20

#include "advent_of_code/2021/day20/day20.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/conway.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2021_20::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  absl::StatusOr<CharBoard> b = ParseAsBoard(input.subspan(2));
  if (!b.ok()) return b.status();

  Conway conway(*b, std::string(lookup));
  conway.set_infinite();
  if (auto st = conway.AdvanceN(2); !st.ok()) return st;
  if (conway.fill() != '.') {
    return absl::UnimplementedError("Can't return a board with non-'.' fill");
  }

  return IntReturn(conway.board().CountOn());
}

absl::StatusOr<std::string> Day_2021_20::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  absl::StatusOr<CharBoard> b = ParseAsBoard(input.subspan(2));
  if (!b.ok()) return b.status();

  Conway conway(*b, std::string(lookup));
  conway.set_infinite();
  if (auto st = conway.AdvanceN(50); !st.ok()) return st;
  if (conway.fill() != '.') {
    return absl::UnimplementedError("Can't return a board with non-'.' fill");
  }

  return IntReturn(conway.board().CountOn());
}

}  // namespace advent_of_code
