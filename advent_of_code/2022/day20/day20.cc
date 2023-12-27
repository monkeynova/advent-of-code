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

void Mix(RingType& list, const std::vector<RingType::const_iterator>& it_list) {
  VLOG(2) << "Mix List: " << list;
  for (RingType::const_iterator src_it : it_list) {
    RingType::const_iterator dest_it = list.Remove(src_it);
    dest_it += *src_it;
    list.MoveBefore(dest_it, src_it);
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
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> input_ints, ParseAsInts(input));

  RingType list(input_ints);
  RingType::const_iterator zero_it;
  RingType::const_iterator start = list.FirstAdded();

  std::vector<RingType::const_iterator> list_it;
  list_it.reserve(input_ints.size());

  auto it = start;
  do {
    list_it.push_back(it);
    if (*it == 0) {
      if (zero_it != RingType::const_iterator()) return Error("Duplicate 0");
      zero_it = it;
    }
    ++it;
  } while (it != start);

  if (zero_it == RingType::const_iterator()) return Error("No zero found");

  Mix(list, list_it);
  return AdventReturn(Coordinates(zero_it));
}

absl::StatusOr<std::string> Day_2022_20::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> input_ints, ParseAsInts(input));

  for (auto& i : input_ints) {
    i *= 811589153;
  }

  RingType list(input_ints);
  RingType::const_iterator zero_it;
  RingType::const_iterator start = list.FirstAdded();

  std::vector<RingType::const_iterator> list_it;
  list_it.reserve(input_ints.size());

  auto it = start;
  do {
    list_it.push_back(it);
    if (*it == 0) {
      if (zero_it != RingType::const_iterator()) return Error("Duplicate 0");
      zero_it = it;
    }
    ++it;
  } while (it != start);

  if (zero_it == RingType::const_iterator()) return Error("No zero found");

  for (int mix = 0; mix < 10; ++mix) {
    Mix(list, list_it);
  }
  return AdventReturn(Coordinates(zero_it));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/20, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_20());
});

}  // namespace advent_of_code
