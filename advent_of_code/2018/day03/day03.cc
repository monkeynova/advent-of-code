// https://adventofcode.com/2018/day/3
//
// --- Day 3: No Matter How You Slice It ---
// -----------------------------------------
// 
// The Elves managed to locate the chimney-squeeze prototype fabric for
// Santa's suit (thanks to someone who helpfully wrote its box IDs on the
// wall of the warehouse in the middle of the night). Unfortunately,
// anomalies are still affecting them - nobody can even agree on how to
// cut the fabric.
// 
// The whole piece of fabric they're working on is a very large square -
// at least 1000 inches on each side.
// 
// Each Elf has made a claim about which area of fabric would be ideal
// for Santa's suit. All claims have an ID and consist of a single
// rectangle with edges parallel to the edges of the fabric. Each claim's
// rectangle is defined as follows:
// 
// * The number of inches between the left edge of the fabric and the
// left edge of the rectangle.
// 
// * The number of inches between the top edge of the fabric and the
// top edge of the rectangle.
// 
// * The width of the rectangle in inches.
// 
// * The height of the rectangle in inches.
// 
// A claim like #123 @ 3,2: 5x4 means that claim ID 123 specifies a
// rectangle 3 inches from the left edge, 2 inches from the top edge, 5
// inches wide, and 4 inches tall. Visually, it claims the square inches
// of fabric represented by # (and ignores the square inches of fabric
// represented by .) in the diagram below:
// 
// ...........
// ...........
// ...#####...
// ...#####...
// ...#####...
// ...#####...
// ...........
// ...........
// ...........
// 
// The problem is that many of the claims overlap, causing two or more
// claims to cover part of the same areas. For example, consider the
// following claims:
// 
// #1 @ 1,3: 4x4
// #2 @ 3,1: 4x4
// #3 @ 5,5: 2x2
// 
// Visually, these claim the following areas:
// 
// ........
// ...2222.
// ...2222.
// .11XX22.
// .11XX22.
// .111133.
// .111133.
// ........
// 
// The four square inches marked with X are claimed by both 1 and 2.
// (Claim 3, while adjacent to the others, does not overlap either of
// them.)
// 
// If the Elves all proceed with their own plans, none of them will have
// enough fabric. How many square inches of fabric are within two or more
// claims?
//
// --- Part Two ---
// ----------------
// 
// Amidst the chaos, you notice that exactly one claim doesn't overlap by
// even a single square inch of fabric with any other claim. If you can
// somehow draw attention to it, maybe the Elves will be able to make
// Santa's suit after all!
// 
// For example, in the claims above, only claim 3 is intact after all
// claims are made.
// 
// What is the ID of the only claim that doesn't overlap?


#include "advent_of_code/2018/day03/day03.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_03::Part1(
    absl::Span<absl::string_view> input) const {
  // #1372 @ 119,13: 29x16
  CharBoard board(1'000, 1'000);
  for (Point p : board.range()) board[p] = '.';
  int dupes = 0;
  for (absl::string_view row : input) {
    PointRectangle r;
    if (!RE2::FullMatch(row, "#\\d+ @ (\\d+,\\d+): (\\d+x\\d+)",
                        r.min.Capture(), r.max.Capture())) {
      return Error("Bad input: ", row);
    }
    r.max.x += r.min.x - 1;
    r.max.y += r.min.y - 1;
    if (!board.OnBoard(r.min)) return Error("Min off board");
    if (!board.OnBoard(r.max)) return Error("Max off board");
    for (Point p : r) {
      if (board[p] == '+') {
        ++dupes;
        board[p] = '#';
      } else if (board[p] == '#') {
        // Already counted.
      } else {
        board[p] = '+';
      }
    }
  }
  return IntReturn(dupes);
}

absl::StatusOr<std::string> Day_2018_03::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<PointRectangle> claims;
  for (int i = 0; i < input.size(); ++i) {
    absl::string_view row = input[i];
    PointRectangle r;
    int idx = -1;
    if (!RE2::FullMatch(row, "#(\\d+) @ (\\d+,\\d+): (\\d+x\\d+)", &idx,
                        r.min.Capture(), r.max.Capture())) {
      return Error("Bad input: ", row);
    }
    if (idx != i + 1) return Error("Bad numbering");
    r.max.x += r.min.x - 1;
    r.max.y += r.min.y - 1;
    claims.push_back(r);
  }

  int clean_id = -1;
  for (int i = 0; i < claims.size(); ++i) {
    const PointRectangle& r1 = claims[i];
    bool overlap = false;
    for (int j = 0; j < claims.size(); ++j) {
      if (i == j) continue;
      const PointRectangle& r2 = claims[j];
      PointRectangle intersect;
      intersect.min.x = std::max(r1.min.x, r2.min.x);
      intersect.min.y = std::max(r1.min.y, r2.min.y);
      intersect.max.x = std::min(r1.max.x, r2.max.x);
      intersect.max.y = std::min(r1.max.y, r2.max.y);
      if (intersect.min.x <= intersect.max.x &&
          intersect.min.y <= intersect.max.y) {
        overlap = true;
        break;
      }
    }
    if (!overlap) {
      if (clean_id != -1) return Error("Dupe clean");
      clean_id = i + 1;
    }
  }
  return IntReturn(clean_id);
}

}  // namespace advent_of_code
