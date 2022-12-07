// https://adventofcode.com/2016/day/5
//
// --- Day 5: How About a Nice Game of Chess? ---
// ----------------------------------------------
//
// You are faced with a security door designed by Easter Bunny engineers
// that seem to have acquired most of their security knowledge by
// watching hacking movies.
//
// The eight-character password for the door is generated one character
// at a time by finding the MD5 hash of some Door ID (your puzzle input)
// and an increasing integer index (starting with 0).
//
// A hash indicates the next character in the password if its hexadecimal
// representation starts with five zeroes. If it does, the sixth
// character in the hash is the next character of the password.
//
// For example, if the Door ID is abc:
//
// * The first index which produces a hash that starts with five zeroes
// is 3231929, which we find by hashing abc3231929; the sixth
// character of the hash, and thus the first character of the
// password, is 1.
//
// * 5017308 produces the next interesting hash, which starts with
// 000008f82..., so the second character of the password is 8.
//
// * The third time a hash starts with five zeroes is for abc5278568,
// discovering the character f.
//
// In this example, after continuing this search a total of eight times,
// the password is 18f47a30.
//
// Given the actual Door ID, what is the password?
//
// --- Part Two ---
// ----------------
//
// As the door slides open, you are presented with a second door that
// uses a slightly more inspired security mechanism. Clearly unimpressed
// by the last version (in what movie is the password decrypted in order?!),
// the Easter Bunny engineers have worked out a better solution.
//
// Instead of simply filling in the password from left to right, the hash
// now also indicates the position within the password to fill. You still
// look for hashes that begin with five zeroes; however, now, the sixth
// character represents the position (0-7), and the seventh character is
// the character to put in that position.
//
// A hash result of 000001f means that f is the second character in the
// password. Use only the first result for each position, and ignore
// invalid positions.
//
// For example, if the Door ID is abc:
//
// * The first interesting hash is from abc3231929, which produces
// 0000015...; so, 5 goes in position 1: _5______.
//
// * In the previous method, 5017308 produced an interesting hash;
// however, it is ignored, because it specifies an invalid position (8).
//
// * The second interesting hash is at index 5357525, which produces
// 000004e...; so, e goes in position 4: _5__e___.
//
// You almost choke on your popcorn as the final character falls into
// place, producing the password 05ace8e3.
//
// Given the actual Door ID and this new method, what is the password? Be
// extra proud of your solution if it uses a cinematic "decrypting"
// animation.

#include "advent_of_code/2016/day05/day05.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/md5.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2016_05::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::string out;
  for (int i = 0;; ++i) {
    MD5 digest;
    std::string str = absl::StrCat(input[0], i);
    absl::string_view md5_result = digest.DigestHex(str);
    VLOG(2) << "MD5(" << str << "): " << md5_result;
    if (md5_result.substr(0, 5) == "00000") {
      VLOG(1) << "Adding: " << md5_result.substr(5, 1) << " (" << i << ")";
      out.append(md5_result.substr(5, 1));
      if (out.size() == 8) break;
    }
  }

  return out;
}

absl::StatusOr<std::string> Day_2016_05::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::string out;
  out.resize(8, '_');
  int added = 0;
  for (int i = 0;; ++i) {
    MD5 digest;
    std::string str = absl::StrCat(input[0], i);
    absl::string_view md5_result = digest.DigestHex(str);
    VLOG(2) << "MD5(" << str << "): " << md5_result;
    if (md5_result.substr(0, 5) == "00000") {
      int pos = md5_result[5] - '0';
      VLOG(1) << "Adding: " << pos << ", " << md5_result.substr(6, 1) << " ("
              << i << ")";
      if (pos < 8 && out[pos] == '_') {
        out[pos] = md5_result[6];
        ++added;
        if (added == 8) break;
      }
    }
  }

  return out;
}

}  // namespace advent_of_code
