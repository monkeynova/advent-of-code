// https://adventofcode.com/2018/day/5
//
// --- Day 5: Alchemical Reduction ---
// -----------------------------------
//
// You've managed to sneak in to the prototype suit manufacturing lab.
// The Elves are making decent progress, but are still struggling with
// the suit's size reduction capabilities.
//
// While the very latest in 1518 alchemical technology might have solved
// their problem eventually, you can do better. You scan the chemical
// composition of the suit's material and discover that it is formed by
// extremely long polymers (one of which is available as your puzzle
// input).
//
// The polymer is formed by smaller units which, when triggered, react
// with each other such that two adjacent units of the same type and
// opposite polarity are destroyed. Units' types are represented by
// letters; units' polarity is represented by capitalization. For
// instance, r and R are units with the same type but opposite polarity,
// whereas r and s are entirely different types and do not react.
//
// For example:
//
// * In aA, a and A react, leaving nothing behind.
//
// * In abBA, bB destroys itself, leaving aA. As above, this then
// destroys itself, leaving nothing.
//
// * In abAB, no two adjacent units are of the same type, and so
// nothing happens.
//
// * In aabAAB, even though aa and AA are of the same type, their
// polarities match, and so nothing happens.
//
// Now, consider a larger example, dabAcCaCBAcCcaDA:
//
// dabA   cC   aCBAcCcaDA  The first 'cC' is removed.
// dab   Aa   CBAcCcaDA    This creates 'Aa', which is removed.
// dabCBA   cCc   aDA      Either 'cC' or 'Cc' are removed (the result is the
// same). dabCBAcaDA        No further actions can be taken.
//
// After all possible reactions, the resulting polymer contains 10 units.
//
// How many units remain after fully reacting the polymer you scanned?
// (Note: in this puzzle and others, the input is large; if you
// copy/paste your input, make sure you get the whole thing.)
//
// --- Part Two ---
// ----------------
//
// Time to improve the polymer.
//
// One of the unit types is causing problems; it's preventing the polymer
// from collapsing as much as it should. Your goal is to figure out which
// unit type is causing the most problems, remove all instances of it
// (regardless of polarity), fully react the remaining polymer, and
// measure its length.
//
// For example, again using the polymer dabAcCaCBAcCcaDA from above:
//
// * Removing all A/a units produces dbcCCBcCcD. Fully reacting this
// polymer produces dbCBcD, which has length 6.
//
// * Removing all B/b units produces daAcCaCAcCcaDA. Fully reacting
// this polymer produces daCAcaDA, which has length 8.
//
// * Removing all C/c units produces dabAaBAaDA. Fully reacting this
// polymer produces daDA, which has length 4.
//
// * Removing all D/d units produces abAcCaCBAcCcaA. Fully reacting
// this polymer produces abCBAc, which has length 6.
//
// In this example, removing all C/c units was best, producing the answer
// 4.
//
// What is the length of the shortest polymer you can produce by removing
// all units of exactly one type and fully reacting the result?

#include "advent_of_code/2018/day05/day05.h"

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

std::string React(std::string in) {
  std::string ret;
  ret.resize(in.size());
  int out = 0;
  for (int i = 0; i < in.size(); ++i) {
    bool remove = false;
    if (i < in.size() - 1) {
      if (in[i] == in[i + 1] + 'a' - 'A') {
        remove = true;
      }
      if (in[i] == in[i + 1] + 'A' - 'a') {
        remove = true;
      }
    }
    if (remove) {
      ++i;
    } else {
      ret[out++] = in[i];
    }
  }
  ret.resize(out);
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2018_05::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::string comp = std::string(input[0]);
  while (true) {
    VLOG(1) << comp;
    std::string new_comp = React(comp);
    if (new_comp.size() == comp.size()) return IntReturn(new_comp.size());
    comp = std::move(new_comp);
  }
  return Error("left infinite loop");
}

absl::StatusOr<std::string> Day_2018_05::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  int min_size = std::numeric_limits<int>::max();
  for (char c = 'a'; c <= 'z'; ++c) {
    std::string comp = std::string(input[0]);
    std::string removed;
    removed.resize(comp.size());
    int out = 0;
    for (int i = 0; i < comp.size(); ++i) {
      if (comp[i] == c || comp[i] == c + 'A' - 'a') {
        // Skip.
      } else {
        removed[out++] = comp[i];
      }
    }
    removed.resize(out);
    comp = std::move(removed);
    VLOG(1) << comp;
    while (true) {
      std::string new_comp = React(comp);
      if (new_comp.size() == comp.size()) break;
      comp = std::move(new_comp);
    }
    VLOG(1) << comp;
    min_size = std::min<int>(min_size, comp.size());
  }
  return IntReturn(min_size);
}

}  // namespace advent_of_code
