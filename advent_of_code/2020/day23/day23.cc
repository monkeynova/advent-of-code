// https://adventofcode.com/2020/day/23
//
// --- Day 23: Crab Cups ---
// -------------------------
//
// The small crab challenges you to a game! The crab is going to mix up
// some cups, and you have to predict where they'll end up.
//
// The cups will be arranged in a circle and labeled clockwise (your
// puzzle input). For example, if your labeling were 32415, there would
// be five cups in the circle; going clockwise around the circle from the
// first cup, the cups would be labeled 3, 2, 4, 1, 5, and then back to 3
// again.
//
// Before the crab starts, it will designate the first cup in your list
// as the current cup. The crab is then going to do 100 moves.
//
// Each move, the crab does the following actions:
//
// * The crab picks up the three cups that are immediately clockwise of
// the current cup. They are removed from the circle; cup spacing is
// adjusted as necessary to maintain the circle.
//
// * The crab selects a destination cup: the cup with a label equal to
// the current cup's label minus one. If this would select one of the
// cups that was just picked up, the crab will keep subtracting one
// until it finds a cup that wasn't just picked up. If at any point
// in this process the value goes below the lowest value on any cup's
// label, it wraps around to the highest value on any cup's label
// instead.
//
// * The crab places the cups it just picked up so that they are
// immediately clockwise of the destination cup. They keep the same
// order as when they were picked up.
//
// * The crab selects a new current cup: the cup which is immediately
// clockwise of the current cup.
//
// For example, suppose your cup labeling were 389125467. If the crab
// were to do merely 10 moves, the following changes would occur:
//
// -- move 1 --
// cups: (3) 8  9  1  2  5  4  6  7
// pick up: 8, 9, 1
// destination: 2
//
// -- move 2 --
// cups:  3 (2) 8  9  1  5  4  6  7
// pick up: 8, 9, 1
// destination: 7
//
// -- move 3 --
// cups:  3  2 (5) 4  6  7  8  9  1
// pick up: 4, 6, 7
// destination: 3
//
// -- move 4 --
// cups:  7  2  5 (8) 9  1  3  4  6
// pick up: 9, 1, 3
// destination: 7
//
// -- move 5 --
// cups:  3  2  5  8 (4) 6  7  9  1
// pick up: 6, 7, 9
// destination: 3
//
// -- move 6 --
// cups:  9  2  5  8  4 (1) 3  6  7
// pick up: 3, 6, 7
// destination: 9
//
// -- move 7 --
// cups:  7  2  5  8  4  1 (9) 3  6
// pick up: 3, 6, 7
// destination: 8
//
// -- move 8 --
// cups:  8  3  6  7  4  1  9 (2) 5
// pick up: 5, 8, 3
// destination: 1
//
// -- move 9 --
// cups:  7  4  1  5  8  3  9  2 (6)
// pick up: 7, 4, 1
// destination: 5
//
// -- move 10 --
// cups: (5) 7  4  1  8  3  9  2  6
// pick up: 7, 4, 1
// destination: 3
//
// -- final --
// cups:  5 (8) 3  7  4  1  9  2  6
//
// In the above example, the cups' values are the labels as they appear
// moving clockwise around the circle; the current cup is marked with ( ).
//
// After the crab is done, what order will the cups be in? Starting after
// the cup labeled 1, collect the other cups' labels clockwise into a
// single string with no extra characters; each number except 1 should
// appear exactly once. In the above example, after 10 moves, the cups
// clockwise from 1 are labeled 9, 2, 6, 5, and so on, producing 92658374.
// If the crab were to complete all 100 moves, the order after cup 1
// would be 67384529.
//
// Using your labeling, simulate 100 moves. What are the labels on the
// cups after cup 1?
//
// --- Part Two ---
// ----------------
//
// Due to what you can only assume is a mistranslation (you're not
// exactly fluent in Crab), you are quite surprised when the crab starts
// arranging many cups in a circle on your raft - one million (1000000)
// in total.
//
// Your labeling is still correct for the first few cups; after that, the
// remaining cups are just numbered in an increasing fashion starting
// from the number after the highest number in your list and proceeding
// one by one until one million is reached. (For example, if your
// labeling were 54321, the cups would be numbered 5, 4, 3, 2, 1, and
// then start counting up from 6 until one million is reached.) In this
// way, every number from one through one million is used exactly once.
//
// After discovering where you made the mistake in translating Crab
// Numbers, you realize the small crab isn't going to do merely 100
// moves; the crab is going to do ten million (10000000) moves!
//
// The crab is going to hide your stars - one each - under the two cups
// that will end up immediately clockwise of cup 1. You can have them if
// you predict what the labels on those cups will be when the crab is
// finished.
//
// In the above example (389125467), this would be 934001 and then 159792;
// multiplying these together produces 149245887792.
//
// Determine which two cups will end up immediately clockwise of cup 1.
// What do you get if you multiply their labels together?

#include "advent_of_code/2020/day23/day23.h"

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

struct Cups {
  struct LLCup {
    int val;
    int next;
  };

  int cur_idx;
  absl::flat_hash_map<int, int> cup_to_index;
  std::vector<LLCup> cups;
};

std::ostream& operator<<(std::ostream& o, const Cups& c) {
  o << "1";
  for (int idx = c.cups[c.cup_to_index.find(1)->second].next;
       c.cups[idx].val != 1; idx = c.cups[idx].next) {
    o << ",";
    if (idx == c.cur_idx) o << "(";
    o << c.cups[idx].val;
    if (idx == c.cur_idx) o << ")";
  }
  return o;
}

void RunMove2(Cups* cups) {
  int cur_cup = cups->cups[cups->cur_idx].val;
  int dest_cup = cur_cup - 1;
  if (dest_cup < 1) dest_cup = cups->cups.size();
  bool found = true;
  int plus_three_idx;
  while (found) {
    found = false;
    int idx = cups->cups[cups->cur_idx].next;
    for (int i = 0; i < 3; ++i) {
      if (cups->cups[idx].val == dest_cup) {
        --dest_cup;
        if (dest_cup < 1) dest_cup = cups->cups.size();
        found = true;
        break;
      }
      plus_three_idx = idx;
      idx = cups->cups[idx].next;
    }
  }
  VLOG(2) << "  dest_cup=" << dest_cup;
  int dest_cup_idx = cups->cup_to_index[dest_cup];
  int tmp = cups->cups[dest_cup_idx].next;
  cups->cups[dest_cup_idx].next = cups->cups[cups->cur_idx].next;
  cups->cups[cups->cur_idx].next = cups->cups[plus_three_idx].next;
  cups->cups[plus_three_idx].next = tmp;
  cups->cur_idx = cups->cups[cups->cur_idx].next;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_23::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups cups;
  cups.cur_idx = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    int cup_num = input[0][i] - '0';
    cups.cup_to_index[cup_num] = cups.cups.size();
    cups.cups.push_back({.val = cup_num, .next = i + 1});
  }
  cups.cups.back().next = 0;
  for (int i = 0; i < 100; ++i) {
    VLOG(1) << cups;
    RunMove2(&cups);
  }
  VLOG(1) << cups;
  std::string ret;
  int ret_i = 0;
  ret.resize(cups.cups.size() - 1);
  for (int idx = cups.cups[cups.cup_to_index[1]].next; cups.cups[idx].val != 1;
       idx = cups.cups[idx].next) {
    ret[ret_i] = cups.cups[idx].val + '0';
    ++ret_i;
  }
  return ret;
}

absl::StatusOr<std::string> Day_2020_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  Cups cups;
  cups.cur_idx = 0;
  for (int i = 0; i < input[0].size(); ++i) {
    int cup_num = input[0][i] - '0';
    cups.cup_to_index[cup_num] = cups.cups.size();
    cups.cups.push_back({.val = cup_num, .next = i + 1});
  }
  for (int i = input[0].size(); i < 1000000; ++i) {
    int cup_num = i + 1;
    cups.cup_to_index[cup_num] = cups.cups.size();
    cups.cups.push_back({.val = cup_num, .next = i + 1});
  }
  cups.cups.back().next = 0;
  for (int i = 0; i < 10000000; ++i) {
    VLOG_IF(1, i % 777777 == 0) << i;
    RunMove2(&cups);
  }
  int next_idx = cups.cups[cups.cup_to_index[1]].next;
  int64_t product = 1;
  product *= cups.cups[next_idx].val;
  next_idx = cups.cups[next_idx].next;
  product *= cups.cups[next_idx].val;

  return IntReturn(product);
}

}  // namespace advent_of_code
