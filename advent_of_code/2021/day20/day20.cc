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

absl::StatusOr<CharBoard> Enhance(const CharBoard& b, absl::string_view lookup, int i) {
  CharBoard new_b(b.width() + 2, b.height() + 2);
  for (Point p : new_b.range()) {
    int bv = 0;
    VLOG(2) << "dst: " << p;
    for (Point d : {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast,
                    Cardinal::kWest, Cardinal::kOrigin, Cardinal::kEast,
                    Cardinal::kSouthWest, Cardinal::kSouth, Cardinal::kSouthEast}) {      
      Point src = p + Cardinal::kNorthWest + d;
      VLOG(2) << "  src: " << src;
      bv *= 2;
      if (b.OnBoard(src)) {
        if (b[src] == '#') {
          VLOG(2) << " on";
          bv |= 1;
        } else {
          CHECK_EQ(b[src], '.');
        }
      } else {
        if (i % 2 == 1 && lookup[0] == '#') {
          bv |= 1;
        }
      }
    }
    VLOG(2) << "bv: " << bv;
    CHECK_LT(bv, lookup.size());
    new_b[p] = lookup[bv];
  }
  return new_b;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_20::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input");
  if (!input[1].empty()) return Error("Bad input");
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input.subspan(2));
  if (!b.ok()) return b.status();
  VLOG(1) << "board:\n" << *b;
  for (int i = 0; i < 2; ++i) {
    b = Enhance(*b, lookup, i);
    if (!b.ok()) return b.status();
  }
  int count = 0;
  for (Point p : b->range()) {
    if ((*b)[p] == '#') ++count;
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2021_20::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  absl::string_view lookup = input[0];
  if (lookup.size() != 512) return Error("Bad input");
  if (!input[1].empty()) return Error("Bad input");
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input.subspan(2));
  if (!b.ok()) return b.status();
  VLOG(1) << "board:\n" << *b;
  for (int i = 0; i < 50; ++i) {
    b = Enhance(*b, lookup, i);
    if (!b.ok()) return b.status();
  }
  int count = 0;
  for (Point p : b->range()) {
    if ((*b)[p] == '#') ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code
