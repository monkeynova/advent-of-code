// https://adventofcode.com/2015/day/4
//
// --- Day 4: The Ideal Stocking Stuffer ---
// -----------------------------------------
// 
// Santa needs help mining some AdventCoins (very similar to bitcoins) to
// use as gifts for all the economically forward-thinking little girls
// and boys.
// 
// To do this, he needs to find MD5 hashes which, in hexadecimal, start
// with at least five zeroes. The input to the MD5 hash is some secret
// key (your puzzle input, given below) followed by a number in decimal.
// To mine AdventCoins, you must find Santa the lowest positive number
// (no leading zeroes: 1, 2, 3, ...) that produces such a hash.
// 
// For example:
// 
// * If your secret key is abcdef, the answer is 609043, because the
// MD5 hash of abcdef609043 starts with five zeroes (000001dbbfa...),
// and it is the lowest such number to do so.
// 
// * If your secret key is pqrstuv, the lowest number it combines with
// to make an MD5 hash starting with five zeroes is 1048970; that is,
// the MD5 hash of pqrstuv1048970 looks like 000006136ef....
//
// --- Part Two ---
// ----------------
// 
// Now find one that starts with six zeroes.


#include "advent_of_code/2015/day04/day04.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/md5.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2015_04::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  for (int i = 0;; ++i) {
    VLOG(1) << i;
    MD5 md5;
    std::string str = absl::StrCat(input[0], i);
    if (VLOG_IS_ON(2)) {
      absl::string_view hex = md5.DigestHex(str);
      VLOG(2) << "MD5(" << str << "): " << hex;
    }
    absl::string_view md5_result = md5.Digest(str);
    if (md5_result[0] == 0 && md5_result[1] == 0 && (md5_result[2] >> 4) == 0) {
      return IntReturn(i);
    }
  }
  return absl::InvalidArgumentError("Unreachable");
}

absl::StatusOr<std::string> Day_2015_04::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  for (int i = 0;; ++i) {
    VLOG(1) << i;
    MD5 md5;
    std::string str = absl::StrCat(input[0], i);
    if (VLOG_IS_ON(2)) {
      absl::string_view hex = md5.DigestHex(str);
      VLOG(2) << "MD5(" << str << "): " << hex;
    }
    absl::string_view md5_result = md5.Digest(str);
    if (md5_result[0] == 0 && md5_result[1] == 0 && md5_result[2] == 0) {
      return IntReturn(i);
    }
  }
  return absl::InvalidArgumentError("Unreachable");
}

}  // namespace advent_of_code
