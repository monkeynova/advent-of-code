// https://adventofcode.com/2016/day/24
//
// --- Day 24: Air Duct Spelunking ---
// -----------------------------------
// 
// You've finally met your match; the doors that provide access to the
// roof are locked tight, and all of the controls and related electronics
// are inaccessible. You simply can't reach them.
// 
// The robot that cleans the air ducts, however, can.
// 
// It's not a very fast little robot, but you reconfigure it to be able
// to interface with some of the exposed wires that have been routed
// through the HVAC system. If you can direct it to each of those
// locations, you should be able to bypass the security controls.
// 
// You extract the duct layout for this area from some blueprints you
// acquired and create a map with the relevant locations marked (your
// puzzle input). 0 is your current location, from which the cleaning
// robot embarks; the other numbers are (in no particular order) the
// locations the robot needs to visit at least once each. Walls are
// marked as #, and open passages are marked as .. Numbers behave like
// open passages.
// 
// For example, suppose you have a map like the following:
// 
// ###########
// #0.1.....2#
// #.#######.#
// #4.......3#
// ###########
// 
// To reach all of the points of interest as quickly as possible, you
// would have the robot take the following path:
// 
// * 0 to 4 (2 steps)
// 
// * 4 to 1 (4 steps; it can't move diagonally)
// 
// * 1 to 2 (6 steps)
// 
// * 2 to 3 (2 steps)
// 
// Since the robot isn't very fast, you need to find it the shortest
// route. This path is the fewest steps (in the above example, a total of
// 14) required to start at 0 and then visit every other location at
// least once.
// 
// Given your actual map, and starting from location 0, what is the
// fewest number of steps required to visit every non-0 number marked on
// the map at least once?
//
// --- Part Two ---
// ----------------
// 
// Of course, if you leave the cleaning robot somewhere weird, someone is
// bound to notice.
// 
// What is the fewest number of steps required to start at 0, visit every
// non-0 number marked on the map at least once, and then return to 0?


#include "advent_of_code/2016/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class PathWalk : public BFSInterface<PathWalk> {
 public:
  PathWalk(const CharBoard& board, bool back_to_origin)
      : board_(board),
        start_(FindStart(board)),
        cur_(start_),
        need_keys_(NeedKeys(board)),
        have_keys_(0),
        back_to_origin_(back_to_origin) {}

  void AddNextSteps(State* state) const final {
    for (Point dir : Cardinal::kFourDirs) {
      Point next_point = cur_ + dir;
      if (board_.OnBoard(next_point) && board_[next_point] != '#') {
        PathWalk next = *this;
        next.cur_ = next_point;
        if (board_[next_point] > '0' && board_[next_point] <= '9') {
          next.have_keys_ |= 1 << (board_[next_point] - '1');
        }
        state->AddNextStep(std::move(next));
      }
    }
  }

  bool IsFinal() const final {
    if (need_keys_ != have_keys_) return false;
    if (back_to_origin_ && cur_ != start_) return false;
    return true;
  }

  const PathWalk& identifier() const override { return *this; }

  bool operator==(const PathWalk& o) const {
    return cur_ == o.cur_ && have_keys_ == o.have_keys_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const PathWalk& pw) {
    return H::combine(std::move(h), pw.cur_, pw.have_keys_);
  }

  friend std::ostream& operator<<(std::ostream& o, const PathWalk& pw) {
    return o << "{" << pw.cur_ << "," << std::hex << pw.have_keys_ << std::dec
             << "}";
  }

 private:
  Point FindStart(const CharBoard& b) {
    Point start;
    for (Point p : b.range()) {
      if (b[p] == '0') start = p;
    }
    return start;
  }

  int NeedKeys(const CharBoard& b) {
    int need_keys = 0;
    for (Point p : b.range()) {
      if (b[p] > '0' && b[p] <= '9') {
        need_keys |= 1 << (b[p] - '1');
      }
    }
    return need_keys;
  }

  const CharBoard& board_;
  Point start_;
  Point cur_;
  int need_keys_;
  int have_keys_;
  bool back_to_origin_;
};

}  // namespace

absl::StatusOr<std::string> Day_2016_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  return IntReturn(PathWalk(*b, false).FindMinSteps());
}

absl::StatusOr<std::string> Day_2016_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  return IntReturn(PathWalk(*b, true).FindMinSteps());
}

}  // namespace advent_of_code
