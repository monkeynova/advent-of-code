// https://adventofcode.com/2018/day/15
//
// --- Day 15: Beverage Bandits ---
// --------------------------------
//
// Having perfected their hot chocolate, the Elves have a new problem:
// the Goblins that live in these caves will do anything to steal it.
// Looks like they're here for a fight.
//
// You scan the area, generating a map of the walls (#), open cavern (.),
// and starting position of every Goblin (G) and Elf (E) (your puzzle
// input).
//
// Combat proceeds in rounds; in each round, each unit that is still
// alive takes a turn, resolving all of its actions before the next
// unit's turn begins. On each unit's turn, it tries to move into range
// of an enemy (if it isn't already) and then attack (if it is in range).
//
// All units are very disciplined and always follow very strict combat
// rules. Units never move or attack diagonally, as doing so would be
// dishonorable. When multiple choices are equally valid, ties are broken
// in reading order: top-to-bottom, then left-to-right. For instance, the
// order in which units take their turns within a round is the reading
// order of their starting positions in that round, regardless of the
// type of unit or whether other units have moved after the round
// started. For example:
//
// would take their
// These units:   turns in this order:
// #######           #######
// #.G.E.#           #.1.2.#
// #E.G.E#           #3.4.5#
// #.G.E.#           #.6.7.#
// #######           #######
//
// Each unit begins its turn by identifying all possible targets (enemy
// units). If no targets remain, combat ends.
//
// Then, the unit identifies all of the open squares (.) that are in
// range of each target; these are the squares which are adjacent
// (immediately up, down, left, or right) to any target and which aren't
// already occupied by a wall or another unit. Alternatively, the unit
// might already be in range of a target. If the unit is not already in
// range of a target, and there are no open squares which are in range of
// a target, the unit ends its turn.
//
// If the unit is already in range of a target, it does not move, but
// continues its turn with an attack. Otherwise, since it is not in range
// of a target, it moves.
//
// To move, the unit first considers the squares that are in range and
// determines which of those squares it could reach in the fewest steps.
// A step is a single movement to any adjacent (immediately up, down,
// left, or right) open (.) square. Units cannot move into walls or other
// units. The unit does this while considering the current positions of
// units and does not do any prediction about where units will be later.
// If the unit cannot reach (find an open path to) any of the squares
// that are in range, it ends its turn. If multiple squares are in range
// and tied for being reachable in the fewest steps, the square which is
// first in reading order is chosen. For example:
//
// Targets:      In range:     Reachable:    Nearest:      Chosen:
// #######       #######       #######       #######       #######
// #E..G.#       #E.?G?#       #E.@G.#       #E.!G.#       #E.+G.#
// #...#.#  -->  #.?.#?#  -->  #.@.#.#  -->  #.!.#.#  -->  #...#.#
// #.G.#G#       #?G?#G#       #@G@#G#       #!G.#G#       #.G.#G#
// #######       #######       #######       #######       #######
//
// In the above scenario, the Elf has three targets (the three Goblins):
//
// * Each of the Goblins has open, adjacent squares which are in range
// (marked with a ? on the map).
//
// * Of those squares, four are reachable (marked @); the other two (on
// the right) would require moving through a wall or unit to reach.
//
// * Three of these reachable squares are nearest, requiring the fewest
// steps (only 2) to reach (marked !).
//
// * Of those, the square which is first in reading order is chosen (+).
//
// The unit then takes a single step toward the chosen square along the
// shortest path to that square. If multiple steps would put the unit
// equally closer to its destination, the unit chooses the step which is
// first in reading order. (This requires knowing when there is more than
// one shortest path so that you can consider the first step of each such
// path.) For example:
//
// In range:     Nearest:      Chosen:       Distance:     Step:
// #######       #######       #######       #######       #######
// #.E...#       #.E...#       #.E...#       #4E   2   12#       #..E..#
// #...?.#  -->  #...!.#  -->  #...+.#  -->  #3   2   101#  -->  #.....#
// #..?G?#       #..!G.#       #...G.#       #432G2#       #...G.#
// #######       #######       #######       #######       #######
//
// The Elf sees three squares in range of a target (?), two of which are
// nearest (!), and so the first in reading order is chosen (+). Under
// "Distance", each open square is marked with its distance from the
// destination square; the two squares to which the Elf could move on
// this turn (down and to the right) are both equally good moves and
// would leave the Elf 2 steps from being in range of the Goblin. Because
// the step which is first in reading order is chosen, the Elf moves
// right one square.
//
// Here's a larger example of movement:
//
// Initially:
// #########
// #G..G..G#
// #.......#
// #.......#
// #G..E..G#
// #.......#
// #.......#
// #G..G..G#
// #########
//
// After 1 round:
// #########
// #.G...G.#
// #...G...#
// #...E..G#
// #.G.....#
// #.......#
// #G..G..G#
// #.......#
// #########
//
// After 2 rounds:
// #########
// #..G.G..#
// #...G...#
// #.G.E.G.#
// #.......#
// #G..G..G#
// #.......#
// #.......#
// #########
//
// After 3 rounds:
// #########
// #.......#
// #..GGG..#
// #..GEG..#
// #G..G...#
// #......G#
// #.......#
// #.......#
// #########
//
// Once the Goblins and Elf reach the positions above, they all are
// either in range of a target or cannot find any square in range of a
// target, and so none of the units can move until a unit dies.
//
// After moving (or if the unit began its turn in range of a target), the
// unit attacks.
//
// To attack, the unit first determines all of the targets that are in
// range of it by being immediately adjacent to it. If there are no such
// targets, the unit ends its turn. Otherwise, the adjacent target with
// the fewest hit points is selected; in a tie, the adjacent target with
// the fewest hit points which is first in reading order is selected.
//
// The unit deals damage equal to its attack power to the selected
// target, reducing its hit points by that amount. If this reduces its
// hit points to 0 or fewer, the selected target dies: its square becomes
// . and it takes no further turns.
//
// Each unit, either Goblin or Elf, has 3 attack power and starts with
// 200 hit points.
//
// For example, suppose the only Elf is about to attack:
//
// HP:            HP:
// G....  9       G....  9
// ..G..  4       ..G..  4
// ..E   G   .  2  -->  ..E..
// ..G..  2       ..G..  2
// ...G.  1       ...G.  1
//
// The "HP" column shows the hit points of the Goblin to the left in the
// corresponding row. The Elf is in range of three targets: the Goblin
// above it (with 4 hit points), the Goblin to its right (with 2 hit
// points), and the Goblin below it (also with 2 hit points). Because
// three targets are in range, the ones with the lowest hit points are
// selected: the two Goblins with 2 hit points each (one to the right of
// the Elf and one below the Elf). Of those, the Goblin first in reading
// order (the one to the right of the Elf) is selected. The selected
// Goblin's hit points (2) are reduced by the Elf's attack power (3),
// reducing its hit points to -1, killing it.
//
// After attacking, the unit's turn ends. Regardless of how the unit's
// turn ends, the next unit in the round takes its turn. If all units
// have taken turns in this round, the round ends, and a new round
// begins.
//
// The Elves look quite outnumbered. You need to determine the outcome of
// the battle: the number of full rounds that were completed (not
// counting the round in which combat ends) multiplied by the sum of the
// hit points of all remaining units at the moment combat ends. (Combat
// only ends when a unit finds no targets during its turn.)
//
// Below is an entire sample combat. Next to each map, each row's units'
// hit points are listed from left to right.
//
// Initially:
// #######
// #.G...#   G(200)
// #...EG#   E(200), G(200)
// #.#.#G#   G(200)
// #..G#E#   G(200), E(200)
// #.....#
// #######
//
// After 1 round:
// #######
// #..G..#   G(200)
// #...EG#   E(197), G(197)
// #.#G#G#   G(200), G(197)
// #...#E#   E(197)
// #.....#
// #######
//
// After 2 rounds:
// #######
// #...G.#   G(200)
// #..GEG#   G(200), E(188), G(194)
// #.#.#G#   G(194)
// #...#E#   E(194)
// #.....#
// #######
//
// Combat ensues; eventually, the top Elf dies:
//
// After 23 rounds:
// #######
// #...G.#   G(200)
// #..G.G#   G(200), G(131)
// #.#.#G#   G(131)
// #...#E#   E(131)
// #.....#
// #######
//
// After 24 rounds:
// #######
// #..G..#   G(200)
// #...G.#   G(131)
// #.#G#G#   G(200), G(128)
// #...#E#   E(128)
// #.....#
// #######
//
// After 25 rounds:
// #######
// #.G...#   G(200)
// #..G..#   G(131)
// #.#.#G#   G(125)
// #..G#E#   G(200), E(125)
// #.....#
// #######
//
// After 26 rounds:
// #######
// #G....#   G(200)
// #.G...#   G(131)
// #.#.#G#   G(122)
// #...#E#   E(122)
// #..G..#   G(200)
// #######
//
// After 27 rounds:
// #######
// #G....#   G(200)
// #.G...#   G(131)
// #.#.#G#   G(119)
// #...#E#   E(119)
// #...G.#   G(200)
// #######
//
// After 28 rounds:
// #######
// #G....#   G(200)
// #.G...#   G(131)
// #.#.#G#   G(116)
// #...#E#   E(113)
// #....G#   G(200)
// #######
//
// More combat ensues; eventually, the bottom Elf dies:
//
// After 47 rounds:
// #######
// #G....#   G(200)
// #.G...#   G(131)
// #.#.#G#   G(59)
// #...#.#
// #....G#   G(200)
// #######
//
// Before the 48th round can finish, the top-left Goblin finds that there
// are no targets remaining, and so combat ends. So, the number of full
// rounds that were completed is 47, and the sum of the hit points of all
// remaining units is 200+131+59+200 = 590. From these, the outcome of
// the battle is 47 * 590 = 27730.
//
// Here are a few example summarized combats:
//
// #######       #######
// #G..#E#       #...#E#   E(200)
// #E#E.E#       #E#...#   E(197)
// #G.##.#  -->  #.E##.#   E(185)
// #...#E#       #E..#E#   E(200), E(200)
// #...E.#       #.....#
// #######       #######
//
// Combat ends after 37 full rounds
// Elves win with 982 total hit points left
// Outcome: 37 * 982 =    36334
//
// #######       #######
// #E..EG#       #.E.E.#   E(164), E(197)
// #.#G.E#       #.#E..#   E(200)
// #E.##E#  -->  #E.##.#   E(98)
// #G..#.#       #.E.#.#   E(200)
// #..E#.#       #...#.#
// #######       #######
//
// Combat ends after 46 full rounds
// Elves win with 859 total hit points left
// Outcome: 46 * 859 =    39514
//
// #######       #######
// #E.G#.#       #G.G#.#   G(200), G(98)
// #.#G..#       #.#G..#   G(200)
// #G.#.G#  -->  #..#..#
// #G..#.#       #...#G#   G(95)
// #...E.#       #...G.#   G(200)
// #######       #######
//
// Combat ends after 35 full rounds
// Goblins win with 793 total hit points left
// Outcome: 35 * 793 =    27755
//
// #######       #######
// #.E...#       #.....#
// #.#..G#       #.#G..#   G(200)
// #.###.#  -->  #.###.#
// #E#G#G#       #.#.#.#
// #...#G#       #G.G#G#   G(98), G(38), G(200)
// #######       #######
//
// Combat ends after 54 full rounds
// Goblins win with 536 total hit points left
// Outcome: 54 * 536 =    28944
//
// #########       #########
// #G......#       #.G.....#   G(137)
// #.E.#...#       #G.G#...#   G(200), G(200)
// #..##..G#       #.G##...#   G(200)
// #...##..#  -->  #...##..#
// #...#...#       #.G.#...#   G(200)
// #.G...G.#       #.......#
// #.....G.#       #.......#
// #########       #########
//
// Combat ends after 20 full rounds
// Goblins win with 937 total hit points left
// Outcome: 20 * 937 =    18740
//
// What is the outcome of the combat described in your puzzle input?
//
// --- Part Two ---
// ----------------
//
// According to your calculations, the Elves are going to lose badly.
// Surely, you won't mess up the timeline too much if you give them just
// a little advanced technology, right?
//
// You need to make sure the Elves not only win, but also suffer no
// losses: even the death of a single Elf is unacceptable.
//
// However, you can't go too far: larger changes will be more likely to
// permanently alter spacetime.
//
// So, you need to find the outcome of the battle in which the Elves have
// the lowest integer attack power (at least 4) that allows them to win
// without a single death. The Goblins always have an attack power of 3.
//
// In the first summarized example above, the lowest attack power the
// Elves need to win without losses is 15:
//
// #######       #######
// #.G...#       #..E..#   E(158)
// #...EG#       #...E.#   E(14)
// #.#.#G#  -->  #.#.#.#
// #..G#E#       #...#.#
// #.....#       #.....#
// #######       #######
//
// Combat ends after 29 full rounds
// Elves win with 172 total hit points left
// Outcome: 29 * 172 =    4988
//
// In the second example above, the Elves need only 4 attack power:
//
// #######       #######
// #E..EG#       #.E.E.#   E(200), E(23)
// #.#G.E#       #.#E..#   E(200)
// #E.##E#  -->  #E.##E#   E(125), E(200)
// #G..#.#       #.E.#.#   E(200)
// #..E#.#       #...#.#
// #######       #######
//
// Combat ends after 33 full rounds
// Elves win with 948 total hit points left
// Outcome: 33 * 948 =    31284
//
// In the third example above, the Elves need 15 attack power:
//
// #######       #######
// #E.G#.#       #.E.#.#   E(8)
// #.#G..#       #.#E..#   E(86)
// #G.#.G#  -->  #..#..#
// #G..#.#       #...#.#
// #...E.#       #.....#
// #######       #######
//
// Combat ends after 37 full rounds
// Elves win with 94 total hit points left
// Outcome: 37 * 94 =    3478
//
// In the fourth example above, the Elves need 12 attack power:
//
// #######       #######
// #.E...#       #...E.#   E(14)
// #.#..G#       #.#..E#   E(152)
// #.###.#  -->  #.###.#
// #E#G#G#       #.#.#.#
// #...#G#       #...#.#
// #######       #######
//
// Combat ends after 39 full rounds
// Elves win with 166 total hit points left
// Outcome: 39 * 166 =    6474
//
// In the last example above, the lone Elf needs 34 attack power:
//
// #########       #########
// #G......#       #.......#
// #.E.#...#       #.E.#...#   E(38)
// #..##..G#       #..##...#
// #...##..#  -->  #...##..#
// #...#...#       #...#...#
// #.G...G.#       #.......#
// #.....G.#       #.......#
// #########       #########
//
// Combat ends after 30 full rounds
// Elves win with 38 total hit points left
// Outcome: 30 * 38 =    1140
//
// After increasing the Elves' attack power until it is just barely
// enough for them to win without any Elves dying, what is the outcome of
// the combat described in your puzzle input?

#include "advent_of_code/2018/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

constexpr Point kOrderedDirs[] = {Cardinal::kNorth, Cardinal::kWest,
                                  Cardinal::kEast, Cardinal::kSouth};

struct PointLT {
  bool operator()(Point p1, Point p2) const {
    if (p1.y != p2.y) return p1.y < p2.y;
    return p1.x < p2.x;
  }
};

class PathWalk : public BFSInterface<PathWalk, Point> {
 public:
  PathWalk(const CharBoard& b, Point start, Point end)
      : board_(&b), cur_(start), end_(end) {}

  Point identifier() const override { return cur_; }

  int min_steps_to_final() const override { return (end_ - cur_).dist(); }

  bool IsFinal() const override { return cur_ == end_; }

  void AddNextSteps(State* state) const override {
    for (Point dir : kOrderedDirs) {
      Point next = cur_ + dir;
      if (!board_->OnBoard(next)) continue;
      if ((*board_)[next] == '.') {
        PathWalk add = *this;
        add.cur_ = next;
        state->AddNextStep(add);
      }
    }
  }

 private:
  // Pointer rather than reference to be swap'able for AStar's use of a
  // priority queue.
  const CharBoard* board_;
  Point cur_;
  Point end_;
};

class FindEnemyAdjacent : public BFSInterface<FindEnemyAdjacent, Point> {
 public:
  struct PointAndDistance {
    int d = std::numeric_limits<int>::max();
    Point p;
    bool operator<(const PointAndDistance& o) const {
      if (d != o.d) return d < o.d;
      return PointLT()(p, o.p);
    }
  };

  FindEnemyAdjacent(const CharBoard& b, Point start, char find,
                    PointAndDistance* ret)
      : board_(b), cur_(start), ret_(ret), find_(find) {}

  Point identifier() const override { return cur_; }

  bool IsFinal() const override { return false; }

  void AddNextSteps(State* state) const override {
    // Stop exploring if we've found a match.
    // if (num_steps() > ret_->d) return;

    for (Point dir : kOrderedDirs) {
      Point next = cur_ + dir;
      if (!board_.OnBoard(next)) continue;
      if (board_[next] == '.') {
        FindEnemyAdjacent add = *this;
        add.cur_ = next;
        state->AddNextStep(add);
      } else if (board_[next] == find_) {
        PointAndDistance test{num_steps(), cur_};
        if (test < *ret_) {
          *ret_ = test;
        }
      }
    }
  }

 private:
  const CharBoard& board_;
  Point cur_;
  PointAndDistance* ret_;
  char find_;
};

class GameBoard {
 public:
  explicit GameBoard(CharBoard board, int elf_attack = 3)
      : board_(std::move(board)), elf_attack_(elf_attack) {
    for (Point p : board_.range()) {
      if (board_[p] == 'G' || board_[p] == 'E') {
        hit_points_[p] = 200;
      }
    }
  }

  int CountElves() {
    int count = 0;
    for (const auto& [p, _] : hit_points_) {
      if (board_[p] == 'E') ++count;
    }
    return count;
  }

  int rounds() { return rounds_; }
  int TotalHitPoints() {
    int total_hp = 0;
    for (const auto& [_, hp] : hit_points_) total_hp += hp;
    return total_hp;
  }

  absl::StatusOr<Point> TryAttack(Point p) {
    char find = '\0';
    int attack;
    if (board_[p] == 'G') {
      find = 'E';
      attack = 3;
    } else if (board_[p] == 'E') {
      find = 'G';
      attack = elf_attack_;
    } else
      return Error("HP at bad location (attack): ", p);
    int fewest_hp = std::numeric_limits<int>::max();
    Point fewest_hp_location;
    for (Point dir : kOrderedDirs) {
      Point check = p + dir;
      if (board_.OnBoard(check) && board_[check] == find) {
        auto it = hit_points_.find(check);
        if (it == hit_points_.end()) return Error("No HP at location");
        if (it->second < fewest_hp) {
          fewest_hp = it->second;
          fewest_hp_location = check;
        }
      }
    }
    if (fewest_hp == std::numeric_limits<int>::max()) {
      return Point{-1, -1};
    }

    auto it = hit_points_.find(fewest_hp_location);
    if (it == hit_points_.end()) return Error("No HP at location");
    if (it->second <= attack) {
      VLOG(1) << "Opponent " << find << " @" << fewest_hp_location << " died";
      // Opponent died.
      board_[fewest_hp_location] = '.';
      hit_points_.erase(it);
      return fewest_hp_location;
    }

    hit_points_[fewest_hp_location] -= attack;
    return Point{-1, -1};
  }

  absl::StatusOr<Point> TryMove(Point p) {
    char find = '\0';
    if (board_[p] == 'G')
      find = 'E';
    else if (board_[p] == 'E')
      find = 'G';
    else
      return Error("HP at bad location (move): ", p);

    FindEnemyAdjacent::PointAndDistance p_and_d;
    (void)FindEnemyAdjacent(board_, p, find, &p_and_d).FindMinSteps();
    Point move_to = p;
    if (p_and_d.d == 0) return move_to;
    if (p_and_d.d == std::numeric_limits<int>::max()) return move_to;

    VLOG(1) << "Nearest enemy from " << board_[p] << " @" << p << " is "
            << p_and_d.d << " away adjacent-to-" << p_and_d.p;
    int min_path_length = std::numeric_limits<int>::max();
    for (Point dir : kOrderedDirs) {
      Point check = p + dir;
      if (!board_.OnBoard(check) || board_[check] != '.') continue;
      absl::optional<int> dist =
          PathWalk(board_, check, p_and_d.p).FindMinSteps();
      if (dist && *dist < min_path_length) {
        min_path_length = *dist;
        move_to = check;
      }
    }
    if (min_path_length != p_and_d.d - 1) {
      return Error("Distance integrity check: ", min_path_length,
                   " != ", p_and_d.d - 1);
    }
    if (move_to == p) {
      return Error("Can't find path from ", p, " to ", p_and_d.p);
    }
    VLOG(1) << "Moving " << board_[p] << " from " << p << " to " << move_to;
    board_[move_to] = board_[p];
    board_[p] = '.';
    hit_points_[move_to] = hit_points_[p];
    hit_points_.erase(p);

    return move_to;
  }

  absl::StatusOr<bool> RunStep() {
    int done = false;

    std::vector<Point> actors;
    for (const auto& [p, _] : hit_points_) actors.push_back(p);
    absl::flat_hash_set<Point> dead;
    for (Point p : actors) {
      if (dead.contains(p)) continue;
      if (hit_points_.find(p) == hit_points_.end()) {
        return Error("Integrity check (actor)");
      }
      if (!EnemyExists()) {
        done = true;
        break;
      }
      absl::StatusOr<Point> moved = TryMove(p);
      if (!moved.ok()) return moved.status();
      absl::StatusOr<Point> died_at = TryAttack(*moved);
      if (!died_at.ok()) return died_at.status();
      if (*died_at != Point{-1, -1}) {
        dead.insert(*died_at);
      }
    }

    if (!done) ++rounds_;
    return done;
  }

  bool EnemyExists() {
    absl::flat_hash_set<char> types;
    for (const auto& [p, _] : hit_points_) {
      types.insert(board_[p]);
    }
    return types.size() == 2;
  }

  std::string DebugString() {
    return absl::StrCat(
        absl::StrJoin(hit_points_, "",
                      [](std::string* out, const std::pair<Point, int>& pair) {
                        absl::StrAppend(out, "  ", pair.second, " @",
                                        pair.first, "\n");
                      }),
        board_.AsString());
  }

 private:
  CharBoard board_;
  int rounds_ = 0;
  std::map<Point, int, PointLT> hit_points_;
  int elf_attack_;
};

}  // namespace

absl::StatusOr<std::string> Day_2018_15::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  GameBoard game(std::move(*b));
  bool done = false;
  while (!done) {
    VLOG(1) << "State: [" << game.rounds() << "]";
    VLOG(2) << "\n" << game.DebugString();
    absl::StatusOr<bool> game_ended = game.RunStep();
    if (!game_ended.ok()) return game_ended.status();
    done = *game_ended;
  }

  VLOG(1) << "State: [" << game.rounds() << "]\n" << game.DebugString();

  return IntReturn(game.TotalHitPoints() * game.rounds());
}

absl::StatusOr<std::string> Day_2018_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  for (int elf_attack = 3; true; ++elf_attack) {
    // TODO(@monkeynova): Binary search.
    GameBoard game(std::move(*b), elf_attack);
    int start_elves = game.CountElves();
    bool done = false;
    while (!done) {
      VLOG(1) << "State: [" << game.rounds() << "]";
      VLOG(2) << "\n" << game.DebugString();
      absl::StatusOr<bool> game_ended = game.RunStep();
      if (!game_ended.ok()) return game_ended.status();
      done = *game_ended;
    }
    int end_elves = game.CountElves();
    VLOG(1) << "elf_attack: " << elf_attack << ": " << start_elves << " => "
            << end_elves;
    if (start_elves == end_elves) {
      VLOG(1) << "State: [" << game.rounds() << "]\n" << game.DebugString();
      return IntReturn(game.TotalHitPoints() * game.rounds());
    }
  }

  return Error("Left infinite loop");
}

}  // namespace advent_of_code
