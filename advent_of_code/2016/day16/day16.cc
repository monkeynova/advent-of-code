// https://adventofcode.com/2016/day/16
//
// --- Day 16: Dragon Checksum ---
// -------------------------------
// 
// You're done scanning this part of the network, but you've left traces
// of your presence. You need to overwrite some disks with random-looking
// data to cover your tracks and update the local security system with a
// new checksum for those disks.
// 
// For the data to not be suspicious, it needs to have certain
// properties; purely random data will be detected as tampering. To
// generate appropriate random data, you'll need to use a modified dragon
// curve.
// 
// Start with an appropriate initial state (your puzzle input). Then, so
// long as you don't have enough data yet to fill the disk, repeat the
// following steps:
// 
// * Call the data you have at this point "a".
// 
// * Make a copy of "a"; call this copy "b".
// 
// * Reverse the order of the characters in "b".
// 
// * In "b", replace all instances of 0 with 1 and all 1s with 0.
// 
// * The resulting data is "a", then a single 0, then "b".
// 
// For example, after a single step of this process,
// 
// * 1 becomes 100.
// 
// * 0 becomes 001.
// 
// * 11111 becomes 11111000000.
// 
// * 111100001010 becomes 1111000010100101011110000.
// 
// Repeat these steps until you have enough data to fill the desired
// disk.
// 
// Once the data has been generated, you also need to create a checksum
// of that data. Calculate the checksum only for the data that fits on
// the disk, even if you generated more data than that in the previous
// step.
// 
// The checksum for some given data is created by considering each
// non-overlapping pair of characters in the input data. If the two
// characters match (00 or 11), the next checksum character is a 1. If
// the characters do not match (01 or 10), the next checksum character is
// a 0. This should produce a new string which is exactly half as long as
// the original. If the length of the checksum is even, repeat the
// process until you end up with a checksum with an odd length.
// 
// For example, suppose we want to fill a disk of length 12, and when we
// finally generate a string of at least length 12, the first 12
// characters are 110010110100. To generate its checksum:
// 
// * Consider each pair: 11, 00, 10, 11, 01, 00.
// 
// * These are same, same, different, same, different, same, producing
// 110101.
// 
// * The resulting string has length 6, which is even, so we repeat the
// process.
// 
// * The pairs are 11 (same), 01 (different), 01 (different).
// 
// * This produces the checksum 100, which has an odd length, so we
// stop.
// 
// Therefore, the checksum for 110010110100 is 100.
// 
// Combining all of these steps together, suppose you want to fill a disk
// of length 20 using an initial state of 10000:
// 
// * Because 10000 is too short, we first use the modified dragon curve
// to make it longer.
// 
// * After one round, it becomes 10000011110 (11 characters), still too
// short.
// 
// * After two rounds, it becomes 10000011110010000111110 (23
// characters), which is enough.
// 
// * Since we only need 20, but we have 23, we get rid of all but the
// first 20 characters: 10000011110010000111.
// 
// * Next, we start calculating the checksum; after one round, we have
// 0111110101, which 10 characters long (even), so we continue.
// 
// * After two rounds, we have 01100, which is 5 characters long (odd),
// so we are done.
// 
// In this example, the correct checksum would therefore be 01100.
// 
// The first disk you have to fill has length 272. Using the initial
// state in your puzzle input, what is the correct checksum?
//
// --- Part Two ---
// ----------------
// 
// The second disk you have to fill has length 35651584. Again using the
// initial state in your puzzle input, what is the correct checksum for
// this disk?


#include "advent_of_code/2016/day16/day16.h"

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

class DragonCurve {
 public:
  static std::string Expand(absl::string_view tmp) {
    std::string copy = std::string(tmp);
    std::reverse(copy.begin(), copy.end());
    for (int i = 0; i < copy.size(); ++i) {
      copy[i] = copy[i] == '0' ? '1' : '0';
    }
    std::string ret = absl::StrCat(tmp, "0", copy);
    VLOG(2) << ret;
    return ret;
  }

  static std::string Checksum(absl::string_view tmp) {
    std::string ret;
    ret.resize((tmp.size() + 1) / 2);
    for (int i = 0; i < tmp.size(); i += 2) {
      if (tmp[i] == tmp[i + 1]) {
        ret[i / 2] = '1';
      } else {
        ret[i / 2] = '0';
      }
    }
    VLOG(2) << ret;
    return ret;
  }

  explicit DragonCurve(absl::string_view str) : str_(str) {}

  char CharAt(int p) {
    // TODO(@monkeynova) We could be more efficient by calculating the needed
    // char on demand for it's position and doing the checksum in slices
    // of size = 2 ** N.
    return '\0';
    // size
    // 2 * size + 1
    // 4 * size + 3
    // 8 * size + 7
    // ...
    // 2 ** N * size + (2 ** N - 1)
    // 2 ** (N+1) * size + (2 ** (N+1) - 1)
  }

 private:
  absl::string_view str_;
};

std::string RunPart1(absl::string_view input, int size) {
  std::string s = std::string(input);
  VLOG(1) << "Start: " << s;
  while (s.size() < size) {
    s = DragonCurve::Expand(s);
  }
  VLOG(1) << "Expanded: " << s;
  s = s.substr(0, size);
  VLOG(1) << "Truncated:" << s;
  while (s.size() % 2 == 0) {
    s = DragonCurve::Checksum(s);
  }
  VLOG(1) << "Checksum: " << s;
  return s;
}

}  // namespace

absl::StatusOr<std::string> Day_2016_16::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    if (RunPart1("10000", 20) != "01100") return Error("Bad code");
  }
  if (input.size() != 1) return Error("Bad input size");
  return RunPart1(input[0], 272);
}

absl::StatusOr<std::string> Day_2016_16::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  return RunPart1(input[0], 35651584);
}

}  // namespace advent_of_code
