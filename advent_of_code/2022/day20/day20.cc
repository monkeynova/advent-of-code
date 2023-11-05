// http://adventofcode.com/2022/day/20

#include "advent_of_code/2022/day20/day20.h"

#include "absl/algorithm/container.h"
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

using RingType = SpliceRing<int64_t, SpliceRingIndexType::kNone>;

void Mix(
    RingType& list,
    const std::vector<RingType::const_iterator>& it_list) {
  VLOG(2) << "Mix List: " << list;
  for (RingType::const_iterator it : it_list) {
    if (*it == 0) {
      // Do nothing.
    } else if (*it > 0) {
      int64_t incs = *it % (list.size() - 1);
      // Splice inserts before the iterator, so when going forward a no-op is
      // it+1. Hence we do one more loop on increment than decrement.
      RingType::const_iterator move_it = it + incs + 1;
      list.MoveBefore(move_it, it);
    } else {
      int64_t decs = (-*it) % (list.size() - 1);
      RingType::const_iterator move_it = it - decs;
      list.MoveBefore(move_it, it);
    }
    VLOG(3) << "Step List: " << list;
  }
}

int64_t Coordinates(RingType::const_iterator it) {
  int64_t sum = 0;
  for (int i = 0; i < 3; ++i) {
    it += 1000;
    sum += *it;
  }
  return sum;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> input_ints = ParseAsInts(input);
  if (!input_ints.ok()) return input_ints.status();

  RingType list;
  std::vector<RingType::const_iterator> list_it;
  for (int64_t v : *input_ints) {
    list.InsertSomewhere(v);
  }
  RingType::const_iterator start = list.SomePoint();
  RingType::const_iterator it = start;
  RingType::const_iterator zero_it;
  list_it.push_back(it);
  if (*it == 0) {
    zero_it = list_it.back();
  }
  for (++it; it != start; ++it) {
    list_it.push_back(it);
    if (*it == 0) {
      if (zero_it != RingType::const_iterator()) return Error("Duplicate 0");
      zero_it = list_it.back();
    }
  }
  if (zero_it == RingType::const_iterator()) return Error("No zero found");

  Mix(list, list_it);
  return AdventReturn(Coordinates(zero_it));
}

absl::StatusOr<std::string> Day_2022_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> input_ints = ParseAsInts(input);
  if (!input_ints.ok()) return input_ints.status();

  RingType list;
  std::vector<RingType::const_iterator> list_it;
  for (int64_t v : *input_ints) {
    list.InsertSomewhere(811589153 * v);
  }
  RingType::const_iterator start = list.SomePoint();
  RingType::const_iterator it = start;
  RingType::const_iterator zero_it;
  list_it.push_back(it);
  if (*it == 0) {
    zero_it = list_it.back();
  }
  for (++it; it != start; ++it) {
    list_it.push_back(it);
    if (*it == 0) {
      if (zero_it != RingType::const_iterator()) return Error("Duplicate 0");
      zero_it = list_it.back();
    }
  }
  if (zero_it == RingType::const_iterator()) return Error("No zero found");

  for (int mix = 0; mix < 10; ++mix) {
    Mix(list, list_it);
  }
  return AdventReturn(Coordinates(zero_it));
}

}  // namespace advent_of_code
