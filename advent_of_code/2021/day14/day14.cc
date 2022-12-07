// https://adventofcode.com/2021/day/14
//
// --- Day 14: Extended Polymerization ---
// ---------------------------------------
// 
// The incredible pressures at this depth are starting to put a strain on
// your submarine. The submarine has polymerization equipment that would
// produce suitable materials to reinforce the submarine, and the nearby
// volcanically-active caves should even have the necessary input
// elements in sufficient quantities.
// 
// The submarine manual contains instructions for finding the optimal
// polymer formula; specifically, it offers a polymer template and a list
// of pair insertion rules (your puzzle input). You just need to work out
// what polymer would result after repeating the pair insertion process a
// few times.
// 
// For example:
// 
// NNCB
// 
// CH -> B
// HH -> N
// CB -> H
// NH -> C
// HB -> C
// HC -> B
// HN -> C
// NN -> C
// BH -> H
// NC -> B
// NB -> B
// BN -> B
// BB -> N
// BC -> B
// CC -> N
// CN -> C
// 
// The first line is the polymer template - this is the starting point of
// the process.
// 
// The following section defines the pair insertion rules. A rule like AB
// -> C means that when elements A and B are immediately adjacent,
// element C should be inserted between them. These insertions all happen
// simultaneously.
// 
// So, starting with the polymer template NNCB, the first step
// simultaneously considers all three pairs:
// 
// * The first pair (NN) matches the rule NN -> C, so element C is
// inserted between the first N and the second N.
// 
// * The second pair (NC) matches the rule NC -> B, so element B is
// inserted between the N and the C.
// 
// * The third pair (CB) matches the rule CB -> H, so element H is
// inserted between the C and the B.
// 
// Note that these pairs overlap: the second element of one pair is the
// first element of the next pair. Also, because all pairs are considered
// simultaneously, inserted elements are not considered to be part of a
// pair until the next step.
// 
// After the first step of this process, the polymer becomes NCNBCHB.
// 
// Here are the results of a few steps using the above rules:
// 
// Template:     NNCB
// After step 1: NCNBCHB
// After step 2: NBCCNBBBCBHCB
// After step 3: NBBBCNCCNBBNBNBBCHBHHBCHB
// After step 4: NBBNBNBBCCNBCNCCNBBNBBNBBBNBBNBBCBHCBHHNHCBBCBHCB
// 
// This polymer grows quickly. After step 5, it has length 97; After step
// 10, it has length 3073. After step 10, B occurs 1749 times, C occurs
// 298 times, H occurs 161 times, and N occurs 865 times; taking the
// quantity of the most common element (B, 1749) and subtracting the
// quantity of the least common element (H, 161) produces 1749 - 161 =
// 1588.
// 
// Apply 10 steps of pair insertion to the polymer template and find the
// most and least common elements in the result. What do you get if you
// take the quantity of the most common element and subtract the quantity
// of the least common element?
//
// --- Part Two ---
// ----------------
// 
// The resulting polymer isn't nearly strong enough to reinforce the
// submarine. You'll need to run more steps of the pair insertion
// process; a total of 40 steps should do it.
// 
// In the above example, the most common element is B (occurring
// 2192039569602 times) and the least common element is H (occurring
// 3849876073 times); subtracting these produces 2188189693529.
// 
// Apply 40 steps of pair insertion to the polymer template and find the
// most and least common elements in the result. What do you get if you
// take the quantity of the most common element and subtract the quantity
// of the least common element?


#include "advent_of_code/2021/day14/day14.h"

#include "absl/algorithm/container.h"
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

struct Problem {
  absl::string_view start;
  absl::flat_hash_map<absl::string_view, absl::string_view> rules;
};

using CharPair = std::array<char, 2>;
using CharPairCounts = absl::flat_hash_map<CharPair, int64_t>;

absl::StatusOr<Problem> Parse(absl::Span<absl::string_view> input) {
  Problem ret;
  if (input.size() < 3) return Error("Bad input");
  ret.start = input[0];
  if (input[1] != "") return Error("Bad input");
  for (int i = 2; i < input.size(); ++i) {
    auto [pair, insert] = PairSplit(input[i], " -> ");
    if (pair.size() != 2) return Error("Bad from: ", input[i]);
    if (insert.size() != 1) return Error("Bad insert: ", input[i]);
    ret.rules.emplace(pair, insert);
  }
  return ret;
}

absl::StatusOr<std::string> Expand(absl::string_view rec, const Problem& p) {
  std::string next;
  for (int j = 0; j < rec.size() - 1; ++j) {
    next.append(rec.substr(j, 1));
    auto it = p.rules.find(rec.substr(j, 2));
    if (it == p.rules.end()) return Error("Could not rule");
    next.append(it->second);
  }
  next.append(1, rec.back());
  return next;
}

absl::StatusOr<CharPairCounts> Expand(const CharPairCounts& pair_counts,
                                      const Problem& p) {
  CharPairCounts new_pair_counts;
  for (const auto& [str, count] : pair_counts) {
    auto it = p.rules.find(absl::string_view(str.data(), 2));
    if (it == p.rules.end()) return Error("Bad");
    CharPair left = {str[0], it->second[0]};
    CharPair right = {it->second[0], str[1]};
    new_pair_counts[left] += count;
    new_pair_counts[right] += count;
  }
  return new_pair_counts;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_14::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Problem> p = Parse(input);
  if (!p.ok()) return p.status();

  std::string rec = std::string(p->start);
  for (int i = 0; i < 10; ++i) {
    absl::StatusOr<std::string> next = Expand(rec, *p);
    if (!next.ok()) return next.status();
    rec = std::move(*next);
  }
  absl::flat_hash_map<absl::string_view, int64_t> counts;
  absl::string_view rec_view = rec;
  for (int i = 0; i < rec.size(); ++i) {
    ++counts[rec_view.substr(i, 1)];
  }
  int64_t max = 0;
  int64_t min = rec.size();
  for (const auto& [_, c] : counts) {
    if (c > max) max = c;
    if (c < min) min = c;
  }
  return IntReturn(max - min);
}

absl::StatusOr<std::string> Day_2021_14::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Problem> p = Parse(input);
  if (!p.ok()) return p.status();

  CharPairCounts pair_counts;
  for (int i = 0; i < p->start.size() - 1; ++i) {
    ++pair_counts[{p->start[i], p->start[i + 1]}];
  }
  for (int i = 0; i < 40; ++i) {
    absl::StatusOr<CharPairCounts> new_pair_counts = Expand(pair_counts, *p);
    pair_counts = std::move(*new_pair_counts);
  }
  absl::flat_hash_map<char, int64_t> single_char_counts = {
      // End characters missed one count in pairs.
      {p->start[0], 1},
      {p->start.back(), 1},
  };
  for (const auto& [str, count] : pair_counts) {
    single_char_counts[str[0]] += count;
    single_char_counts[str[1]] += count;
  }

  int64_t max = 0;
  int64_t min = std::numeric_limits<int64_t>::max();
  for (auto [_, count] : single_char_counts) {
    // All characters counted twice (once in each pair).
    if (count % 2 != 0) return Error("MOD 2");
    count /= 2;
    if (count > max) max = count;
    if (count < min) min = count;
  }
  return IntReturn(max - min);
}

}  // namespace advent_of_code
