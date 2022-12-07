// https://adventofcode.com/2020/day/11
//
// --- Day 11: Seating System ---
// ------------------------------
// 
// Your plane lands with plenty of time to spare. The final leg of your
// journey is a ferry that goes directly to the tropical island where you
// can finally start your vacation. As you reach the waiting area to
// board the ferry, you realize you're so early, nobody else has even
// arrived yet!
// 
// By modeling the process people use to choose (or abandon) their seat
// in the waiting area, you're pretty sure you can predict the best place
// to sit. You make a quick map of the seat layout (your puzzle input).
// 
// The seat layout fits neatly on a grid. Each position is either floor (.),
// an empty seat (L), or an occupied seat (#). For example, the initial
// seat layout might look like this:
// 
// L.LL.LL.LL
// LLLLLLL.LL
// L.L.L..L..
// LLLL.LL.LL
// L.LL.LL.LL
// L.LLLLL.LL
// ..L.L.....
// LLLLLLLLLL
// L.LLLLLL.L
// L.LLLLL.LL
// 
// Now, you just need to model the people who will be arriving shortly.
// Fortunately, people are entirely predictable and always follow a
// simple set of rules. All decisions are based on the number of occupied
// seats adjacent to a given seat (one of the eight positions immediately
// up, down, left, right, or diagonal from the seat). The following rules
// are applied to every seat simultaneously:
// 
// * If a seat is empty (L) and there are no occupied seats adjacent to
// it, the seat becomes occupied.
// 
// * If a seat is occupied (#) and four or more seats adjacent to it
// are also occupied, the seat becomes empty.
// 
// * Otherwise, the seat's state does not change.
// 
// Floor (.) never changes; seats don't move, and nobody sits on the
// floor.
// 
// After one round of these rules, every seat in the example layout
// becomes occupied:
// 
// #.##.##.##
// #######.##
// #.#.#..#..
// ####.##.##
// #.##.##.##
// #.#####.##
// ..#.#.....
// ##########
// #.######.#
// #.#####.##
// 
// After a second round, the seats with four or more occupied adjacent
// seats become empty again:
// 
// #.LL.L#.##
// #LLLLLL.L#
// L.L.L..L..
// #LLL.LL.L#
// #.LL.LL.LL
// #.LLLL#.##
// ..L.L.....
// #LLLLLLLL#
// #.LLLLLL.L
// #.#LLLL.##
// 
// This process continues for three more rounds:
// 
// #.##.L#.##
// #L###LL.L#
// L.#.#..#..
// #L##.##.L#
// #.##.LL.LL
// #.###L#.##
// ..#.#.....
// #L######L#
// #.LL###L.L
// #.#L###.##
// 
// #.#L.L#.##
// #LLL#LL.L#
// L.L.L..#..
// #LLL.##.L#
// #.LL.LL.LL
// #.LL#L#.##
// ..L.L.....
// #L#LLLL#L#
// #.LLLLLL.L
// #.#L#L#.##
// 
// #.#L.L#.##
// #LLL#LL.L#
// L.#.L..#..
// #L##.##.L#
// #.#L.LL.LL
// #.#L#L#.##
// ..L.L.....
// #L#L##L#L#
// #.LLLLLL.L
// #.#L#L#.##
// 
// At this point, something interesting happens: the chaos stabilizes and
// further applications of these rules cause no seats to change state!
// Once people stop moving around, you count 37 occupied seats.
// 
// Simulate your seating area by applying the seating rules repeatedly
// until no seats change state. How many seats end up occupied?
//
// --- Part Two ---
// ----------------
// 
// As soon as people start to arrive, you realize your mistake. People
// don't just care about adjacent seats - they care about the first seat
// they can see in each of those eight directions!
// 
// Now, instead of considering just the eight immediately adjacent seats,
// consider the first seat in each of those eight directions. For
// example, the empty seat below would see eight occupied seats:
// 
// .......#.
// ...#.....
// .#.......
// .........
// ..#L....#
// ....#....
// .........
// #........
// ...#.....
// 
// The leftmost empty seat below would only see one empty seat, but
// cannot see any of the occupied ones:
// 
// .............
// .L.L.#.#.#.#.
// .............
// 
// The empty seat below would see no occupied seats:
// 
// .##.##.
// #.#.#.#
// ##...##
// ...L...
// ##...##
// #.#.#.#
// .##.##.
// 
// Also, people seem to be more tolerant than you expected: it now takes
// five or more visible occupied seats for an occupied seat to become
// empty (rather than four or more from the previous rules). The other
// rules still apply: empty seats that see no occupied seats become
// occupied, seats matching no rule don't change, and floor never
// changes.
// 
// Given the same starting layout as above, these new rules cause the
// seating area to shift around as follows:
// 
// L.LL.LL.LL
// LLLLLLL.LL
// L.L.L..L..
// LLLL.LL.LL
// L.LL.LL.LL
// L.LLLLL.LL
// ..L.L.....
// LLLLLLLLLL
// L.LLLLLL.L
// L.LLLLL.LL
// 
// #.##.##.##
// #######.##
// #.#.#..#..
// ####.##.##
// #.##.##.##
// #.#####.##
// ..#.#.....
// ##########
// #.######.#
// #.#####.##
// 
// #.LL.LL.L#
// #LLLLLL.LL
// L.L.L..L..
// LLLL.LL.LL
// L.LL.LL.LL
// L.LLLLL.LL
// ..L.L.....
// LLLLLLLLL#
// #.LLLLLL.L
// #.LLLLL.L#
// 
// #.L#.##.L#
// #L#####.LL
// L.#.#..#..
// ##L#.##.##
// #.##.#L.##
// #.#####.#L
// ..#.#.....
// LLL####LL#
// #.L#####.L
// #.L####.L#
// 
// #.L#.L#.L#
// #LLLLLL.LL
// L.L.L..#..
// ##LL.LL.L#
// L.LL.LL.L#
// #.LLLLL.LL
// ..L.L.....
// LLLLLLLLL#
// #.LLLLL#.L
// #.L#LL#.L#
// 
// #.L#.L#.L#
// #LLLLLL.LL
// L.L.L..#..
// ##L#.#L.L#
// L.L#.#L.L#
// #.L####.LL
// ..#.#.....
// LLL###LLL#
// #.LLLLL#.L
// #.L#LL#.L#
// 
// #.L#.L#.L#
// #LLLLLL.LL
// L.L.L..#..
// ##L#.#L.L#
// L.L#.LL.L#
// #.LLLL#.LL
// ..#.L.....
// LLL###LLL#
// #.LLLLL#.L
// #.L#LL#.L#
// 
// Again, at this point, people stop shifting around and the seating area
// reaches equilibrium. Once this occurs, you count 26 occupied seats.
// 
// Given the new visibility method and the rule change for occupied seats
// becoming empty, once equilibrium is reached, how many seats end up
// occupied?


#include "advent_of_code/2020/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

CharBoard Update(CharBoard in) {
  CharBoard out(in);
  for (Point c : in.range()) {
    int neighbors = 0;
    for (Point dir : Cardinal::kEightDirs) {
      Point n = c + dir;
      if (!in.OnBoard(n)) continue;
      if (in[n] == '#') ++neighbors;
    }
    if (in[c] == 'L') {
      if (neighbors == 0) out[c] = '#';
    } else if (in[c] == '#') {
      if (neighbors >= 4) out[c] = 'L';
    }
  }
  return out;
}

using VisMap = absl::flat_hash_map<std::pair<Point, Point>, Point>;

VisMap ComputeVismap(CharBoard in) {
  VisMap vis_map;
  for (Point c : in.range()) {
    if (in[c] == '.') continue;
    for (Point dir : Cardinal::kEightDirs) {
      Point n = c + dir;
      while (in.OnBoard(n)) {
        if (in[n] != '.') {
          vis_map.emplace(std::make_pair(c, dir), n);
          break;
        }
        n += dir;
      }
    }
  }
  return vis_map;
}

CharBoard Update2(CharBoard in, const VisMap& vis_map) {
  CharBoard out = in;
  for (Point c : in.range()) {
    int neighbors = 0;
    if (in[c] != '.') {
      for (Point dir : Cardinal::kEightDirs) {
        auto it = vis_map.find(std::make_pair(c, dir));
        if (it != vis_map.end()) {
          Point d = it->second;
          if (in[d] == '#') ++neighbors;
        }
      }
    }
    if (in[c] == 'L') {
      if (neighbors == 0) out[c] = '#';
    } else if (in[c] == '#') {
      if (neighbors >= 5) out[c] = 'L';
    }
  }
  return out;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_11::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> parsed = ParseAsBoard(input);
  if (!parsed.ok()) return parsed.status();

  CharBoard cur = *parsed;
  for (int i = 0;; ++i) {
    VLOG(1) << "Board:\n" << cur << "\n";
    CharBoard next = Update(cur);
    if (next == cur) break;
    cur = next;
  }
  return IntReturn(cur.CountOn());
}

absl::StatusOr<std::string> Day_2020_11::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> parsed = ParseAsBoard(input);
  if (!parsed.ok()) return parsed.status();

  CharBoard cur = *parsed;
  VisMap vis_map = ComputeVismap(cur);
  for (int i = 0;; ++i) {
    VLOG(1) << "Board:\n" << cur << "\n";
    CharBoard next = Update2(cur, vis_map);
    if (next == cur) break;
    cur = next;
  }
  return IntReturn(cur.CountOn());
}

}  // namespace advent_of_code
