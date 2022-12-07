// https://adventofcode.com/2019/day/10
//
// --- Day 10: Monitoring Station ---
// ----------------------------------
//
// You fly into the asteroid belt and reach the Ceres monitoring station.
// The Elves here have an emergency: they're having trouble tracking all
// of the asteroids and can't be sure they're safe.
//
// The Elves would like to build a new monitoring station in a nearby
// area of space; they hand you a map of all of the asteroids in that
// region (your puzzle input).
//
// The map indicates whether each position is empty (.) or contains an
// asteroid (#). The asteroids are much smaller than they appear on the
// map, and every asteroid is exactly in the center of its marked
// position. The asteroids can be described with X,Y coordinates where X
// is the distance from the left edge and Y is the distance from the top
// edge (so the top-left corner is 0,0 and the position immediately to
// its right is 1,0).
//
// Your job is to figure out which asteroid would be the best place to
// build a new monitoring station. A monitoring station can detect any
// asteroid to which it has direct line of sight - that is, there cannot
// be another asteroid exactly between them. This line of sight can be at
// any angle, not just lines aligned to the grid or diagonally. The best
// location is the asteroid that can detect the largest number of other
// asteroids.
//
// For example, consider the following map:
//
// .#..#
// .....
// #####
// ....#
// ...   #   #
//
// The best location for a new monitoring station on this map is the
// highlighted asteroid at 3,4 because it can detect 8 asteroids, more
// than any other location. (The only asteroid it cannot detect is the
// one at 1,0; its view of this asteroid is blocked by the asteroid at
// 2,2.) All other asteroids are worse locations; they can detect 7 or
// fewer other asteroids. Here is the number of other asteroids a
// monitoring station on each asteroid could detect:
//
// .7..7
// .....
// 67775
// ....7
// ...87
//
// Here is an asteroid (#) and some examples of the ways its line of
// sight might be blocked. If there were another asteroid at the location
// of a capital letter, the locations marked with the corresponding
// lowercase letter would be blocked and could not be detected:
//
// #.........
// ...A......
// ...B..a...
// .EDCG....a
// ..F.c.b...
// .....c....
// ..efd.c.gb
// .......c..
// ....f...c.
// ...e..d..c
//
// Here are some larger examples:
//
// *
//
// Best is 5,8 with 33 other asteroids detected:
//
// ......#.#.
// #..#.#....
// ..#######.
// .#.#.###..
// .#..#.....
// ..#....#.#
// #..#....#.
// .##.#..###
// ##...       #       ..#.
// .#....####
//
// *
//
// Best is 1,2 with 35 other asteroids detected:
//
// #.#...#.#.
// .###....#.
// .       #       ....#...
// ##.#.#.#.#
// ....#.#.#.
// .##..###.#
// ..#...##..
// ..##....##
// ......#...
// .####.###.
//
// *
//
// Best is 6,3 with 41 other asteroids detected:
//
// .#..#..###
// ####.###.#
// ....###.#.
// ..###.       #       #.#
// ##.##.#.#.
// ....###..#
// ..#.#..#.#
// #..#.#.###
// .##...##.#
// .....#.#..
//
// *
//
// Best is 11,13 with 210 other asteroids detected:
//
// .#..##.###...#######
// ##.############..##.
// .#.######.########.#
// .###.#######.####.#.
// #####.##.#.##.###.##
// ..#####..#.#########
// ####################
// #.####....###.#.#.##
// ##.#################
// #####.##.###..####..
// ..######..##.#######
// ####.##.####...##..#
// .#####..#.######.###
// ##...#.####       #       #####...
// #.##########.#######
// .####.#.###.###.#.##
// ....##.##.###..#####
// .#.#.###########.###
// #.#.#.#####.####.###
// ###.##.####.##.#..##
//
// Find the best location for a new monitoring station. How many other
// asteroids can be detected from that location?
//
// --- Part Two ---
// ----------------
//
// Once you give them the coordinates, the Elves quickly deploy an
// Instant Monitoring Station to the location and discover the worst:
// there are simply too many asteroids.
//
// The only solution is complete vaporization by giant laser.
//
// Fortunately, in addition to an asteroid scanner, the new monitoring
// station also comes equipped with a giant rotating laser perfect for
// vaporizing asteroids. The laser starts by pointing up and always
// rotates clockwise, vaporizing any asteroid it hits.
//
// If multiple asteroids are exactly in line with the station, the laser
// only has enough power to vaporize one of them before continuing its
// rotation. In other words, the same asteroids that can be detected can
// be vaporized, but if vaporizing one asteroid makes another one
// detectable, the newly-detected asteroid won't be vaporized until the
// laser has returned to the same position by rotating a full 360
// degrees.
//
// For example, consider the following map, where the asteroid with the
// new monitoring station (and laser) is marked X:
//
// .#....#####...#..
// ##...##.#####..##
// ##...#...#.#####.
// ..#.....X...###..
// ..#.#.....#....##
//
// The first nine asteroids to get vaporized, in order, would be:
//
// .#....###   2   4   ...#..
// ##...##.   1   3   #   6   7   ..   9   #
// ##...#...   5   .   8   ####.
// ..#.....X...###..
// ..#.#.....#....##
//
// Note that some asteroids (the ones behind the asteroids marked 1, 5,
// and 7) won't have a chance to be vaporized until the next full
// rotation. The laser continues rotating; the next nine to be vaporized
// are:
//
// .#....###.....#..
// ##...##...#.....#
// ##...#......   1   2   3   4   .
// ..#.....X...   5   ##..
// ..#.   9   .....   8   ....   7   6
//
// The next nine to be vaporized are then:
//
// .   8   ....###.....#..   5   6   ...   9   #...#.....#   3   4   ...   7   ...........
// ..   2   .....X....##..
// ..   1   ..............
//
// Finally, the laser completes its first full rotation (1 through 3), a
// second rotation (4 through 8), and vaporizes the last asteroid (9)
// partway through its third rotation:
//
// ......   2   3   4   .....   6   ..
// ......   1   ...   5   .....   7   .................
// ........X....   8   9   ..
// .................
//
// In the large example above (the one with the best monitoring station
// location at 11,13):
//
// * The 1st asteroid to be vaporized is at 11,12.
//
// * The 2nd asteroid to be vaporized is at 12,1.
//
// * The 3rd asteroid to be vaporized is at 12,2.
//
// * The 10th asteroid to be vaporized is at 12,8.
//
// * The 20th asteroid to be vaporized is at 16,0.
//
// * The 50th asteroid to be vaporized is at 16,9.
//
// * The 100th asteroid to be vaporized is at 10,16.
//
// * The 199th asteroid to be vaporized is at 9,6.
//
// * The 200th asteroid to be vaporized is at 8,2.
//
// * The 201st asteroid to be vaporized is at 10,9.
//
// * The 299th and final asteroid to be vaporized is at 11,1.
//
// The Elves are placing bets on which will be the 200th asteroid to be
// vaporized. Win the bet by determining which asteroid that will be;
// what do you get if you multiply its X coordinate by 100 and then add
// its Y coordinate? (For example, 8,2 becomes 802.)

#include "advent_of_code/2019/day10/day10.h"

#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {
namespace {

struct Board {
  absl::flat_hash_set<Point> asteroids;
  int width;
  int height;
};

int ObscureCount(const Board& board, Point start, Point end) {
  int count = 0;
  Point delta = end - start;
  Point step = delta.min_step();
  for (Point check = start + step; check != end; check += step) {
    if (board.asteroids.contains(check)) {
      ++count;
    }
  }
  return count;
}

int CountVisible(const Board& board, Point coord) {
  int visible = 0;
  for (const auto& maybe_visible : board.asteroids) {
    if (maybe_visible == coord) continue;

    if (ObscureCount(board, coord, maybe_visible) > 0) continue;
    ++visible;
  }
  return visible;
}

struct MostVisible {
  Point most_visible;
  int seen;
};

MostVisible FindMostVisible(const Board& board) {
  MostVisible ret{{0, 0}, 0};
  for (const Point& coord : board.asteroids) {
    int visible = CountVisible(board, coord);
    if (visible > ret.seen) {
      ret.seen = visible;
      ret.most_visible = coord;
    }
  }
  return ret;
}

absl::StatusOr<Board> ParseBoard(absl::Span<absl::string_view> input) {
  absl::StatusOr<CharBoard> char_board = CharBoard::Parse(input);
  if (!char_board.ok()) return char_board.status();

  Board ret;
  ret.height = char_board->height();
  ret.width = char_board->width();
  for (Point p : char_board->range()) {
    switch ((*char_board)[p]) {
      case '.': {
        break;
      }
      case 'X': {
        ret.asteroids.insert(p);
        break;
      }
      default:
        return absl::InvalidArgumentError("bad token");
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_10::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Board> asteroids = ParseBoard(input);
  if (!asteroids.ok()) return asteroids.status();
  MostVisible most_visible = FindMostVisible(*asteroids);

  return IntReturn(most_visible.seen);
}

struct OrderedDestruct {
  Point p;
  double theta;
  int count;
  bool operator<(const OrderedDestruct& other) const {
    if (count < other.count) return true;
    if (count > other.count) return false;
    if (theta < other.theta) return true;
    if (theta > other.theta) return false;
    // Equal.
    return false;
  }
};

absl::StatusOr<std::string> Day_2019_10::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Board> board = ParseBoard(input);
  if (!board.ok()) return board.status();
  if (board->asteroids.size() < 201) {
    return absl::InvalidArgumentError("There aren't 200 asteroids to destroy");
  }
  MostVisible most_visible = FindMostVisible(*board);

  std::vector<OrderedDestruct> ordered_destruct;
  Point origin = most_visible.most_visible;
  for (const Point& p : board->asteroids) {
    if (p == origin) continue;
    Point delta = p - origin;
    // atan2(y,x) gives the angle from the x-axis, towards the y-axis.
    // We want angle from the -y-axis towards the +x-axis, so the args
    // are swapped and y is negated.
    double theta = atan2(delta.x, -delta.y);
    if (theta < 0) {
      // We want angles from [0, 2*PI), but atan2 returns [-PI, PI).
      theta += 8 * atan2(1, 1) /* 2 * pi */;
    }
    ordered_destruct.push_back(
        {.p = p, .theta = theta, .count = ObscureCount(*board, origin, p)});
  }
  std::sort(ordered_destruct.begin(), ordered_destruct.end());
  int ret = ordered_destruct[199].p.x * 100 + ordered_destruct[199].p.y;
  return IntReturn(ret);
}

}  // namespace advent_of_code
