// https://adventofcode.com/2020/day/24
//
// --- Day 24: Lobby Layout ---
// ----------------------------
// 
// Your raft makes it to the tropical island; it turns out that the small
// crab was an excellent navigator. You make your way to the resort.
// 
// As you enter the lobby, you discover a small problem: the floor is
// being renovated. You can't even reach the check-in desk until they've
// finished installing the new tile floor.
// 
// The tiles are all hexagonal; they need to be arranged in a hex grid
// with a very specific color pattern. Not in the mood to wait, you offer
// to help figure out the pattern.
// 
// The tiles are all white on one side and black on the other. They start
// with the white side facing up. The lobby is large enough to fit
// whatever pattern might need to appear there.
// 
// A member of the renovation crew gives you a list of the tiles that
// need to be flipped over (your puzzle input). Each line in the list
// identifies a single tile that needs to be flipped by giving a series
// of steps starting from a reference tile in the very center of the
// room. (Every line starts from the same reference tile.)
// 
// Because the tiles are hexagonal, every tile has six neighbors: east,
// southeast, southwest, west, northwest, and northeast. These directions
// are given in your list, respectively, as e, se, sw, w, nw, and ne. A
// tile is identified by a series of these directions with no delimiters;
// for example, esenee identifies the tile you land on if you start at
// the reference tile and then move one tile east, one tile southeast,
// one tile northeast, and one tile east.
// 
// Each time a tile is identified, it flips from white to black or from
// black to white. Tiles might be flipped more than once. For example, a
// line like esew flips a tile immediately adjacent to the reference
// tile, and a line like nwwswee flips the reference tile itself.
// 
// Here is a larger example:
// 
// sesenwnenenewseeswwswswwnenewsewsw
// neeenesenwnwwswnenewnwwsewnenwseswesw
// seswneswswsenwwnwse
// nwnwneseeswswnenewneswwnewseswneseene
// swweswneswnenwsewnwneneseenw
// eesenwseswswnenwswnwnwsewwnwsene
// sewnenenenesenwsewnenwwwse
// wenwwweseeeweswwwnwwe
// wsweesenenewnwwnwsenewsenwwsesesenwne
// neeswseenwwswnwswswnw
// nenwswwsewswnenenewsenwsenwnesesenew
// enewnwewneswsewnwswenweswnenwsenwsw
// sweneswneswneneenwnewenewwneswswnese
// swwesenesewenwneswnwwneseswwne
// enesenwswwswneneswsenwnewswseenwsese
// wnwnesenesenenwwnenwsewesewsesesew
// nenewswnwewswnenesenwnesewesw
// eneswnwswnwsenenwnwnwwseeswneewsenese
// neswnwewnwnwseenwseesewsenwsweewe
// wseweeenwnesenwwwswnew
// 
// In the above example, 10 tiles are flipped once (to black), and 5 more
// are flipped twice (to black, then back to white). After all of these
// instructions have been followed, a total of 10 tiles are black.
// 
// Go through the renovation crew's list and determine which tiles they
// need to flip. After all of the instructions have been followed, how
// many tiles are left with the black side up?
//
// --- Part Two ---
// ----------------
// 
// The tile floor in the lobby is meant to be a living art exhibit. Every
// day, the tiles are all flipped according to the following rules:
// 
// * Any black tile with zero or more than 2 black tiles immediately
// adjacent to it is flipped to white.
// 
// * Any white tile with exactly 2 black tiles immediately adjacent to
// it is flipped to black.
// 
// Here, tiles immediately adjacent means the six tiles directly touching
// the tile in question.
// 
// The rules are applied simultaneously to every tile; put another way,
// it is first determined which tiles need to be flipped, then they are
// all flipped at the same time.
// 
// In the above example, the number of black tiles that are facing up
// after the given number of days has passed is as follows:
// 
// Day 1: 15
// Day 2: 12
// Day 3: 25
// Day 4: 14
// Day 5: 23
// Day 6: 28
// Day 7: 41
// Day 8: 37
// Day 9: 49
// Day 10: 37
// 
// Day 20: 132
// Day 30: 259
// Day 40: 406
// Day 50: 566
// Day 60: 788
// Day 70: 1106
// Day 80: 1373
// Day 90: 1844
// Day 100: 2208
// 
// After executing this process a total of 100 times, there would be 2208
// black tiles facing up.
// 
// How many tiles will be black after 100 days?


#include "advent_of_code/2020/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

absl::StatusOr<absl::flat_hash_map<Point, bool>> ParseGrid(
    absl::Span<absl::string_view> input) {
  absl::flat_hash_map<Point, bool> grid;
  for (absl::string_view str : input) {
    Point p = {0, 0};
    for (int i = 0; i < str.size(); ++i) {
      if (str[i] == 'w') {
        p += 2 * Cardinal::kWest;
      } else if (str[i] == 'e') {
        p += 2 * Cardinal::kEast;
      } else if (str[i] == 'n') {
        if (i + 1 == str.size()) return Error("Bad sequence (@", i, "): ", str);
        if (str[i + 1] == 'w') {
          p += Cardinal::kNorthWest;
        } else if (str[i + 1] == 'e') {
          p += Cardinal::kNorthEast;
        } else {
          return Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else if (str[i] == 's') {
        if (i + 1 == str.size()) return Error("Bad sequence (@", i, "): ", str);
        if (str[i + 1] == 'w') {
          p += Cardinal::kSouthWest;
        } else if (str[i + 1] == 'e') {
          p += Cardinal::kSouthEast;
        } else {
          return Error("Bad sequence (@", i, "): ", str);
        }
        ++i;
      } else {
        return Error("Bad sequence (@", i, "): ", str);
      }
    }
    grid[p] = !grid[p];
  }
  return grid;
}

absl::StatusOr<absl::flat_hash_map<Point, bool>> RunStep(
    const absl::flat_hash_map<Point, bool>& grid) {
  const std::vector<Point> kDirs = {2 * Cardinal::kWest,  2 * Cardinal::kEast,
                                    Cardinal::kSouthWest, Cardinal::kSouthEast,
                                    Cardinal::kNorthWest, Cardinal::kNorthEast};

  absl::flat_hash_map<Point, int> neighbors;
  for (const auto& [p, is_black] : grid) {
    if (!is_black) continue;
    for (Point d : kDirs) {
      ++neighbors[p + d];
    }
  }

  absl::flat_hash_map<Point, bool> ret;
  for (const auto& [p, count] : neighbors) {
    bool is_black = false;
    if (auto it = grid.find(p); it != grid.end()) is_black = it->second;
    if (is_black) {
      if (count == 1 || count == 2) {
        ret[p] = true;
      }
    } else if (count == 2) {
      ret[p] = true;
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<Point, bool>> grid = ParseGrid(input);
  if (!grid.ok()) return grid.status();
  int black_count = 0;
  for (const auto& [_, is_black] : *grid) {
    if (is_black) ++black_count;
  }
  return IntReturn(black_count);
}

absl::StatusOr<std::string> Day_2020_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<Point, bool>> grid = ParseGrid(input);
  if (!grid.ok()) return grid.status();

  for (int i = 0; i < 100; ++i) {
    grid = RunStep(*grid);
    if (!grid.ok()) return grid.status();
  }

  int black_count = 0;
  for (const auto& [_, is_black] : *grid) {
    if (is_black) ++black_count;
  }
  return IntReturn(black_count);
}

}  // namespace advent_of_code
