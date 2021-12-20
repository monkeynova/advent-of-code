#include "advent_of_code/2021/day20/day20.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

absl::StatusOr<CharBoard> Enhance(const CharBoard& b, absl::string_view lookup, char fill) {
  CharBoard new_b(b.width() + 2, b.height() + 2);
  for (Point p : new_b.range()) {
    int bv = 0;
    for (Point d : {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast,
                    Cardinal::kWest, Cardinal::kOrigin, Cardinal::kEast,
                    Cardinal::kSouthWest, Cardinal::kSouth, Cardinal::kSouthEast}) {      
      Point src = p + Cardinal::kNorthWest + d;
      bv *= 2;
      char src_val = b.OnBoard(src) ? b[src] : fill;
      if (src_val == '#') ++bv;
      else if (src_val != '.') return Error("Bad board");
    }
    if (bv >= lookup.size()) return Error("Bad lookup size");
    new_b[p] = lookup[bv];
  }
  return new_b;
}

absl::StatusOr<CharBoard> EnhanceNTimes(const CharBoard& b, absl::string_view lookup, int count) {
  absl::StatusOr<CharBoard> ret = b;
  char fill = '.';
  for (int i = 0; i < count; ++i) {
    ret = Enhance(*ret, lookup, fill);
    if (!ret.ok()) return ret.status();
    if (fill == '.') fill = lookup[0];
    else {
      if (fill != '#') return Error("Bad board");
      fill = lookup[511];
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_20::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  absl::StatusOr<CharBoard> b = CharBoard::Parse(input.subspan(2));
  if (!b.ok()) return b.status();

  b = EnhanceNTimes(*b, lookup, 2);

  return IntReturn(b->CountOn());
}

absl::StatusOr<std::string> Day_2021_20::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input (too small)");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input (lookup size)");
  if (!input[1].empty()) return Error("Bad input (missing empty line)");

  absl::StatusOr<CharBoard> b = CharBoard::Parse(input.subspan(2));
  if (!b.ok()) return b.status();

  b = EnhanceNTimes(*b, lookup, 50);

  return IntReturn(b->CountOn());
}

}  // namespace advent_of_code
