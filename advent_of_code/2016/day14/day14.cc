// https://adventofcode.com/2016/day/14
//
// --- Day 14: One-Time Pad ---
// ----------------------------
// 
// In order to communicate securely with Santa while you're on this
// mission, you've been using a one-time pad that you generate using a
// pre-agreed algorithm. Unfortunately, you've run out of keys in your
// one-time pad, and so you need to generate some more.
// 
// To generate keys, you first get a stream of random data by taking the
// MD5 of a pre-arranged salt (your puzzle input) and an increasing
// integer index (starting with 0, and represented in decimal); the
// resulting MD5 hash should be represented as a string of lowercase
// hexadecimal digits.
// 
// However, not all of these MD5 hashes are keys, and you need 64 new
// keys for your one-time pad. A hash is a key only if:
// 
// * It contains three of the same character in a row, like 777. Only
// consider the first such triplet in a hash.
// 
// * One of the next 1000 hashes in the stream contains that same
// character five times in a row, like 77777.
// 
// Considering future hashes for five-of-a-kind sequences does not cause
// those hashes to be skipped; instead, regardless of whether the current
// hash is a key, always resume testing for keys starting with the very
// next hash.
// 
// For example, if the pre-arranged salt is abc:
// 
// * The first index which produces a triple is 18, because the MD5
// hash of abc18 contains ...cc38887a5.... However, index 18 does not
// count as a key for your one-time pad, because none of the next
// thousand hashes (index 19 through index 1018) contain 88888.
// 
// * The next index which produces a triple is 39; the hash of abc39
// contains eee. It is also the first key: one of the next thousand
// hashes (the one at index 816) contains eeeee.
// 
// * None of the next six triples are keys, but the one after that, at
// index 92, is: it contains 999 and index 200 contains 99999.
// 
// * Eventually, index 22728 meets all of the criteria to generate the
// 64th key.
// 
// So, using our example salt of abc, index 22728 produces the 64th key.
// 
// Given the actual salt in your puzzle input, what index produces your
// 64th one-time pad key?
//
// --- Part Two ---
// ----------------
// 
// Of course, in order to make this process even more secure, you've also
// implemented key stretching.
// 
// Key stretching forces attackers to spend more time generating hashes.
// Unfortunately, it forces everyone else to spend more time, too.
// 
// To implement key stretching, whenever you generate a hash, before you
// use it, you first find the MD5 hash of that hash, then the MD5 hash of
// that hash, and so on, a total of 2016 additional hashings. Always use
// lowercase hexadecimal representations of hashes.
// 
// For example, to find the stretched hash for index 0 and salt abc:
// 
// * Find the MD5 hash of abc0: 577571be4de9dcce85a041ba0410f29f.
// 
// * Then, find the MD5 hash of that hash:
// eec80a0c92dc8a0777c619d9bb51e910.
// 
// * Then, find the MD5 hash of that hash:
// 16062ce768787384c81fe17a7a60c7e3.
// 
// * ...repeat many times...
// 
// * Then, find the MD5 hash of that hash:
// a107ff634856bb300138cac6568c0f24.
// 
// So, the stretched hash for index 0 in this situation is a107ff.... In
// the end, you find the original hash (one use of MD5), then find the
// hash-of-the-previous-hash 2016 times, for a total of 2017 uses of MD5.
// 
// The rest of the process remains the same, but now the keys are
// entirely different. Again for salt abc:
// 
// * The first triple (222, at index 5) has no matching 22222 in the
// next thousand hashes.
// 
// * The second triple (eee, at index 10) hash a matching eeeee at
// index 89, and so it is the first key.
// 
// * Eventually, index 22551 produces the 64th key (triple fff with
// matching fffff at index 22859.
// 
// Given the actual salt in your puzzle input and using 2016 extra MD5
// calls of key stretching, what index now produces your 64th one-time
// pad key?


#include "advent_of_code/2016/day14/day14.h"

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

absl::StatusOr<std::string> Day_2016_14::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");

  std::vector<std::string> window;
  int found = 0;
  for (int index = 0; true; ++index) {
    MD5 digest;
    std::string str = absl::StrCat(input[0], index);
    window.push_back(std::string(digest.DigestHex(str)));
    if (window.size() < 1001) continue;

    if (window.size() != index + 1) return Error("Bad append");

    int test_index = index - 1000;
    absl::flat_hash_set<char> hunt_set;
    {
      absl::string_view test = window[test_index];
      for (int i = 0; i + 2 < test.size(); ++i) {
        if (test[i] == test[i + 1] && test[i] == test[i + 2]) {
          VLOG(2) << "Found 3-ple of " << test.substr(i, 1) << " in " << test
                  << " @" << test_index << "," << i;
          hunt_set.insert(test[i]);
          break;
        }
      }
    }
    for (char hunt : hunt_set) {
      bool found_5ple = false;
      for (int j = 1; j <= 1000; ++j) {
        absl::string_view test = window[test_index + j];
        for (int i = 0; i + 4 < test.size(); ++i) {
          if (test[i] == hunt && test[i] == test[i + 1] &&
              test[i] == test[i + 2] && test[i] == test[i + 3] &&
              test[i] == test[i + 4]) {
            VLOG(2) << "Found 5-ple of " << test.substr(i, 1) << " in " << test
                    << " @" << test_index + j << "," << i;
            found_5ple = true;
          }
        }
      }
      if (found_5ple) {
        ++found;
        VLOG(1) << "Found Key #" << found << " @" << test_index;
        if (found == 64) return IntReturn(test_index);
        break;
      }
    }
  }

  return Error("Impossible");
}

absl::StatusOr<std::string> Day_2016_14::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");

  std::vector<std::string> window;
  int found = 0;
  absl::flat_hash_map<char, int> has_5ple;
  for (int i = '0'; i <= '9'; ++i) has_5ple[i] = -1;
  for (int i = 'a'; i <= 'f'; ++i) has_5ple[i] = -1;
  MD5 digest;
  for (int index = 0; true; ++index) {
    std::string str = absl::StrCat(input[0], index);
    for (int i = 0; i < 2017; ++i) {
      str = std::string(digest.DigestHex(str));
    }

    for (int i = 0; i + 4 < str.size(); ++i) {
      if (str[i] == str[i + 1] && str[i] == str[i + 2] &&
          str[i] == str[i + 3] && str[i] == str[i + 4]) {
        VLOG(2) << "Found 5-ple of " << str.substr(i, 1) << " in " << str
                << " @" << index << "," << i;
        has_5ple[str[i]] = index;
      }
    }

    window.push_back(str);
    if (window.size() < 1001) continue;

    if (window.size() != index + 1) return Error("Bad append");

    int test_index = index - 1000;
    char hunt = '\0';
    {
      absl::string_view test = window[test_index];
      for (int i = 0; i + 2 < test.size(); ++i) {
        if (test[i] == test[i + 1] && test[i] == test[i + 2]) {
          VLOG(2) << "Found 3-ple of " << test.substr(i, 1) << " in " << test
                  << " @" << test_index << "," << i;
          hunt = test[i];
          break;
        }
      }
    }
    if (has_5ple[hunt] > test_index) {
      ++found;
      VLOG(1) << "Found Key #" << found << " @" << test_index;
      if (found == 64) return IntReturn(test_index);
    }
  }

  return Error("Impossible");
}

}  // namespace advent_of_code
