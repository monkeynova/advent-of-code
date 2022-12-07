// https://adventofcode.com/2015/day/11
//
// --- Day 11: Corporate Policy ---
// --------------------------------
//
// Santa's previous password expired, and he needs help choosing a new
// one.
//
// To help him remember his new password after the old one expires, Santa
// has devised a method of coming up with a password based on the
// previous one. Corporate policy dictates that passwords must be exactly
// eight lowercase letters (for security reasons), so he finds his new
// password by incrementing his old password string repeatedly until it
// is valid.
//
// Incrementing is just like counting with numbers: xx, xy, xz, ya, yb,
// and so on. Increase the rightmost letter one step; if it was z, it
// wraps around to a, and repeat with the next letter to the left until
// one doesn't wrap around.
//
// Unfortunately for Santa, a new Security-Elf recently started, and he
// has imposed some additional password requirements:
//
// * Passwords must include one increasing straight of at least three
// letters, like abc, bcd, cde, and so on, up to xyz. They cannot
// skip letters; abd doesn't count.
//
// * Passwords may not contain the letters i, o, or l, as these letters
// can be mistaken for other characters and are therefore confusing.
//
// * Passwords must contain at least two different, non-overlapping
// pairs of letters, like aa, bb, or zz.
//
// For example:
//
// * hijklmmn meets the first requirement (because it contains the
// straight hij) but fails the second requirement requirement
// (because it contains i and l).
//
// * abbceffg meets the third requirement (because it repeats bb and ff)
// but fails the first requirement.
//
// * abbcegjk fails the third requirement, because it only has one
// double letter (bb).
//
// * The next password after abcdefgh is abcdffaa.
//
// * The next password after ghijklmn is ghjaabcc, because you
// eventually skip all the passwords that start with ghi..., since i
// is not allowed.
//
// Given Santa's current password (your puzzle input), what should his
// next password be?
//
// --- Part Two ---
// ----------------
//
// Santa's password expired again. What's the next one?

#include "advent_of_code/2015/day11/day11.h"

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

void Increment(std::string* password) {
  for (int i = password->size() - 1; i >= 0; --i) {
    if (++(*password)[i] != 'z' + 1) break;
    (*password)[i] = 'a';
  }
}

bool IsValid(absl::string_view password) {
  static absl::flat_hash_set<char> kBadChars = {'i', 'o', 'l'};
  bool has_inc_triple = false;
  bool has_two_pairs = false;
  absl::string_view last_pair;
  for (int i = 0; i < password.size(); ++i) {
    // TODO(@monkeynova): Push to Increment.
    if (kBadChars.contains(password[i])) return false;
    if (i + 1 < password.size()) {
      if (password[i] == password[i + 1]) {
        if (last_pair.empty()) {
          last_pair = password.substr(i, 2);
        } else if (password.substr(i, 2) != last_pair) {
          has_two_pairs = true;
        }
      }
    }
    if (i + 2 < password.size()) {
      if (password[i] == password[i + 1] - 1 &&
          password[i] == password[i + 2] - 2) {
        has_inc_triple = true;
      }
    }
  }
  if (!has_inc_triple) return false;
  if (!has_two_pairs) return false;

  return true;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_11::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  std::string password = std::string(input[0]);
  do {
    Increment(&password);
  } while (!IsValid(password));

  return password;
}

absl::StatusOr<std::string> Day_2015_11::Part2(
    absl::Span<absl::string_view> input) const {
  std::string password = std::string(input[0]);
  do {
    Increment(&password);
  } while (!IsValid(password));
  do {
    Increment(&password);
  } while (!IsValid(password));

  return password;
}

}  // namespace advent_of_code
