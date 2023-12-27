#include "advent_of_code/2017/day17/day17.h"

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

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_17::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  int n;
  if (!absl::SimpleAtoi(input[0], &n)) return Error("Bad int:", input[0]);
  SpliceRing<int> buf;
  SpliceRing<int>::const_iterator pos = buf.InsertFirst(0);
  for (int i = 0; i < 2017; ++i) {
    VLOG(2) << "Buf: " << buf;
    pos = buf.InsertAfter(pos + n, i + 1);
  }
  return AdventReturn(*(pos + 1));
}

absl::StatusOr<std::string> Day_2017_17::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  int n;
  if (!absl::SimpleAtoi(input[0], &n)) return Error("Bad int:", input[0]);
  int pos = 0;
  int last_after_zero = 0;
  // Split the loop at n so we can do the expensive full mod when the increment
  // could be larger than 2i, but do a cheaper if+subtract after that.
  for (int i = 0; i < n; ++i) {
    pos = (pos + n) % (i + 1);
    ++pos;
    if (pos == 1) last_after_zero = i + 1;
  }
  for (int i = n; i < 50'000'000; ++i) {
    pos += n;
    if (pos > i) pos -= i + 1;
    ++pos;
    if (pos == 1) last_after_zero = i + 1;
  }
  return AdventReturn(last_after_zero);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2017, /*day=*/17, []() {
  return std::unique_ptr<AdventDay>(new Day_2017_17());
});

}  // namespace advent_of_code
