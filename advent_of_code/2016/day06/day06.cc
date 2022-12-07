// https://adventofcode.com/2016/day/6
//
// --- Day 6: Signals and Noise ---
// --------------------------------
//
// Something is jamming your communications with Santa. Fortunately, your
// signal is only partially jammed, and protocol in situations like this
// is to switch to a simple repetition code to get the message through.
//
// In this model, the same message is sent repeatedly. You've recorded
// the repeating message signal (your puzzle input), but the data seems
// quite corrupted - almost too badly to recover. Almost.
//
// All you need to do is figure out which character is most frequent for
// each position. For example, suppose you had recorded the following
// messages:
//
// eedadn
// drvtee
// eandsr
// raavrd
// atevrs
// tsrnev
// sdttsa
// rasrtv
// nssdts
// ntnada
// svetve
// tesnvt
// vntsnd
// vrdear
// dvrsen
// enarar
//
// The most common character in the first column is e; in the second, a;
// in the third, s, and so on. Combining these characters returns the
// error-corrected message, easter.
//
// Given the recording in your puzzle input, what is the error-corrected
// version of the message being sent?
//
// --- Part Two ---
// ----------------
//
// Of course, that would be the message - if you hadn't agreed to use a
// modified repetition code instead.
//
// In this modified code, the sender instead transmits what looks like
// random data, but for each character, the character they actually want
// to send is slightly less likely than the others. Even after
// signal-jamming noise, you can look at the letter distributions in each
// column and choose the least common letter to reconstruct the original
// message.
//
// In the above example, the least common character in the first column
// is a; in the second, d, and so on. Repeating this process for the
// remaining characters produces the original message, advent.
//
// Given the recording in your puzzle input and this new decoding
// methodology, what is the original message that Santa is trying to
// send?

#include "advent_of_code/2016/day06/day06.h"

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

absl::StatusOr<std::string> Day_2016_06::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::flat_hash_map<char, int>> freqs;
  if (input.empty()) return Error("Bad Input");
  freqs.resize(input[0].size());
  for (absl::string_view txt : input) {
    if (freqs.size() != txt.size()) return Error("Mismatch input");
    for (int i = 0; i < txt.size(); ++i) {
      ++freqs[i][txt[i]];
    }
  }
  std::string out;
  out.resize(freqs.size());
  for (int i = 0; i < freqs.size(); ++i) {
    char max_char = '\0';
    int max_count = -1;
    for (const auto& [c, count] : freqs[i]) {
      if (count > max_count) {
        max_count = count;
        max_char = c;
      }
    }
    out[i] = max_char;
  }
  return out;
}

absl::StatusOr<std::string> Day_2016_06::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::flat_hash_map<char, int>> freqs;
  if (input.empty()) return Error("Bad Input");
  freqs.resize(input[0].size());
  for (absl::string_view txt : input) {
    if (freqs.size() != txt.size()) return Error("Mismatch input");
    for (int i = 0; i < txt.size(); ++i) {
      ++freqs[i][txt[i]];
    }
  }
  std::string out;
  out.resize(freqs.size());
  for (int i = 0; i < freqs.size(); ++i) {
    char min_char = '\0';
    int min_count = std::numeric_limits<int>::max();
    for (const auto& [c, count] : freqs[i]) {
      if (count < min_count) {
        min_count = count;
        min_char = c;
      }
    }
    out[i] = min_char;
  }
  return out;
}

}  // namespace advent_of_code
