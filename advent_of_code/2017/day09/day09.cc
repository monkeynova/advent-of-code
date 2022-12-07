// https://adventofcode.com/2017/day/9
//
// --- Day 9: Stream Processing ---
// --------------------------------
// 
// A large stream blocks your path. According to the locals, it's not
// safe to cross the stream at the moment because it's full of garbage.
// You look down at the stream; rather than water, you discover that it's
// a stream of characters.
// 
// You sit for a while and record part of the stream (your puzzle input).
// The characters represent groups - sequences that begin with { and end
// with }. Within a group, there are zero or more other things, separated
// by commas: either another group or garbage. Since groups can contain
// other groups, a } only closes the most-recently-opened unclosed group
// - that is, they are nestable. Your puzzle input represents a single,
// large group which itself contains many smaller ones.
// 
// Sometimes, instead of a group, you will find garbage. Garbage begins
// with < and ends with >. Between those angle brackets, almost any
// character can appear, including { and }. Within garbage, < has no
// special meaning.
// 
// In a futile attempt to clean up the garbage, some program has canceled
// some of the characters within it using !: inside garbage, any
// character that comes after ! should be ignored, including <, >, and
// even another !.
// 
// You don't see any characters that deviate from these rules. Outside
// garbage, you only find well-formed groups, and garbage always
// terminates according to the rules above.
// 
// Here are some self-contained pieces of garbage:
// 
// * <>, empty garbage.
// 
// * <random characters>, garbage containing random characters.
// 
// * <<<<>, because the extra < are ignored.
// 
// * <{!>}>, because the first > is canceled.
// 
// * <!!>, because the second ! is canceled, allowing the > to
// terminate the garbage.
// 
// * <!!!>>, because the second ! and the first > are canceled.
// 
// * <{o"i!a,<{i<a>, which ends at the first >.
// 
// Here are some examples of whole streams and the number of groups they
// contain:
// 
// * {}, 1 group.
// 
// * {{{}}}, 3 groups.
// 
// * {{},{}}, also 3 groups.
// 
// * {{{},{},{{}}}}, 6 groups.
// 
// * {<{},{},{{}}>}, 1 group (which itself contains garbage).
// 
// * {<a>,<a>,<a>,<a>}, 1 group.
// 
// * {{<a>},{<a>},{<a>},{<a>}}, 5 groups.
// 
// * {{<!>},{<!>},{<!>},{<a>}}, 2 groups (since all but the last > are
// canceled).
// 
// Your goal is to find the total score for all groups in your input.
// Each group is assigned a score which is one more than the score of the
// group that immediately contains it. (The outermost group gets a score
// of 1.)
// 
// * {}, score of 1.
// 
// * {{{}}}, score of 1 + 2 + 3 = 6.
// 
// * {{},{}}, score of 1 + 2 + 2 = 5.
// 
// * {{{},{},{{}}}}, score of 1 + 2 + 3 + 3 + 3 + 4 = 16.
// 
// * {<a>,<a>,<a>,<a>}, score of 1.
// 
// * {{<ab>},{<ab>},{<ab>},{<ab>}}, score of 1 + 2 + 2 + 2 + 2 = 9.
// 
// * {{<!!>},{<!!>},{<!!>},{<!!>}}, score of 1 + 2 + 2 + 2 + 2 = 9.
// 
// * {{<a!>},{<a!>},{<a!>},{<ab>}}, score of 1 + 2 = 3.
// 
// What is the total score for all groups in your input?
//
// --- Part Two ---
// ----------------
// 
// Now, you're ready to remove the garbage.
// 
// To prove you've removed it, you need to count all of the characters
// within the garbage. The leading and trailing < and > don't count, nor
// do any canceled characters or the ! doing the canceling.
// 
// * <>, 0 characters.
// 
// * <random characters>, 17 characters.
// 
// * <<<<>, 3 characters.
// 
// * <{!>}>, 2 characters.
// 
// * <!!>, 0 characters.
// 
// * <!!!>>, 0 characters.
// 
// * <{o"i!a,<{i<a>, 10 characters.
// 
// How many non-canceled characters are within the garbage in your puzzle
// input?


#include "advent_of_code/2017/day09/day09.h"

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

int TotalGroupScore(absl::string_view str) {
  int total_score = 0;
  int group_level = 0;
  bool in_garbage = false;
  bool in_garbage_escape = false;

  for (char c : str) {
    VLOG(2) << c << ": group_level" << group_level
            << ", in_garbage=" << in_garbage
            << ", in_garbage_escape=" << in_garbage_escape;
    if (in_garbage) {
      if (in_garbage_escape) {
        in_garbage_escape = false;
      } else {
        if (c == '>') in_garbage = false;
        if (c == '!') in_garbage_escape = true;
      }
    } else {
      if (c == '{') ++group_level;
      if (c == '}') {
        total_score += group_level;
        --group_level;
      }
      if (c == '<') in_garbage = true;
    }
  }

  return total_score;
}

int GarbageCount(absl::string_view str) {
  int garbage_count = 0;
  int group_level = 0;
  bool in_garbage = false;
  bool in_garbage_escape = false;

  for (char c : str) {
    VLOG(2) << c << ": group_level" << group_level
            << ", in_garbage=" << in_garbage
            << ", in_garbage_escape=" << in_garbage_escape;
    if (in_garbage) {
      if (in_garbage_escape) {
        in_garbage_escape = false;
      } else {
        if (c == '>')
          in_garbage = false;
        else if (c == '!')
          in_garbage_escape = true;
        else
          ++garbage_count;
      }
    } else {
      if (c == '{') ++group_level;
      if (c == '}') {
        --group_level;
      }
      if (c == '<') in_garbage = true;
    }
  }

  return garbage_count;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2017_09::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    std::vector<std::pair<absl::string_view, int>> tests = {
        {"{}", 1},
        {"{{{}}}", 6},
        {"{{}{}}", 5},
        {"{{<a!>},{<a!>},{<a!>},{<ab>}}", 3},
    };
    for (const auto& [str, expect] : tests) {
      int got = TotalGroupScore(str);
      if (got != expect) {
        return Error("Bad test: ", got, " != ", expect, ": ", str);
      }
    }
  }
  if (input.size() != 1) return Error("Bad input");
  return IntReturn(TotalGroupScore(input[0]));
}

absl::StatusOr<std::string> Day_2017_09::Part2(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    std::vector<std::pair<absl::string_view, int>> tests = {
        {"<>", 0},
        {"<random characters>", 17},
        {"<{o\"i!a,<{i<a>", 10},
        {"<!!>", 0},
    };
    for (const auto& [str, expect] : tests) {
      int got = GarbageCount(str);
      if (got != expect) {
        return Error("Bad test: ", got, " != ", expect, ": ", str);
      }
    }
  }
  if (input.size() != 1) return Error("Bad input");
  return IntReturn(GarbageCount(input[0]));
}

}  // namespace advent_of_code
