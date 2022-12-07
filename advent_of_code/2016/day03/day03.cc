// https://adventofcode.com/2016/day/3
//
// --- Day 3: Squares With Three Sides ---
// ---------------------------------------
// 
// Now that you can think clearly, you move deeper into the labyrinth of
// hallways and office furniture that makes up this part of Easter Bunny
// HQ. This must be a graphic design department; the walls are covered in
// specifications for triangles.
// 
// Or are they?
// 
// The design document gives the side lengths of each triangle it
// describes, but... 5 10 25? Some of these aren't triangles. You can't
// help but mark the impossible ones.
// 
// In a valid triangle, the sum of any two sides must be larger than the
// remaining side. For example, the "triangle" given above is impossible,
// because 5 + 10 is not larger than 25.
// 
// In your puzzle input, how many of the listed triangles are possible?
//
// --- Part Two ---
// ----------------
// 
// Now that you've helpfully marked up their design documents, it occurs
// to you that triangles are specified in groups of three vertically.
// Each set of three numbers in a column specifies a triangle. Rows are
// unrelated.
// 
// For example, given the following specification, numbers with the same
// hundreds digit would be part of the same triangle:
// 
// 101 301 501
// 102 302 502
// 103 303 503
// 201 401 601
// 202 402 602
// 203 403 603
// 
// In your puzzle input, and instead reading by columns, how many of the
// listed triangles are possible?


#include "advent_of_code/2016/day03/day03.h"

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

bool IsValid(int t1[3]) {
  return t1[0] + t1[1] > t1[2] && t1[0] + t1[2] > t1[1] &&
         t1[1] + t1[2] > t1[0];
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_03::Part1(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  for (absl::string_view in : input) {
    int t1[3];
    if (!RE2::FullMatch(in, "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[0], &t1[1],
                        &t1[2])) {
      return Error("Bad input: ", in);
    }
    if (IsValid(t1)) ++count;
  }
  return IntReturn(count);
}

absl::StatusOr<std::string> Day_2016_03::Part2(
    absl::Span<absl::string_view> input) const {
  int count = 0;
  if (input.size() % 3 != 0) return Error("Bad size");
  for (int i = 0; i < input.size(); i += 3) {
    int t1[3];
    int t2[3];
    int t3[3];
    if (!RE2::FullMatch(input[i], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[0],
                        &t2[0], &t3[0])) {
      return Error("Bad input: ", input[i]);
    }
    if (!RE2::FullMatch(input[i + 1], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[1],
                        &t2[1], &t3[1])) {
      return Error("Bad input: ", input[i + 1]);
    }
    if (!RE2::FullMatch(input[i + 2], "\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)", &t1[2],
                        &t2[2], &t3[2])) {
      return Error("Bad input: ", input[i + 2]);
    }
    if (IsValid(t1)) ++count;
    if (IsValid(t2)) ++count;
    if (IsValid(t3)) ++count;
  }
  return IntReturn(count);
}

}  // namespace advent_of_code
