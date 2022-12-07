// https://adventofcode.com/2015/day/16
//
// --- Day 16: Aunt Sue ---
// ------------------------
// 
// Your Aunt Sue has given you a wonderful gift, and you'd like to send
// her a thank you card. However, there's a small problem: she signed it
// "From, Aunt Sue".
// 
// You have 500 Aunts named "Sue".
// 
// So, to avoid sending the card to the wrong person, you need to figure
// out which Aunt Sue (which you conveniently number 1 to 500, for
// sanity) gave you the gift. You open the present and, as luck would
// have it, good ol' Aunt Sue got you a My First Crime Scene Analysis
// Machine! Just what you wanted. Or needed, as the case may be.
// 
// The My First Crime Scene Analysis Machine (MFCSAM for short) can
// detect a few specific compounds in a given sample, as well as how many
// distinct kinds of those compounds there are. According to the
// instructions, these are what the MFCSAM can detect:
// 
// * children, by human DNA age analysis.
// 
// * cats. It doesn't differentiate individual breeds.
// 
// * Several seemingly random breeds of dog: samoyeds, pomeranians,
// akitas, and vizslas.
// 
// * goldfish. No other kinds of fish.
// 
// * trees, all in one group.
// 
// * cars, presumably by exhaust or gasoline or something.
// 
// * perfumes, which is handy, since many of your Aunts Sue wear a few
// kinds.
// 
// In fact, many of your Aunts Sue have many of these. You put the
// wrapping from the gift into the MFCSAM. It beeps inquisitively at you
// a few times and then prints out a message on ticker tape:
// 
// children: 3
// cats: 7
// samoyeds: 2
// pomeranians: 3
// akitas: 0
// vizslas: 0
// goldfish: 5
// trees: 3
// cars: 2
// perfumes: 1
// 
// You make a list of the things you can remember about each Aunt Sue.
// Things missing from your list aren't zero - you simply don't remember
// the value.
// 
// What is the number of the Sue that got you the gift?
//
// --- Part Two ---
// ----------------
// 
// As you're about to send the thank you note, something in the MFCSAM's
// instructions catches your eye. Apparently, it has an outdated
// retroencabulator, and so the output from the machine isn't exact
// values - some of them indicate ranges.
// 
// In particular, the cats and trees readings indicates that there are
// greater than that many (due to the unpredictable nuclear decay of cat
// dander and tree pollen), while the pomeranians and goldfish readings
// indicate that there are fewer than that many (due to the modial
// interaction of magnetoreluctance).
// 
// What is the number of the real Aunt Sue?


#include "advent_of_code/2015/day16/day16.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_16::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> features = {
      {"children", 3}, {"cats", 7},    {"samoyeds", 2}, {"pomeranians", 3},
      {"akitas", 0},   {"vizslas", 0}, {"goldfish", 5}, {"trees", 3},
      {"cars", 2},     {"perfumes", 1}};
  absl::flat_hash_set<int> aunts;
  for (int i = 1; i <= 500; ++i) aunts.insert(i);
  for (absl::string_view str : input) {
    int id, v1, v2, v3;
    absl::string_view f1, f2, f3;
    // Sue 1: children: 1, cars: 8, vizslas: 7
    if (!RE2::FullMatch(str,
                        "Sue (\\d+): (.*): (\\d+), (.*): (\\d+), (.*): (\\d+)",
                        &id, &f1, &v1, &f2, &v2, &f3, &v3)) {
      return Error("Bad input: ", str);
    }
    for (auto pair : std::vector<std::pair<absl::string_view, int>>{
             {f1, v1}, {f2, v2}, {f3, v3}}) {
      if (features[pair.first] != pair.second) aunts.erase(id);
    }
  }
  if (aunts.size() != 1) return Error("Aunt not unique: ", aunts.size());
  return IntReturn(*aunts.begin());
}

absl::StatusOr<std::string> Day_2015_16::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<absl::string_view, int> features = {
      {"children", 3}, {"cats", 7},    {"samoyeds", 2}, {"pomeranians", 3},
      {"akitas", 0},   {"vizslas", 0}, {"goldfish", 5}, {"trees", 3},
      {"cars", 2},     {"perfumes", 1}};
  absl::flat_hash_set<int> aunts;
  for (int i = 1; i <= 500; ++i) aunts.insert(i);
  for (absl::string_view str : input) {
    int id, v1, v2, v3;
    absl::string_view f1, f2, f3;
    // Sue 1: children: 1, cars: 8, vizslas: 7
    if (!RE2::FullMatch(str,
                        "Sue (\\d+): (.*): (\\d+), (.*): (\\d+), (.*): (\\d+)",
                        &id, &f1, &v1, &f2, &v2, &f3, &v3)) {
      return Error("Bad input: ", str);
    }
    for (auto pair : std::vector<std::pair<absl::string_view, int>>{
             {f1, v1}, {f2, v2}, {f3, v3}}) {
      if (pair.first == "cats" || pair.first == "trees") {
        if (features[pair.first] >= pair.second) aunts.erase(id);
      } else if (pair.first == "pomeranians" || pair.first == "goldfish") {
        if (features[pair.first] <= pair.second) aunts.erase(id);
      } else {
        if (features[pair.first] != pair.second) aunts.erase(id);
      }
    }
  }
  if (aunts.size() != 1) return Error("Aunt not unique: ", aunts.size());
  return IntReturn(*aunts.begin());
}

}  // namespace advent_of_code
