// https://adventofcode.com/2019/day/20
//
// --- Day 20: Donut Maze ---
// --------------------------
//
// You notice a strange pattern on the surface of Pluto and land nearby
// to get a closer look. Upon closer inspection, you realize you've come
// across one of the famous space-warping mazes of the long-lost Pluto
// civilization!
//
// Because there isn't much space on Pluto, the civilization that used to
// live here thrived by inventing a method for folding spacetime.
// Although the technology is no longer understood, mazes like this one
// provide a small glimpse into the daily life of an ancient Pluto
// citizen.
//
// This maze is shaped like a donut. Portals along the inner and outer
// edge of the donut can instantly teleport you from one side to the
// other. For example:
//
// A
// A
// #######.#########
// #######.........#
// #######.#######.#
// #######.#######.#
// #######.#######.#
// #####  B    ###.#
// BC...##  C    ###.#
// ##.##       ###.#
// ##...DE  F  ###.#
// #####    G  ###.#
// #########.#####.#
// DE..#######...###.#
// #.#########.###.#
// FG..#########.....#
// ###########.#####
// Z
// Z
//
// This map of the maze shows solid walls (#) and open passages (.).
// Every maze on Pluto has a start (the open tile next to AA) and an end
// (the open tile next to ZZ). Mazes on Pluto also have portals; this
// maze has three pairs of portals: BC, DE, and FG. When on an open tile
// next to one of these labels, a single step can take you to the other
// tile with the same label. (You can only walk on . tiles; labels and
// empty space are not traversable.)
//
// One path through the maze doesn't require any portals. Starting at AA,
// you could go down 1, right 8, down 12, left 4, and down 1 to reach ZZ,
// a total of 26 steps.
//
// However, there is a shorter path: You could walk from AA to the inner
// BC portal (4 steps), warp to the outer BC portal (1 step), walk to the
// inner DE (6 steps), warp to the outer DE (1 step), walk to the outer
// FG (4 steps), warp to the inner FG (1 step), and finally walk to ZZ (6
// steps). In total, this is only 23 steps.
//
// Here is a larger example:
//
// A
// A
// #################.#############
// #.#...#...................#.#.#
// #.#.#.###.###.###.#########.#.#
// #.#.#.......#...#.....#.#.#...#
// #.#########.###.#####.#.#.###.#
// #.............#.#.....#.......#
// ###.###########.###.#####.#.#.#
// #.....#        A   C    #.#.#.#
// #######        S   P    #####.#
// #.#...#                 #......VT
// #.#.#.#                 #.#####
// #...#.#               YN....#.#
// #.###.#                 #####.#
// DI....#.#                 #.....#
// #####.#                 #.###.#
// ZZ......#               QG....#..AS
// ###.###                 #######
// JO..#.#.#                 #.....#
// #.#.#.#                 ###.#.#
// #...#..DI             BU....#..LF
// #####.#                 #.#####
// YN......#               VT..#....QG
// #.###.#                 #.###.#
// #.#...#                 #.....#
// ###.###    J L     J    #.#.###
// #.....#    O F     P    #.#...#
// #.###.#####.#.#####.#####.###.#
// #...#.#.#...#.....#.....#.#...#
// #.#####.###.###.#.#.#########.#
// #...#.#.....#...#.#.#.#.....#.#
// #.###.#####.###.###.#.#.#######
// #.#.........#...#.............#
// #########.###.###.#############
// B   J   C
// U   P   P
//
// Here, AA has no direct path to ZZ, but it does connect to AS and CP.
// By passing through AS, QG, BU, and JO, you can reach ZZ in 58 steps.
//
// In your maze, how many steps does it take to get from the open tile
// marked AA to the open tile marked ZZ?
//
// --- Part Two ---
// ----------------
//
// Strangely, the exit isn't open when you reach it. Then, you remember:
// the ancient Plutonians were famous for building recursive spaces.
//
// The marked connections in the maze aren't portals: they physically
// connect to a larger or smaller copy of the maze. Specifically, the
// labeled tiles around the inside edge actually connect to a smaller
// copy of the same maze, and the smaller copy's inner labeled tiles
// connect to yet a smaller copy, and so on.
//
// When you enter the maze, you are at the outermost level; when at the
// outermost level, only the outer labels AA and ZZ function (as the
// start and end, respectively); all other outer labeled tiles are
// effectively walls. At any other level, AA and ZZ count as walls, but
// the other outer labeled tiles bring you one level outward.
//
// Your goal is to find a path through the maze that brings you back to
// ZZ at the outermost level of the maze.
//
// In the first example above, the shortest path is now the loop around
// the right side. If the starting level is 0, then taking the
// previously-shortest path would pass through BC (to level 1), DE (to
// level 2), and FG (back to level 1). Because this is not the outermost
// level, ZZ is a wall, and the only option is to go back around to BC,
// which would only send you even deeper into the recursive maze.
//
// In the second example above, there is no path that brings you to ZZ at
// the outermost level.
//
// Here is a more interesting example:
//
// Z L X W       C
// Z P Q B       K
// ###########.#.#.#.#######.###############
// #...#.......#.#.......#.#.......#.#.#...#
// ###.#.#.#.#.#.#.#.###.#.#.#######.#.#.###
// #.#...#.#.#...#.#.#...#...#...#.#.......#
// #.###.#######.###.###.#.###.###.#.#######
// #...#.......#.#...#...#.............#...#
// #.#########.#######.#.#######.#######.###
// #...#.#    F       R I       Z    #.#.#.#
// #.###.#    D       E C       H    #.#.#.#
// #.#...#                           #...#.#
// #.###.#                           #.###.#
// #.#....OA                       WB..#.#..ZH
// #.###.#                           #.#.#.#
// CJ......#                           #.....#
// #######                           #######
// #.#....CK                         #......IC
// #.###.#                           #.###.#
// #.....#                           #...#.#
// ###.###                           #.#.#.#
// XF....#.#                         RF..#.#.#
// #####.#                           #######
// #......CJ                       NM..#...#
// ###.#.#                           #.###.#
// RE....#.#                           #......RF
// ###.###        X   X       L      #.#.#.#
// #.....#        F   Q       P      #.#.#.#
// ###.###########.###.#######.#########.###
// #.....#...#.....#.......#...#.....#.#...#
// #####.#.###.#######.#######.###.###.#.#.#
// #.......#.......#.#.#.#.#...#...#...#.#.#
// #####.###.#####.#.#.#.#.###.###.#.###.###
// #.......#.....#.#...#...............#...#
// #############.#.#.###.###################
// A O F   N
// A A D   M
//
// One shortest path through the maze is the following:
//
// * Walk from AA to XF (16 steps)
//
// * Recurse into level 1 through XF (1 step)
//
// * Walk from XF to CK (10 steps)
//
// * Recurse into level 2 through CK (1 step)
//
// * Walk from CK to ZH (14 steps)
//
// * Recurse into level 3 through ZH (1 step)
//
// * Walk from ZH to WB (10 steps)
//
// * Recurse into level 4 through WB (1 step)
//
// * Walk from WB to IC (10 steps)
//
// * Recurse into level 5 through IC (1 step)
//
// * Walk from IC to RF (10 steps)
//
// * Recurse into level 6 through RF (1 step)
//
// * Walk from RF to NM (8 steps)
//
// * Recurse into level 7 through NM (1 step)
//
// * Walk from NM to LP (12 steps)
//
// * Recurse into level 8 through LP (1 step)
//
// * Walk from LP to FD (24 steps)
//
// * Recurse into level 9 through FD (1 step)
//
// * Walk from FD to XQ (8 steps)
//
// * Recurse into level 10 through XQ (1 step)
//
// * Walk from XQ to WB (4 steps)
//
// * Return to level 9 through WB (1 step)
//
// * Walk from WB to ZH (10 steps)
//
// * Return to level 8 through ZH (1 step)
//
// * Walk from ZH to CK (14 steps)
//
// * Return to level 7 through CK (1 step)
//
// * Walk from CK to XF (10 steps)
//
// * Return to level 6 through XF (1 step)
//
// * Walk from XF to OA (14 steps)
//
// * Return to level 5 through OA (1 step)
//
// * Walk from OA to CJ (8 steps)
//
// * Return to level 4 through CJ (1 step)
//
// * Walk from CJ to RE (8 steps)
//
// * Return to level 3 through RE (1 step)
//
// * Walk from RE to IC (4 steps)
//
// * Recurse into level 4 through IC (1 step)
//
// * Walk from IC to RF (10 steps)
//
// * Recurse into level 5 through RF (1 step)
//
// * Walk from RF to NM (8 steps)
//
// * Recurse into level 6 through NM (1 step)
//
// * Walk from NM to LP (12 steps)
//
// * Recurse into level 7 through LP (1 step)
//
// * Walk from LP to FD (24 steps)
//
// * Recurse into level 8 through FD (1 step)
//
// * Walk from FD to XQ (8 steps)
//
// * Recurse into level 9 through XQ (1 step)
//
// * Walk from XQ to WB (4 steps)
//
// * Return to level 8 through WB (1 step)
//
// * Walk from WB to ZH (10 steps)
//
// * Return to level 7 through ZH (1 step)
//
// * Walk from ZH to CK (14 steps)
//
// * Return to level 6 through CK (1 step)
//
// * Walk from CK to XF (10 steps)
//
// * Return to level 5 through XF (1 step)
//
// * Walk from XF to OA (14 steps)
//
// * Return to level 4 through OA (1 step)
//
// * Walk from OA to CJ (8 steps)
//
// * Return to level 3 through CJ (1 step)
//
// * Walk from CJ to RE (8 steps)
//
// * Return to level 2 through RE (1 step)
//
// * Walk from RE to XQ (14 steps)
//
// * Return to level 1 through XQ (1 step)
//
// * Walk from XQ to FD (8 steps)
//
// * Return to level 0 through FD (1 step)
//
// * Walk from FD to ZZ (18 steps)
//
// This path takes a total of 396 steps to move from AA at the outermost
// layer to ZZ at the outermost layer.
//
// In your maze, when accounting for recursion, how many steps does it
// take to get from the open tile marked AA to the open tile marked ZZ,
// both at the outermost layer?

#include "advent_of_code/2019/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

bool IsCapAlpha(char c) { return c >= 'A' && c <= 'Z'; }

class Maze {
 public:
  static absl::StatusOr<Maze> Parse(absl::Span<absl::string_view> input) {
    while (!input.empty() && input.back() == "") {
      input = input.subspan(0, input.size() - 1);
    }
    absl::StatusOr<CharBoard> with_portals = CharBoard::Parse(input);
    if (!with_portals.ok()) return with_portals.status();

    absl::flat_hash_map<std::string, std::vector<Point>> portals;

    PointRectangle range = with_portals->range();
    range.min += Point{2, 2};
    range.max -= Point{2, 2};

    Maze ret(with_portals->width() - 4, with_portals->height() - 4);
    absl::StatusOr<CharBoard> sub_board = with_portals->SubBoard(range);
    if (!sub_board.ok()) return sub_board.status();
    ret.board_ = std::move(*sub_board);
    for (Point p : range) {
      if (with_portals->at(p) == '.') {
        if (IsCapAlpha(with_portals->at(p + Point{0, -1}))) {
          char portal_name[] = {with_portals->at(p + Point{0, -2}),
                                with_portals->at(p + Point{0, -1}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
        if (IsCapAlpha(with_portals->at(p + Point{0, 1}))) {
          char portal_name[] = {with_portals->at(p + Point{0, 1}),
                                with_portals->at(p + Point{0, 2}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
        if (IsCapAlpha(with_portals->at(p + Point{-1, 0}))) {
          char portal_name[] = {with_portals->at(p + Point{-2, 0}),
                                with_portals->at(p + Point{-1, 0}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
        if (IsCapAlpha(with_portals->at(p + Point{1, 0}))) {
          char portal_name[] = {with_portals->at(p + Point{1, 0}),
                                with_portals->at(p + Point{2, 0}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
      }
    }
    {
      auto it = portals.find("AA");
      if (it == portals.end()) return absl::InvalidArgumentError("No start");
      if (it->second.size() != 1)
        return absl::InvalidArgumentError("Bad start");
      ret.start_ = it->second[0];
      portals.erase(it);
    }
    {
      auto it = portals.find("ZZ");
      if (it == portals.end()) return absl::InvalidArgumentError("No end");
      if (it->second.size() != 1) return absl::InvalidArgumentError("Bad end");
      ret.end_ = it->second[0];
      portals.erase(it);
    }
    for (const auto& name_to_points : portals) {
      const std::vector<Point>& points = name_to_points.second;
      if (points.size() != 2)
        return absl::InvalidArgumentError("Portal not connected right");
      ret.portals_.emplace(points[0], points[1]);
      ret.portals_.emplace(points[1], points[0]);
    }

    VLOG(1) << "\nBoard:\n"
            << ret.board_ << "\nStart: " << ret.start_ << "; End: " << ret.end_
            << "\nPortals:\n"
            << absl::StrJoin(
                   ret.portals_, "\n",
                   [](std::string* out, const std::pair<Point, Point>& portal) {
                     absl::StrAppend(out, portal.first, "<->", portal.second);
                   });
    return ret;
  }

  bool CanStand(Point p) const {
    if (!board_.OnBoard(p)) return false;
    return board_[p] == '.';
  }

  bool OnEdge(Point p) const {
    if (p.x == 0) return true;
    if (p.y == 0) return true;
    if (p.y == board_.height() - 1) return true;
    if (p.x == board_.width() - 1) return true;
    return false;
  }

  absl::optional<int> FindPath() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const Maze& maze, Point start, Point end)
          : maze_(maze), cur_(start), end_(end) {}

      Point identifier() const override { return cur_; }
      bool IsFinal() const override { return cur_ == end_; }

      void AddNextSteps(State* state) const override {
        for (Point dir : Cardinal::kFourDirs) {
          Point next_cur = cur_ + dir;
          if (maze_.CanStand(next_cur)) {
            PathWalk next = *this;
            next.cur_ = next_cur;
            state->AddNextStep(next);
          }
        }
        if (auto it = maze_.portals().find(cur_); it != maze_.portals().end()) {
          PathWalk next = *this;
          next.cur_ = it->second;
          state->AddNextStep(next);
        }
      }

     private:
      const Maze& maze_;
      Point cur_;
      Point end_;
    };
    return PathWalk(*this, start_, end_).FindMinSteps();
  }

  absl::optional<int> FindRecursivePath() {
    class PathWalk : public BFSInterface<PathWalk, Point3> {
     public:
      PathWalk(const Maze& maze, Point start, Point end)
          : maze_(maze), cur_({start.x, start.y, 0}), end_({end.x, end.y, 0}) {}

      Point3 identifier() const override { return cur_; }
      bool IsFinal() const override { return cur_ == end_; }

      void AddNextSteps(State* state) const override {
        for (Point dir : Cardinal::kFourDirs) {
          Point3 next_cur = cur_ + Point3{dir.x, dir.y, 0};
          if (maze_.CanStand({next_cur.x, next_cur.y})) {
            PathWalk next = *this;
            next.cur_ = next_cur;
            state->AddNextStep(next);
          }
        }
        if (auto it = maze_.portals().find({cur_.x, cur_.y});
            it != maze_.portals().end()) {
          PathWalk next = *this;
          next.cur_ = {it->second.x, it->second.y,
                       cur_.z + (maze_.OnEdge({cur_.x, cur_.y}) ? -1 : +1)};
          if (next.cur_.z >= 0) {
            state->AddNextStep(next);
          }
        }
      }

     private:
      const Maze& maze_;
      Point3 cur_;
      Point3 end_;
    };
    return PathWalk(*this, start_, end_).FindMinSteps();
  }

  const absl::flat_hash_map<Point, Point>& portals() const { return portals_; }

 private:
  Maze(int width, int height) : board_(width, height) {}

  CharBoard board_;
  absl::flat_hash_map<Point, Point> portals_;
  Point start_;
  Point end_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Maze> maze = Maze::Parse(input);
  if (!maze.ok()) return maze.status();

  return IntReturn(maze->FindPath());
}

absl::StatusOr<std::string> Day_2019_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Maze> maze = Maze::Parse(input);
  if (!maze.ok()) return maze.status();

  return IntReturn(maze->FindRecursivePath());
}

}  // namespace advent_of_code
