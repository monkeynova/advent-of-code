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
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");

  SpliceRing<int> ring(1, size);
  SpliceRing<int>::const_iterator cur = ring.FirstAdded();
  while (ring.size() > 1) {
    cur = ring.Remove(cur + 1);
  }
  return AdventReturn(*cur);
}

absl::StatusOr<std::string> Day_2016_19::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  SpliceRing<int> ring(1, size);
  SpliceRing<int>::const_iterator cur = ring.FirstAdded();
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

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_19()); });

}  // namespace advent_of_code
