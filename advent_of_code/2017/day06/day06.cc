// https://adventofcode.com/2017/day/6
//
// --- Day 6: Memory Reallocation ---
// ----------------------------------
//
// A debugger program here is having an issue: it is trying to repair a
// memory reallocation routine, but it keeps getting stuck in an infinite
// loop.
//
// In this area, there are sixteen memory banks; each memory bank can
// hold any number of blocks. The goal of the reallocation routine is to
// balance the blocks between the memory banks.
//
// The reallocation routine operates in cycles. In each cycle, it finds
// the memory bank with the most blocks (ties won by the lowest-numbered
// memory bank) and redistributes those blocks among the banks. To do
// this, it removes all of the blocks from the selected bank, then moves
// to the next (by index) memory bank and inserts one of the blocks. It
// continues doing this until it runs out of blocks; if it reaches the
// last memory bank, it wraps around to the first one.
//
// The debugger would like to know how many redistributions can be done
// before a blocks-in-banks configuration is produced that has been seen
// before.
//
// For example, imagine a scenario with only four memory banks:
//
// * The banks start with 0, 2, 7, and 0 blocks. The third bank has the
// most blocks, so it is chosen for redistribution.
//
// * Starting with the next bank (the fourth bank) and then continuing
// to the first bank, the second bank, and so on, the 7 blocks are
// spread out over the memory banks. The fourth, first, and second
// banks get two blocks each, and the third bank gets one back. The
// final result looks like this: 2 4 1 2.
//
// * Next, the second bank is chosen because it contains the most
// blocks (four). Because there are four memory banks, each gets one
// block. The result is: 3 1 2 3.
//
// * Now, there is a tie between the first and fourth memory banks,
// both of which have three blocks. The first bank wins the tie, and
// its three blocks are distributed evenly over the other three
// banks, leaving it with none: 0 2 3 4.
//
// * The fourth bank is chosen, and its four blocks are distributed
// such that each of the four banks receives one: 1 3 4 1.
//
// * The third bank is chosen, and the same thing happens: 2 4 1 2.
//
// At this point, we've reached a state we've seen before: 2 4 1 2 was
// already seen. The infinite loop is detected after the fifth block
// redistribution cycle, and so the answer in this example is 5.
//
// Given the initial block counts in your puzzle input, how many
// redistribution cycles must be completed before a configuration is
// produced that has been seen before?
//
// --- Part Two ---
// ----------------
//
// Out of curiosity, the debugger would also like to know the size of the
// loop: starting from a state that has already been seen, how many block
// redistribution cycles must be performed before that same state is seen
// again?
//
// In the example above, 2 4 1 2 is seen again after four cycles, and so
// the answer in that example would be 4.
//
// How many cycles are in the infinite loop that arises from the
// configuration in your puzzle input?

#include "advent_of_code/2017/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

std::vector<int64_t> RunStep(std::vector<int64_t> in) {
  int64_t max_idx;
  int64_t max_value = std::numeric_limits<int64_t>::min();
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] > max_value) {
      max_value = in[i];
      max_idx = i;
    }
  }
  int64_t q = in[max_idx];
  in[max_idx] = 0;
  int64_t per = q / in.size();
  int64_t rem = q % in.size();
  for (int i = 0; i < in.size(); ++i) {
    in[i] += per;
  }
  for (int j = 0; j < rem; ++j) {
    ++in[(max_idx + j + 1) % in.size()];
  }
  return in;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_06::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> split = absl::StrSplit(input[0], "\t");
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(split);
  if (!list.ok()) return list.status();
  std::vector<int64_t> tmp = *list;
  absl::flat_hash_set<std::vector<int64_t>> hist;
  while (!hist.contains(tmp)) {
    VLOG(2) << absl::StrJoin(tmp, ",");
    hist.insert(tmp);
    tmp = RunStep(std::move(tmp));
  }
  return IntReturn(hist.size());
}

absl::StatusOr<std::string> Day_2017_06::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> split = absl::StrSplit(input[0], "\t");
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(split);
  if (!list.ok()) return list.status();
  std::vector<int64_t> tmp = *list;
  absl::flat_hash_map<std::vector<int64_t>, int> hist;
  for (int i = 0; !hist.contains(tmp); ++i) {
    VLOG(2) << absl::StrJoin(tmp, ",");
    hist[tmp] = i;
    tmp = RunStep(std::move(tmp));
  }
  return IntReturn(hist.size() - hist[tmp]);
}

}  // namespace advent_of_code
