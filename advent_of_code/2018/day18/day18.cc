// https://adventofcode.com/2018/day/18
//
// --- Day 18: Settlers of The North Pole ---
// ------------------------------------------
//
// On the outskirts of the North Pole base construction project, many
// Elves are collecting lumber.
//
// The lumber collection area is 50 acres by 50 acres; each acre can be
// either open ground (.), trees (|), or a lumberyard (#). You take a
// scan of the area (your puzzle input).
//
// Strange magic is at work here: each minute, the landscape looks
// entirely different. In exactly one minute, an open acre can fill with
// trees, a wooded acre can be converted to a lumberyard, or a lumberyard
// can be cleared to open ground (the lumber having been sent to other
// projects).
//
// The change to each acre is based entirely on the contents of that acre
// as well as the number of open, wooded, or lumberyard acres adjacent to
// it at the start of each minute. Here, "adjacent" means any of the
// eight acres surrounding that acre. (Acres on the edges of the lumber
// collection area might have fewer than eight adjacent acres; the
// missing acres aren't counted.)
//
// In particular:
//
// * An open acre will become filled with trees if three or more
// adjacent acres contained trees. Otherwise, nothing happens.
//
// * An acre filled with trees will become a lumberyard if three or
// more adjacent acres were lumberyards. Otherwise, nothing happens.
//
// * An acre containing a lumberyard will remain a lumberyard if it was
// adjacent to at least one other lumberyard and at least one acre
// containing trees. Otherwise, it becomes open.
//
// These changes happen across all acres simultaneously, each of them
// using the state of all acres at the beginning of the minute and
// changing to their new form by the end of that same minute. Changes
// that happen during the minute don't affect each other.
//
// For example, suppose the lumber collection area is instead only 10 by
// 10 acres with this initial configuration:
//
// Initial state:
// .#.#...|#.
// .....#|##|
// .|..|...#.
// ..|#.....#
// #.#|||#|#|
// ...#.||...
// .|....|...
// ||...#|.#|
// |.||||..|.
// ...#.|..|.
//
// After 1 minute:
// .......##.
// ......|###
// .|..|...#.
// ..|#||...#
// ..##||.|#|
// ...#||||..
// ||...|||..
// |||||.||.|
// ||||||||||
// ....||..|.
//
// After 2 minutes:
// .......#..
// ......|#..
// .|.|||....
// ..##|||..#
// ..###|||#|
// ...#|||||.
// |||||||||.
// ||||||||||
// ||||||||||
// .|||||||||
//
// After 3 minutes:
// .......#..
// ....|||#..
// .|.||||...
// ..###|||.#
// ...##|||#|
// .||##|||||
// ||||||||||
// ||||||||||
// ||||||||||
// ||||||||||
//
// After 4 minutes:
// .....|.#..
// ...||||#..
// .|.#||||..
// ..###||||#
// ...###||#|
// |||##|||||
// ||||||||||
// ||||||||||
// ||||||||||
// ||||||||||
//
// After 5 minutes:
// ....|||#..
// ...||||#..
// .|.##||||.
// ..####|||#
// .|.###||#|
// |||###||||
// ||||||||||
// ||||||||||
// ||||||||||
// ||||||||||
//
// After 6 minutes:
// ...||||#..
// ...||||#..
// .|.###|||.
// ..#.##|||#
// |||#.##|#|
// |||###||||
// ||||#|||||
// ||||||||||
// ||||||||||
// ||||||||||
//
// After 7 minutes:
// ...||||#..
// ..||#|##..
// .|.####||.
// ||#..##||#
// ||##.##|#|
// |||####|||
// |||###||||
// ||||||||||
// ||||||||||
// ||||||||||
//
// After 8 minutes:
// ..||||##..
// ..|#####..
// |||#####|.
// ||#...##|#
// ||##..###|
// ||##.###||
// |||####|||
// ||||#|||||
// ||||||||||
// ||||||||||
//
// After 9 minutes:
// ..||###...
// .||#####..
// ||##...##.
// ||#....###
// |##....##|
// ||##..###|
// ||######||
// |||###||||
// ||||||||||
// ||||||||||
//
// After 10 minutes:
// .||##.....
// ||###.....
// ||##......
// |##.....##
// |##.....##
// |##....##|
// ||##.####|
// ||#####|||
// ||||#|||||
// ||||||||||
//
// After 10 minutes, there are 37 wooded acres and 31 lumberyards.
// Multiplying the number of wooded acres by the number of lumberyards
// gives the total resource value after ten minutes: 37 * 31 = 1147.
//
// What will the total resource value of the lumber collection area be
// after 10 minutes?
//
// --- Part Two ---
// ----------------
//
// This important natural resource will need to last for at least
// thousands of years. Are the Elves collecting this lumber sustainably?
//
// What will the total resource value of the lumber collection area be
// after 1000000000 minutes?

#include "advent_of_code/2018/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

CharBoard Update(const CharBoard& in) {
  CharBoard ret = in;
  for (Point p : in.range()) {
    absl::flat_hash_map<char, int> neighbors;
    for (Point dir : Cardinal::kEightDirs) {
      Point test = p + dir;
      if (in.OnBoard(test)) {
        ++neighbors[in[test]];
      }
    }
    switch (in[p]) {
      case '.': {
        if (neighbors['|'] >= 3) {
          ret[p] = '|';
        }
        break;
      }
      case '|': {
        if (neighbors['#'] >= 3) {
          ret[p] = '#';
        }
        break;
      }
      case '#': {
        if (neighbors['#'] < 1 || neighbors['|'] < 1) {
          ret[p] = '.';
        }
        break;
      }
      default:
        LOG(FATAL) << "Bad board char @" << p << ": " << in[p];
    }
  }
  return ret;
}
// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_18::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> in = ParseAsBoard(input);
  if (!in.ok()) return in.status();
  CharBoard step = *in;
  for (int i = 0; i < 10; ++i) {
    VLOG(1) << "Step [" << i << "]:\n" << step;
    step = Update(step);
  }
  int trees = step.CountChar('|');
  int lumber = step.CountChar('#');
  return IntReturn(trees * lumber);
}

absl::StatusOr<std::string> Day_2018_18::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> in = ParseAsBoard(input);
  if (!in.ok()) return in.status();
  CharBoard step = *in;
  constexpr int kNumSteps = 1'000'000'000;
  absl::flat_hash_map<CharBoard, int> hist;
  for (int i = 0; i < kNumSteps; ++i) {
    if (hist.contains(step)) {
      int loop_size = i - hist[step];
      int loop_offset = hist[step];
      LOG(INFO) << "Found loop at " << loop_offset << " of size " << loop_size;
      int final_idx = (kNumSteps - loop_offset) % loop_size + loop_offset;
      bool found = false;
      for (const auto& [board, idx] : hist) {
        if (idx == final_idx) {
          step = board;
          found = true;
          break;
        }
      }
      if (!found) return Error("Could not find loop value!");
      break;
    }
    hist[step] = i;
    VLOG(1) << "Step [" << i << "]:\n" << step;
    step = Update(step);
  }
  int trees = step.CountChar('|');
  int lumber = step.CountChar('#');
  return IntReturn(trees * lumber);
}

}  // namespace advent_of_code
