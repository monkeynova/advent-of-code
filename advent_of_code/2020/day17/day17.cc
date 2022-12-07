// https://adventofcode.com/2020/day/17
//
// --- Day 17: Conway Cubes ---
// ----------------------------
// 
// As your flight slowly drifts through the sky, the Elves at the
// Mythical Information Bureau at the North Pole contact you. They'd like
// some help debugging a malfunctioning experimental energy source aboard
// one of their super-secret imaging satellites.
// 
// The experimental energy source is based on cutting-edge technology: a
// set of Conway Cubes contained in a pocket dimension! When you hear
// it's having problems, you can't help but agree to take a look.
// 
// The pocket dimension contains an infinite 3-dimensional grid. At every
// integer 3-dimensional coordinate (x,y,z), there exists a single cube
// which is either active or inactive.
// 
// In the initial state of the pocket dimension, almost all cubes start
// inactive. The only exception to this is a small flat region of cubes
// (your puzzle input); the cubes in this region start in the specified
// active (#) or inactive (.) state.
// 
// The energy source then proceeds to boot up by executing six cycles.
// 
// Each cube only ever considers its neighbors: any of the 26 other cubes
// where any of their coordinates differ by at most 1. For example, given
// the cube at x=1,y=2,z=3, its neighbors include the cube at x=2,y=2,z=2,
// the cube at x=0,y=2,z=3, and so on.
// 
// During a cycle, all cubes simultaneously change their state according
// to the following rules:
// 
// * If a cube is active and exactly 2 or 3 of its neighbors are also
// active, the cube remains active. Otherwise, the cube becomes
// inactive.
// 
// * If a cube is inactive but exactly 3 of its neighbors are active,
// the cube becomes active. Otherwise, the cube remains inactive.
// 
// The engineers responsible for this experimental energy source would
// like you to simulate the pocket dimension and determine what the
// configuration of cubes should be at the end of the six-cycle boot
// process.
// 
// For example, consider the following initial state:
// 
// .#.
// ..#
// ###
// 
// Even though the pocket dimension is 3-dimensional, this initial state
// represents a small 2-dimensional slice of it. (In particular, this
// initial state defines a 3x3x1 region of the 3-dimensional space.)
// 
// Simulating a few cycles from this initial state produces the following
// configurations, where the result of each cycle is shown layer-by-layer
// at each given z coordinate (and the frame of view follows the active
// cells in each cycle):
// 
// Before any cycles:
// 
// z=0
// .#.
// ..#
// ###
// 
// 
// After 1 cycle:
// 
// z=-1
// #..
// ..#
// .#.
// 
// z=0
// #.#
// .##
// .#.
// 
// z=1
// #..
// ..#
// .#.
// 
// 
// After 2 cycles:
// 
// z=-2
// .....
// .....
// ..#..
// .....
// .....
// 
// z=-1
// ..#..
// .#..#
// ....#
// .#...
// .....
// 
// z=0
// ##...
// ##...
// #....
// ....#
// .###.
// 
// z=1
// ..#..
// .#..#
// ....#
// .#...
// .....
// 
// z=2
// .....
// .....
// ..#..
// .....
// .....
// 
// 
// After 3 cycles:
// 
// z=-2
// .......
// .......
// ..##...
// ..###..
// .......
// .......
// .......
// 
// z=-1
// ..#....
// ...#...
// #......
// .....##
// .#...#.
// ..#.#..
// ...#...
// 
// z=0
// ...#...
// .......
// #......
// .......
// .....##
// .##.#..
// ...#...
// 
// z=1
// ..#....
// ...#...
// #......
// .....##
// .#...#.
// ..#.#..
// ...#...
// 
// z=2
// .......
// .......
// ..##...
// ..###..
// .......
// .......
// .......
// 
// After the full six-cycle boot process completes, 112 cubes are left in
// the active state.
// 
// Starting with your given initial configuration, simulate six cycles.
// How many cubes are left in the active state after the sixth cycle?
//
// --- Part Two ---
// ----------------
// 
// For some reason, your simulated results don't match what the
// experimental energy source engineers expected. Apparently, the pocket
// dimension actually has four spatial dimensions, not three.
// 
// The pocket dimension contains an infinite 4-dimensional grid. At every
// integer 4-dimensional coordinate (x,y,z,w), there exists a single cube
// (really, a hypercube) which is still either active or inactive.
// 
// Each cube only ever considers its neighbors: any of the 80 other cubes
// where any of their coordinates differ by at most 1. For example, given
// the cube at x=1,y=2,z=3,w=4, its neighbors include the cube at
// x=2,y=2,z=3,w=3, the cube at x=0,y=2,z=3,w=4, and so on.
// 
// The initial state of the pocket dimension still consists of a small
// flat region of cubes. Furthermore, the same rules for cycle updating
// still apply: during each cycle, consider the number of active
// neighbors of each cube.
// 
// For example, consider the same initial state as in the example above.
// Even though the pocket dimension is 4-dimensional, this initial state
// represents a small 2-dimensional slice of it. (In particular, this
// initial state defines a 3x3x1x1 region of the 4-dimensional space.)
// 
// Simulating a few cycles from this initial state produces the following
// configurations, where the result of each cycle is shown layer-by-layer
// at each given z and w coordinate:
// 
// Before any cycles:
// 
// z=0, w=0
// .#.
// ..#
// ###
// 
// 
// After 1 cycle:
// 
// z=-1, w=-1
// #..
// ..#
// .#.
// 
// z=0, w=-1
// #..
// ..#
// .#.
// 
// z=1, w=-1
// #..
// ..#
// .#.
// 
// z=-1, w=0
// #..
// ..#
// .#.
// 
// z=0, w=0
// #.#
// .##
// .#.
// 
// z=1, w=0
// #..
// ..#
// .#.
// 
// z=-1, w=1
// #..
// ..#
// .#.
// 
// z=0, w=1
// #..
// ..#
// .#.
// 
// z=1, w=1
// #..
// ..#
// .#.
// 
// 
// After 2 cycles:
// 
// z=-2, w=-2
// .....
// .....
// ..#..
// .....
// .....
// 
// z=-1, w=-2
// .....
// .....
// .....
// .....
// .....
// 
// z=0, w=-2
// ###..
// ##.##
// #...#
// .#..#
// .###.
// 
// z=1, w=-2
// .....
// .....
// .....
// .....
// .....
// 
// z=2, w=-2
// .....
// .....
// ..#..
// .....
// .....
// 
// z=-2, w=-1
// .....
// .....
// .....
// .....
// .....
// 
// z=-1, w=-1
// .....
// .....
// .....
// .....
// .....
// 
// z=0, w=-1
// .....
// .....
// .....
// .....
// .....
// 
// z=1, w=-1
// .....
// .....
// .....
// .....
// .....
// 
// z=2, w=-1
// .....
// .....
// .....
// .....
// .....
// 
// z=-2, w=0
// ###..
// ##.##
// #...#
// .#..#
// .###.
// 
// z=-1, w=0
// .....
// .....
// .....
// .....
// .....
// 
// z=0, w=0
// .....
// .....
// .....
// .....
// .....
// 
// z=1, w=0
// .....
// .....
// .....
// .....
// .....
// 
// z=2, w=0
// ###..
// ##.##
// #...#
// .#..#
// .###.
// 
// z=-2, w=1
// .....
// .....
// .....
// .....
// .....
// 
// z=-1, w=1
// .....
// .....
// .....
// .....
// .....
// 
// z=0, w=1
// .....
// .....
// .....
// .....
// .....
// 
// z=1, w=1
// .....
// .....
// .....
// .....
// .....
// 
// z=2, w=1
// .....
// .....
// .....
// .....
// .....
// 
// z=-2, w=2
// .....
// .....
// ..#..
// .....
// .....
// 
// z=-1, w=2
// .....
// .....
// .....
// .....
// .....
// 
// z=0, w=2
// ###..
// ##.##
// #...#
// .#..#
// .###.
// 
// z=1, w=2
// .....
// .....
// .....
// .....
// .....
// 
// z=2, w=2
// .....
// .....
// ..#..
// .....
// .....
// 
// After the full six-cycle boot process completes, 848 cubes are left in
// the active state.
// 
// Starting with your given initial configuration, simulate six cycles in
// a 4-dimensional space. How many cubes are left in the active state
// after the sixth cycle?


#include "advent_of_code/2020/day17/day17.h"

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

class Grid3 {
 public:
  Grid3() = default;

  Grid3(const CharBoard& char_board) {
    for (Point p : char_board.range()) {
      if (char_board[p] == '#') {
        grid_.insert(Point3{p.x, p.y, 0});
      }
    }
  }

  Grid3 Step() const {
    Grid3 ret;
    absl::flat_hash_map<Point3, int> neighbors;
    for (Point3 p : grid_) {
      for (Point3 dir : Cardinal3::kNeighborDirs) {
        ++neighbors[p + dir];
      }
    }
    for (const auto& [point, count] : neighbors) {
      if (count == 3)
        ret.grid_.insert(point);
      else if (grid_.contains(point) && count == 2)
        ret.grid_.insert(point);
    }

    return ret;
  }

  int CountTiles() const { return grid_.size(); }

 private:
  absl::flat_hash_set<Point3> grid_;
};

class Grid4 {
 public:
  Grid4() = default;

  Grid4(const CharBoard& char_board) {
    for (Point p : char_board.range()) {
      if (char_board[p] == '#') {
        grid_.insert(Point4{p.x, p.y, 0, 0});
      }
    }
  }

  Grid4 Step() const {
    Grid4 ret;
    absl::flat_hash_map<Point4, int> neighbors;
    for (Point4 p : grid_) {
      for (Point4 dir : Cardinal4::kNeighborDirs) {
        ++neighbors[p + dir];
      }
    }
    for (const auto& [point, count] : neighbors) {
      if (count == 3)
        ret.grid_.insert(point);
      else if (grid_.contains(point) && count == 2)
        ret.grid_.insert(point);
    }

    return ret;
  }

  int CountTiles() const { return grid_.size(); }

 private:
  absl::flat_hash_set<Point4> grid_;
};

}  // namespace

absl::StatusOr<std::string> Day_2020_17::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> initial = ParseAsBoard(input);
  if (!initial.ok()) return initial.status();
  Grid3 grid(*initial);
  for (int i = 0; i < 6; ++i) {
    grid = grid.Step();
  }
  return IntReturn(grid.CountTiles());
}

absl::StatusOr<std::string> Day_2020_17::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> initial = ParseAsBoard(input);
  if (!initial.ok()) return initial.status();
  Grid4 grid(*initial);
  for (int i = 0; i < 6; ++i) {
    grid = grid.Step();
  }
  return IntReturn(grid.CountTiles());
}

}  // namespace advent_of_code
