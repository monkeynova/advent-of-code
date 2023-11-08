#include "advent_of_code/2016/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/splice_ring.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2016_19::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");

  SpliceRing<int> ring;
  SpliceRing<int>::const_iterator cur = ring.InsertFirst(1);
  for (int i = 1; i < size; ++i) {
    cur = ring.InsertAfter(cur, i + 1);
  }
  ++cur;
  while (ring.size() > 1) {
    cur = ring.Remove(cur + 1);
  }
  return AdventReturn(*cur);
}

absl::StatusOr<std::string> Day_2016_19::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  SpliceRing<int> ring;
  SpliceRing<int>::const_iterator cur = ring.InsertFirst(1);
  for (int i = 1; i < size; ++i) {
    cur = ring.InsertAfter(cur, i + 1);
  }
  ++cur;
  int delta = ring.size() / 2;
  auto remove = cur + delta;
  while (ring.size() > 1) {
    remove = ring.Remove(remove);
    ++remove;
    ++cur;
    if (ring.size() % 2) {
      --delta;
      --remove;
    }
    DCHECK(delta == ring.size() / 2);
    DCHECK(cur + delta == remove);
  }
  return AdventReturn(*remove);
}

}  // namespace advent_of_code
