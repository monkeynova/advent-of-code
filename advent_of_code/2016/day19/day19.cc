// https://adventofcode.com/2016/day/19
//
// --- Day 19: An Elephant Named Joseph ---
// ----------------------------------------
// 
// The Elves contact you over a highly secure emergency channel. Back at
// the North Pole, the Elves are busy misunderstanding White Elephant
// parties.
// 
// Each Elf brings a present. They all sit in a circle, numbered starting
// with position 1. Then, starting with the first Elf, they take turns
// stealing all the presents from the Elf to their left. An Elf with no
// presents is removed from the circle and does not take turns.
// 
// For example, with five Elves (numbered 1 to 5):
// 
// 1
// 5   2
// 4 3     
// 
// * Elf 1 takes Elf 2's present.
// 
// * Elf 2 has no presents and is skipped.
// 
// * Elf 3 takes Elf 4's present.
// 
// * Elf 4 has no presents and is also skipped.
// 
// * Elf 5 takes Elf 1's two presents.
// 
// * Neither Elf 1 nor Elf 2 have any presents, so both are skipped.
// 
// * Elf 3 takes Elf 5's three presents.
// 
// So, with five Elves, the Elf that sits starting in position 3 gets all
// the presents.
// 
// With the number of Elves given in your puzzle input, which Elf gets
// all the presents?
//
// --- Part Two ---
// ----------------
// 
// Realizing the folly of their present-exchange rules, the Elves agree
// to instead steal presents from the Elf directly across the circle. If
// two Elves are across the circle, the one on the left (from the
// perspective of the stealer) is stolen from. The other rules remain
// unchanged: Elves with no presents are removed from the circle
// entirely, and the other elves move in slightly to keep the circle
// evenly spaced.
// 
// For example, with five Elves (again numbered 1 to 5):
// 
// * The Elves sit in a circle; Elf 1 goes first:
// 
// 1       5   2
// 4 3     
// 
// * Elves 3 and 4 are across the circle; Elf 3's present is stolen,
// being the one to the left. Elf 3 leaves the circle, and the rest
// of the Elves move in:
// 
// 1                  1
// 5   2  -->  5   2
// 4 -          4     
// 
// * Elf 2 steals from the Elf directly across the circle, Elf 5:
// 
// 1         1 
// -          2         -->     2
// 4         4      
// 
// * Next is Elf 4 who, choosing between Elves 1 and 2, steals from Elf
// 1:
// 
// -          2  
// 2  -->
// 4                 4     
// 
// * Finally, Elf 2 steals from Elf 4:
// 
// 2           -->  2  
// -     
// 
// So, with five Elves, the Elf that sits starting in position 2 gets all
// the presents.
// 
// With the number of Elves given in your puzzle input, which Elf now
// gets all the presents?


#include "advent_of_code/2016/day19/day19.h"

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

absl::StatusOr<std::string> Day_2016_19::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  std::vector<int> cycle(size, 0);
  for (int i = 0; i < cycle.size(); ++i) cycle[i] = i + 1;
  int offset = 0;
  while (cycle.size() > 1) {
    VLOG_IF(2, cycle.size() < 100) << absl::StrJoin(cycle, ",");
    std::vector<int> next_cycle;
    next_cycle.reserve(cycle.size() / 2 + 1);
    for (int i = offset; i < cycle.size(); i += 2) {
      next_cycle.push_back(cycle[i]);
    }
    offset = (offset + cycle.size()) % 2;
    cycle = next_cycle;
  }
  if (cycle.size() != 1) return Error("WTF?");
  return IntReturn(cycle[0]);
}

absl::StatusOr<std::string> Day_2016_19::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int size;
  if (!absl::SimpleAtoi(input[0], &size)) return Error("Bad int");
  std::vector<int> cycle(size, 0);
  for (int i = 0; i < cycle.size(); ++i) cycle[i] = i + 1;
  int offset = cycle.size() / 2;
  int pos = cycle.size() % 2;
  while (cycle.size() > 1) {
    VLOG_IF(2, cycle.size() < 100) << absl::StrJoin(cycle, ",");
    std::vector<int> next_cycle;
    for (int i = 0; i < offset; ++i) {
      next_cycle.push_back(cycle[i]);
    }
    for (int i = offset; i < cycle.size(); ++i) {
      if (pos == 2) next_cycle.push_back(cycle[i]);
      pos = (pos + 1) % 3;
    }
    offset = 0;
    if (next_cycle.empty()) {
      cycle = {cycle.back()};
    } else {
      cycle = next_cycle;
    }
  }
  if (cycle.size() != 1) return Error("WTF?");
  return IntReturn(cycle[0]);
}

}  // namespace advent_of_code
