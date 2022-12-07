// https://adventofcode.com/2019/day/15
//
// --- Day 15: Oxygen System ---
// -----------------------------
//
// Out here in deep space, many things can go wrong. Fortunately, many of
// those things have indicator lights. Unfortunately, one of those lights
// is lit: the oxygen system for part of the ship has failed!
//
// According to the readouts, the oxygen system must have failed days ago
// after a rupture in oxygen tank two; that section of the ship was
// automatically sealed once oxygen levels went dangerously low. A single
// remotely-operated repair droid is your only option for fixing the
// oxygen system.
//
// The Elves' care package included an Intcode program (your puzzle
// input) that you can use to remotely control the repair droid. By
// running that program, you can direct the repair droid to the oxygen
// system and fix the problem.
//
// The remote control program executes the following steps in a loop
// forever:
//
// * Accept a movement command via an input instruction.
//
// * Send the movement command to the repair droid.
//
// * Wait for the repair droid to finish the movement operation.
//
// * Report on the status of the repair droid via an output
// instruction.
//
// Only four movement commands are understood: north (1), south (2), west
// (3), and east (4). Any other command is invalid. The movements differ
// in direction, but not in distance: in a long enough east-west hallway,
// a series of commands like 4,4,4,4,3,3,3,3 would leave the repair droid
// back where it started.
//
// The repair droid can reply with any of the following status codes:
//
// * 0: The repair droid hit a wall. Its position has not changed.
//
// * 1: The repair droid has moved one step in the requested direction.
//
// * 2: The repair droid has moved one step in the requested direction;
// its new position is the location of the oxygen system.
//
// You don't know anything about the area around the repair droid, but
// you can figure it out by watching the status codes.
//
// For example, we can draw the area using D for the droid, # for walls,
// . for locations the droid can traverse, and empty space for unexplored
// locations. Then, the initial state looks like this:
//
//
//
// D
//
//
//
// To make the droid go north, send it 1. If it replies with 0, you know
// that location is a wall and that the droid didn't move:
//
//
// #
// D
//
//
//
// To move east, send 4; a reply of 1 means the movement was successful:
//
//
// #
// .D
//
//
//
// Then, perhaps attempts to move north (1), south (2), and east (4) are
// all met with replies of 0:
//
//
// ##
// .D#
// #
//
//
// Now, you know the repair droid is in a dead end. Backtrack with 3
// (which you already know will get a reply of 1 because you already know
// that location is open):
//
//
// ##
// D.#
// #
//
//
// Then, perhaps west (3) gets a reply of 0, south (2) gets a reply of 1,
// south again (2) gets a reply of 0, and then west (3) gets a reply of 2:
//
//
// ##
// #..#
// D.#
// #
//
// Now, because of the reply of 2, you know you've found the oxygen
// system! In this example, it was only 2 moves away from the repair
// droid's starting position.
//
// What is the fewest number of movement commands required to move the
// repair droid from its starting position to the location of the oxygen
// system?
//
// --- Part Two ---
// ----------------
//
// You quickly repair the oxygen system; oxygen gradually fills the area.
//
// Oxygen starts in the location containing the repaired oxygen system.
// It takes one minute for oxygen to spread to all open locations that
// are adjacent to a location that already contains oxygen. Diagonal
// locations are not adjacent.
//
// In the example above, suppose you've used the droid to explore the
// area fully and have the following map (where locations that currently
// contain oxygen are marked O):
//
// ##
// #..##
// #.#..#
// #.O.#
// ###
//
// Initially, the only location which contains oxygen is the location of
// the repaired oxygen system. However, after one minute, the oxygen
// spreads to all open (.) locations that are adjacent to a location
// containing oxygen:
//
// ##
// #..##
// #.#..#
// #OOO#
// ###
//
// After a total of two minutes, the map looks like this:
//
// ##
// #..##
// #O#O.#
// #OOO#
// ###
//
// After a total of three minutes:
//
// ##
// #O.##
// #O#OO#
// #OOO#
// ###
//
// And finally, the whole region is full of oxygen after a total of four
// minutes:
//
// ##
// #OO##
// #O#OO#
// #OOO#
// ###
//
// So, in this example, all locations contain oxygen after 4 minutes.
//
// Use the repair droid to get a complete map of the area. How many
// minutes will it take to fill with oxygen?

#include "advent_of_code/2019/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/random/random.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {
namespace {

class Droid : public IntCode::IOModule {
 public:
  Droid() {
    SetBoard(pos_, 1);
    dir_ = Cardinal::kNorth;
  }

  enum class InputDirection {
    kNorthInput = 1,
    kSouthInput = 2,
    kWestInput = 3,
    kEastInput = 4,
  };

  bool PauseIntCode() override { return done_; }

  absl::StatusOr<int64_t> Fetch() override {
    absl::StatusOr<InputDirection> direction = PickDirection();
    if (!direction.ok()) return direction.status();
    dir_ = kInputDirectionMap[static_cast<int>(*direction)];
    return static_cast<int64_t>(*direction);
  }

  absl::StatusOr<InputDirection> PickDirection() {
    if (current_path_.empty()) {
      if (absl::Status st = ComputePathToNearestUnknown(); !st.ok()) return st;
    }
    InputDirection dir = current_path_.front();
    current_path_.pop_front();
    return dir;
  }

  absl::Status ComputePathToNearestUnknown() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const absl::flat_hash_map<Point, int>& board, Point start,
               std::deque<InputDirection>* out_directions)
          : board_(board), cur_(start), out_directions_(out_directions) {}

      Point identifier() const override { return cur_; }

      bool IsFinal() const override {
        if (board_.contains(cur_)) return false;
        out_directions_->clear();
        out_directions_->insert(out_directions_->end(), directions_.begin(),
                                directions_.end());
        return true;
      }

      void AddNextSteps(State* state) const override {
        for (InputDirection dir :
             {InputDirection::kNorthInput, InputDirection::kSouthInput,
              InputDirection::kWestInput, InputDirection::kEastInput}) {
          Point new_location = cur_ + kInputDirectionMap[static_cast<int>(dir)];
          if (auto it = board_.find(new_location);
              it != board_.end() && it->second == 0) {
            // Wall.
            continue;
          }
          PathWalk next = *this;
          next.cur_ = new_location;
          next.directions_.push_back(dir);
          state->AddNextStep(next);
        }
      }

     private:
      const absl::flat_hash_map<Point, int>& board_;
      Point cur_;
      std::vector<InputDirection> directions_;
      std::deque<InputDirection>* out_directions_;
    };

    absl::optional<int> dist =
        PathWalk(board_, pos_, &current_path_).FindMinSteps();
    if (!dist) {
      // Fully explored the map.
      done_ = true;
      current_path_ = {InputDirection::kNorthInput};
    }
    return absl::OkStatus();
  }

  absl::optional<int> DistanceToO2() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const absl::flat_hash_map<Point, int>& board, Point start,
               Point end)
          : board_(board), cur_(start), end_(end) {}

      Point identifier() const override { return cur_; }

      bool IsFinal() const override { return cur_ == end_; }

      void AddNextSteps(State* state) const override {
        for (Point dir : Cardinal::kFourDirs) {
          Point next_cur = cur_ + dir;
          auto it = board_.find(next_cur);
          CHECK(it != board_.end());
          if (it->second == 0) continue;
          PathWalk next = *this;
          next.cur_ = next_cur;
          state->AddNextStep(next);
        }
      }

     private:
      const absl::flat_hash_map<Point, int>& board_;
      Point cur_;
      Point end_;
    };
    return PathWalk(board_, Cardinal::kOrigin, o2_pos_).FindMinSteps();
  }

  absl::StatusOr<int> GreatestDistanceFromO2() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const absl::flat_hash_map<Point, int>& board, Point start,
               int* max_dist)
          : board_(board), cur_(start), max_dist_(max_dist) {}

      Point identifier() const override { return cur_; }

      bool IsFinal() const override {
        *max_dist_ = std::max(*max_dist_, num_steps());
        return false;
      }

      void AddNextSteps(State* state) const override {
        for (Point dir : Cardinal::kFourDirs) {
          Point next_cur = cur_ + dir;
          auto it = board_.find(next_cur);
          CHECK(it != board_.end());
          if (it->second == 0) continue;
          PathWalk next = *this;
          next.cur_ = next_cur;
          state->AddNextStep(next);
        }
      }

     private:
      const absl::flat_hash_map<Point, int>& board_;
      Point cur_;
      int* max_dist_;
    };

    int max_dist = -1;
    absl::optional<int> should_be_empty =
        PathWalk(board_, o2_pos_, &max_dist).FindMinSteps();
    if (should_be_empty) {
      return Error("Internal error: GreatestDistanceFromO2 (found path)");
    }
    return max_dist;
  }

  enum class OutputStatus {
    kHitWall = 0,
    kMovedOneStep = 1,
    kMovedOneStepFoundO2 = 2,
  };

  void SetBoard(Point p, int val) {
    board_.emplace(p, val);
    if (val == 2) {
      o2_pos_ = p;
    }
    range_.ExpandInclude(p);
  }

  absl::Status Put(int64_t val) override {
    Point next = pos_ + dir_;
    auto it = board_.find(next);
    if (it != board_.end()) {
      if (it->second != val) {
        return absl::InternalError("Board changed");
      }
      // All good.
    } else {
      SetBoard(next, val);
    }
    if (val != 0) {
      pos_ += dir_;
    }
    VLOG(2) << "Board:\n" << DebugBoard();
    return absl::OkStatus();
  }

  std::string DebugBoard() const {
    char render[] = {'#', ' ', '*'};
    CharBoard board(range_.max.x - range_.min.x + 1,
                    range_.max.y - range_.min.y + 1);
    for (Point p : range_) {
      char c;
      if (p == Cardinal::kOrigin) {
        c = '+';
      } else if (p == pos_) {
        c = 'R';
      } else {
        auto it = board_.find(p);
        c = it == board_.end() ? '?' : render[it->second];
      }
      board[p - range_.min] = c;
    }
    return board.AsString();
  }

  Point min() const { return range_.min; }
  Point max() const { return range_.max; }

 private:
  absl::BitGen bitgen_;
  std::deque<InputDirection> current_path_;
  Point pos_ = Cardinal::kOrigin;
  Point dir_;
  Point o2_pos_;
  bool done_ = false;

  static constexpr Point kInputDirectionMap[] = {
      Cardinal::kOrigin, Cardinal::kNorth, Cardinal::kSouth, Cardinal::kWest,
      Cardinal::kEast};

  PointRectangle range_ = PointRectangle::Null();
  absl::flat_hash_map<Point, int> board_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_15::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Droid droid;
  if (absl::Status st = codes->Run(&droid); !st.ok()) {
    return st;
  }
  VLOG(1) << droid.min() << "-" << droid.max() << "\n" << droid.DebugBoard();

  return IntReturn(droid.DistanceToO2());
}

absl::StatusOr<std::string> Day_2019_15::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Droid droid;
  if (absl::Status st = codes->Run(&droid); !st.ok()) {
    return st;
  }
  VLOG(1) << droid.min() << "-" << droid.max() << "\n" << droid.DebugBoard();

  return IntReturn(droid.GreatestDistanceFromO2());
}

}  // namespace advent_of_code
