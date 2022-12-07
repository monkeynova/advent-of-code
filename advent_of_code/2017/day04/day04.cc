// https://adventofcode.com/2017/day/4
//
// --- Day 4: High-Entropy Passphrases ---
// ---------------------------------------
//
// A new system policy has been put in place that requires all accounts
// to use a passphrase instead of simply a password. A passphrase
// consists of a series of words (lowercase letters) separated by spaces.
//
// To ensure security, a valid passphrase must contain no duplicate
// words.
//
// For example:
//
// * aa bb cc dd ee is valid.
//
// * aa bb cc dd aa is not valid - the word aa appears more than once.
//
// * aa bb cc dd aaa is valid - aa and aaa count as different words.
//
// The system's full passphrase list is available as your puzzle input.
// How many passphrases are valid?
//
// --- Part Two ---
// ----------------
//
// For added security, yet another system policy has been put in place.
// Now, a valid passphrase must contain no two words that are anagrams of
// each other - that is, a passphrase is invalid if any word's letters
// can be rearranged to form any other word in the passphrase.
//
// For example:
//
// * abcde fghij is a valid passphrase.
//
// * abcde xyz ecdab is not valid - the letters from the third word can
// be rearranged to form the first word.
//
// * a ab abc abd abf abj is a valid passphrase, because all letters
// need to be used when forming another word.
//
// * iiii oiii ooii oooi oooo is valid.
//
// * oiii ioii iioi iiio is not valid - any of these words can be
// rearranged to form any other word.
//
// Under this new system policy, how many passphrases are valid?

#include "advent_of_code/2017/day04/day04.h"

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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_04::Part1(
    absl::Span<absl::string_view> input) const {
  int valid = 0;
  for (absl::string_view in : input) {
    std::vector<absl::string_view> list = absl::StrSplit(in, " ");
    absl::flat_hash_set<absl::string_view> uniq(list.begin(), list.end());
    if (list.size() == uniq.size()) ++valid;
  }
  return IntReturn(valid);
}

absl::StatusOr<std::string> Day_2017_04::Part2(
    absl::Span<absl::string_view> input) const {
  int valid = 0;
  for (absl::string_view in : input) {
    std::vector<absl::string_view> list = absl::StrSplit(in, " ");
    absl::flat_hash_set<std::string> uniq;
    for (absl::string_view w : list) {
      std::string sorted_word = std::string(w);
      std::sort(sorted_word.begin(), sorted_word.end());
      uniq.insert(sorted_word);
    }
    if (list.size() == uniq.size()) ++valid;
  }
  return IntReturn(valid);
}

}  // namespace advent_of_code
