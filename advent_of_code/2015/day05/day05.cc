// https://adventofcode.com/2015/day/5
//
// --- Day 5: Doesn't He Have Intern-Elves For This? ---
// -----------------------------------------------------
// 
// Santa needs help figuring out which strings in his text file are
// naughty or nice.
// 
// A nice string is one with all of the following properties:
// 
// * It contains at least three vowels (aeiou only), like aei, xazegov,
// or aeiouaeiouaeiou.
// 
// * It contains at least one letter that appears twice in a row, like
// xx, abcdde (dd), or aabbccdd (aa, bb, cc, or dd).
// 
// * It does not contain the strings ab, cd, pq, or xy, even if they
// are part of one of the other requirements.
// 
// For example:
// 
// * ugknbfddgicrmopn is nice because it has at least three vowels (u...i...o...),
// a double letter (...dd...), and none of the disallowed substrings.
// 
// * aaa is nice because it has at least three vowels and a double
// letter, even though the letters used by different rules overlap.
// 
// * jchzalrnumimnmhp is naughty because it has no double letter.
// 
// * haegwjzuvuyypxyu is naughty because it contains the string xy.
// 
// * dvszwmarrgswjxmb is naughty because it contains only one vowel.
// 
// How many strings are nice?
//
// --- Part Two ---
// ----------------
// 
// Realizing the error of his ways, Santa has switched to a better model
// of determining whether a string is naughty or nice. None of the old
// rules apply, as they are all clearly ridiculous.
// 
// Now, a nice string is one with all of the following properties:
// 
// * It contains a pair of any two letters that appears at least twice
// in the string without overlapping, like xyxy (xy) or aabcdefgaa (aa),
// but not like aaa (aa, but it overlaps).
// 
// * It contains at least one letter which repeats with exactly one
// letter between them, like xyx, abcdefeghi (efe), or even aaa.
// 
// For example:
// 
// * qjhvhtzxzqqjkmpb is nice because is has a pair that appears twice
// (qj) and a letter that repeats with exactly one letter between
// them (zxz).
// 
// * xxyxx is nice because it has a pair that appears twice and a
// letter that repeats with one between, even though the letters used
// by each rule overlap.
// 
// * uurcxstgmygtbstg is naughty because it has a pair (tg) but no
// repeat with a single letter between them.
// 
// * ieodomkazucvgmuy is naughty because it has a repeating letter with
// one between (odo), but no pair that appears twice.
// 
// How many strings are nice under these new rules?


#include "advent_of_code/2015/day05/day05.h"

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

enum Classification {
  kNaughty = 1,
  kNice = 2,
};

Classification Classify1(absl::string_view input) {
  int vowel_count = 0;
  const absl::flat_hash_set<char> vowels = {'a', 'e', 'i', 'o', 'u'};
  bool has_double = false;
  const absl::flat_hash_set<absl::string_view> bad_pairs = {"ab", "cd", "pq",
                                                            "xy"};
  bool has_bad_pair = false;

  for (int i = 0; i < input.size(); ++i) {
    if (vowels.contains(input[i])) ++vowel_count;
    if (i > 0) {
      if (input[i - 1] == input[i]) has_double = true;
      if (bad_pairs.contains(input.substr(i - 1, 2))) has_bad_pair = true;
    }
  }

  if (vowel_count < 3) return kNaughty;
  if (!has_double) return kNaughty;
  if (has_bad_pair) return kNaughty;

  return kNice;
}

Classification Classify2(absl::string_view input) {
  bool found_repeat_around = false;
  for (int i = 2; i < input.size(); ++i) {
    if (input[i] == input[i - 2]) {
      found_repeat_around = true;
      break;
    }
  }
  if (!found_repeat_around) return kNaughty;

  bool found_dupe = false;
  for (int i = 0; i < input.size(); ++i) {
    for (int j = i + 2; j < input.size(); ++j) {
      if (input.substr(i, 2) == input.substr(j, 2)) {
        found_dupe = true;
        break;
      }
    }
  }
  if (!found_dupe) return kNaughty;

  return kNice;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_05::Part1(
    absl::Span<absl::string_view> input) const {
  int nice = 0;
  for (absl::string_view str : input) {
    if (Classify1(str) == kNice) ++nice;
  }
  return IntReturn(nice);
}

absl::StatusOr<std::string> Day_2015_05::Part2(
    absl::Span<absl::string_view> input) const {
  int nice = 0;
  for (absl::string_view str : input) {
    if (Classify2(str) == kNice) ++nice;
  }
  return IntReturn(nice);
}

}  // namespace advent_of_code
