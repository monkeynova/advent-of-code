// https://adventofcode.com/2020/day/6
//
// --- Day 6: Custom Customs ---
// -----------------------------
// 
// As your flight approaches the regional airport where you'll switch to
// a much larger plane, customs declaration forms are distributed to the
// passengers.
// 
// The form asks a series of 26 yes-or-no questions marked a through z.
// All you need to do is identify the questions for which anyone in your
// group answers "yes". Since your group is just you, this doesn't take
// very long.
// 
// However, the person sitting next to you seems to be experiencing a
// language barrier and asks if you can help. For each of the people in
// their group, you write down the questions for which they answer "yes",
// one per line. For example:
// 
// abcx
// abcy
// abcz
// 
// In this group, there are 6 questions to which anyone answered "yes": a,
// b, c, x, y, and z. (Duplicate answers to the same question don't count
// extra; each question counts at most once.)
// 
// Another group asks for your help, then another, and eventually you've
// collected answers from every group on the plane (your puzzle input).
// Each group's answers are separated by a blank line, and within each
// group, each person's answers are on a single line. For example:
// 
// abc
// 
// a
// b
// c
// 
// ab
// ac
// 
// a
// a
// a
// a
// 
// b
// 
// This list represents answers from five groups:
// 
// * The first group contains one person who answered "yes" to 3
// questions: a, b, and c.
// 
// * The second group contains three people; combined, they answered
// "yes" to 3 questions: a, b, and c.
// 
// * The third group contains two people; combined, they answered "yes"
// to 3 questions: a, b, and c.
// 
// * The fourth group contains four people; combined, they answered
// "yes" to only 1 question, a.
// 
// * The last group contains one person who answered "yes" to only 1
// question, b.
// 
// In this example, the sum of these counts is 3 + 3 + 3 + 1 + 1 = 11.
// 
// For each group, count the number of questions to which anyone answered
// "yes". What is the sum of those counts?
//
// --- Part Two ---
// ----------------
// 
// As you finish the last group's customs declaration, you notice that
// you misread one word in the instructions:
// 
// You don't need to identify the questions to which anyone answered
// "yes"; you need to identify the questions to which everyone answered
// "yes"!
// 
// Using the same example as above:
// 
// abc
// 
// a
// b
// c
// 
// ab
// ac
// 
// a
// a
// a
// a
// 
// b
// 
// This list represents answers from five groups:
// 
// * In the first group, everyone (all 1 person) answered "yes" to 3
// questions: a, b, and c.
// 
// * In the second group, there is no question to which everyone
// answered "yes".
// 
// * In the third group, everyone answered yes to only 1 question, a.
// Since some people did not answer "yes" to b or c, they don't
// count.
// 
// * In the fourth group, everyone answered yes to only 1 question, a.
// 
// * In the fifth group, everyone (all 1 person) answered "yes" to 1
// question, b.
// 
// In this example, the sum of these counts is 3 + 0 + 1 + 1 + 1 = 6.
// 
// For each group, count the number of questions to which everyone
// answered "yes". What is the sum of those counts?


#include "advent_of_code/2020/day06/day06.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2020_06::Part1(
    absl::Span<absl::string_view> input) const {
  int group_sum = 0;
  absl::flat_hash_set<char> hist;
  for (absl::string_view str : input) {
    if (str.empty()) {
      group_sum += hist.size();
      hist.clear();
    }
    for (char c : str) {
      hist.insert(c);
    }
  }
  group_sum += hist.size();
  return IntReturn(group_sum);
}

absl::StatusOr<std::string> Day_2020_06::Part2(
    absl::Span<absl::string_view> input) const {
  int group_sum = 0;
  absl::flat_hash_map<char, int> hist;
  int group_size = 0;
  for (absl::string_view str : input) {
    if (str.empty()) {
      int all_yes = 0;
      for (const auto& pair : hist) {
        if (pair.second == group_size) {
          ++all_yes;
        }
      }
      group_sum += all_yes;
      hist.clear();
      group_size = 0;
    } else {
      for (char c : str) {
        hist[c]++;
      }
      group_size++;
    }
  }
  int all_yes = 0;
  for (const auto& pair : hist) {
    if (pair.second == group_size) {
      ++all_yes;
    }
  }
  group_sum += all_yes;
  return IntReturn(group_sum);
}

}  // namespace advent_of_code
