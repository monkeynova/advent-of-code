// https://adventofcode.com/2017/day/5
//
// --- Day 5: A Maze of Twisty Trampolines, All Alike ---
// ------------------------------------------------------
// 
// An urgent interrupt arrives from the CPU: it's trapped in a maze of
// jump instructions, and it would like assistance from any programs with
// spare cycles to help find the exit.
// 
// The message includes a list of the offsets for each jump. Jumps are
// relative: -1 moves to the previous instruction, and 2 skips the next
// one. Start at the first instruction in the list. The goal is to follow
// the jumps until one leads outside the list.
// 
// In addition, these instructions are a little strange; after each jump,
// the offset of that instruction increases by 1. So, if you come across
// an offset of 3, you would move three instructions forward, but change
// it to a 4 for the next time it is encountered.
// 
// For example, consider the following list of jump offsets:
// 
// 0
// 3
// 0
// 1
// -3
// 
// Positive jumps ("forward") move downward; negative jumps move upward.
// For legibility in this example, these offset values will be written
// all on one line, with the current instruction marked in parentheses.
// The following steps would be taken before an exit is found:
// 
// * (0) 3 0 1 -3 - before we have taken any steps.
// 
// * (1) 3 0 1 -3 - jump with offset 0 (that is, don't jump at all).
// Fortunately, the instruction is then incremented to 1.
// 
// *  2 (3) 0 1 -3 - step forward because of the instruction we just
// modified. The first instruction is incremented again, now to 2.
// 
// *  2 4 0 1 (-3) - jump all the way to the end; leave a 4 behind.
// 
// *  2 (4) 0 1 -2 - go back to where we just were; increment -3 to -2.
// 
// *  2 5 0 1 -2 - jump 4 steps forward, escaping the maze.
// 
// In this example, the exit is reached in 5 steps.
// 
// How many steps does it take to reach the exit?
//
// --- Part Two ---
// ----------------
// 
// Now, the jumps are even stranger: after each jump, if the offset was
// three or more, instead decrease it by 1. Otherwise, increase it by 1
// as before.
// 
// Using this rule with the above example, the process now takes 10
// steps, and the offset values after finding the exit are left as 2 3 2
// 3 -1.
// 
// How many steps does it now take to reach the exit?


#include "advent_of_code/2017/day05/day05.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_05::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> jumps = ParseAsInts(input);
  if (!jumps.ok()) return jumps.status();
  int steps = 0;
  for (int ip = 0; ip >= 0 && ip < jumps->size(); ++steps) {
    ip += (*jumps)[ip]++;
  }
  return IntReturn(steps);
}

absl::StatusOr<std::string> Day_2017_05::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> jumps = ParseAsInts(input);
  if (!jumps.ok()) return jumps.status();
  int steps = 0;
  for (int ip = 0; ip >= 0 && ip < jumps->size(); ++steps) {
    int delta = (*jumps)[ip];
    if (delta >= 3) {
      --(*jumps)[ip];
    } else {
      ++(*jumps)[ip];
    }
    ip += delta;
  }
  return IntReturn(steps);
}

}  // namespace advent_of_code
