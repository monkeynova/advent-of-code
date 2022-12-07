// https://adventofcode.com/2019/day/17
//
// --- Day 17: Set and Forget ---
// ------------------------------
// 
// An early warning system detects an incoming solar flare and
// automatically activates the ship's electromagnetic shield.
// Unfortunately, this has cut off the Wi-Fi for many small robots that,
// unaware of the impending danger, are now trapped on exterior
// scaffolding on the unsafe side of the shield. To rescue them, you'll
// have to act quickly!
// 
// The only tools at your disposal are some wired cameras and a small
// vacuum robot currently asleep at its charging station. The video
// quality is poor, but the vacuum robot has a needlessly bright LED that
// makes it easy to spot no matter where it is.
// 
// An Intcode program, the Aft Scaffolding Control and Information
// Interface (ASCII, your puzzle input), provides access to the cameras
// and the vacuum robot. Currently, because the vacuum robot is asleep,
// you can only access the cameras.
// 
// Running the ASCII program on your Intcode computer will provide the
// current view of the scaffolds. This is output, purely coincidentally,
// as ASCII code: 35 means #, 46 means ., 10 starts a new line of output
// below the current one, and so on. (Within a line, characters are drawn
// left-to-right.)
// 
// In the camera output, # represents a scaffold and . represents open
// space. The vacuum robot is visible as ^, v, <, or > depending on
// whether it is facing up, down, left, or right respectively. When drawn
// like this, the vacuum robot is always on a scaffold; if the vacuum
// robot ever walks off of a scaffold and begins tumbling through space
// uncontrollably, it will instead be visible as X.
// 
// In general, the scaffold forms a path, but it sometimes loops back
// onto itself. For example, suppose you can see the following view from
// the cameras:
// 
// ..#..........
// ..#..........
// #######...###
// #.#...#...#.#
// #############
// ..#...#...#..
// ..#####...^..
// 
// Here, the vacuum robot, ^ is facing up and sitting at one end of the
// scaffold near the bottom-right of the image. The scaffold continues
// up, loops across itself several times, and ends at the top-left of the
// image.
// 
// The first step is to calibrate the cameras by getting the alignment
// parameters of some well-defined points. Locate all scaffold
// intersections; for each, its alignment parameter is the distance
// between its left edge and the left edge of the view multiplied by the
// distance between its top edge and the top edge of the view. Here, the
// intersections from the above image are marked O:
// 
// ..#..........
// ..#..........
// ##O####...###
// #.#...#...#.#
// ##O###O###O##
// ..#...#...#..
// ..#####...^..
// 
// For these intersections:
// 
// * The top-left intersection is 2 units from the left of the image
// and 2 units from the top of the image, so its alignment parameter
// is 2 * 2 = 4.
// 
// * The bottom-left intersection is 2 units from the left and 4 units
// from the top, so its alignment parameter is 2 * 4 = 8.
// 
// * The bottom-middle intersection is 6 from the left and 4 from the
// top, so its alignment parameter is 24.
// 
// * The bottom-right intersection's alignment parameter is 40.
// 
// To calibrate the cameras, you need the sum of the alignment parameters.
// In the above example, this is 76.
// 
// Run your ASCII program. What is the sum of the alignment parameters
// for the scaffold intersections?
//
// --- Part Two ---
// ----------------
// 
// Now for the tricky part: notifying all the other robots about the
// solar flare. The vacuum robot can do this automatically if it gets
// into range of a robot. However, you can't see the other robots on the
// camera, so you need to be thorough instead: you need to make the
// vacuum robot visit every part of the scaffold at least once.
// 
// The vacuum robot normally wanders randomly, but there isn't time for
// that today. Instead, you can override its movement logic with new
// rules.
// 
// Force the vacuum robot to wake up by changing the value in your ASCII
// program at address 0 from 1 to 2. When you do this, you will be
// automatically prompted for the new movement rules that the vacuum
// robot should use. The ASCII program will use input instructions to
// receive them, but they need to be provided as ASCII code; end each
// line of logic with a single newline, ASCII code 10.
// 
// First, you will be prompted for the main movement routine. The main
// routine may only call the movement functions: A, B, or C. Supply the
// movement functions to use as ASCII text, separating them with commas (,,
// ASCII code 44), and ending the list with a newline (ASCII code 10).
// For example, to call A twice, then alternate between B and C three
// times, provide the string A,A,B,C,B,C,B,C and then a newline.
// 
// Then, you will be prompted for each movement function. Movement
// functions may use L to turn left, R to turn right, or a number to move
// forward that many units. Movement functions may not call other
// movement functions. Again, separate the actions with commas and end
// the list with a newline. For example, to move forward 10 units, turn
// left, move forward 8 units, turn right, and finally move forward 6
// units, provide the string 10,L,8,R,6 and then a newline.
// 
// Finally, you will be asked whether you want to see a continuous video
// feed; provide either y or n and a newline. Enabling the continuous
// video feed can help you see what's going on, but it also requires a
// significant amount of processing power, and may even cause your
// Intcode computer to overheat.
// 
// Due to the limited amount of memory in the vacuum robot, the ASCII
// definitions of the main routine and the movement functions may each
// contain at most 20 characters, not counting the newline.
// 
// For example, consider the following camera feed:
// 
// #######...#####
// #.....#...#...#
// #.....#...#...#
// ......#...#...#
// ......#...###.#
// ......#.....#.#
// ^########...#.#
// ......#.#...#.#
// ......#########
// ........#...#..
// ....#########..
// ....#...#......
// ....#...#......
// ....#...#......
// ....#####......
// 
// In order for the vacuum robot to visit every part of the scaffold at
// least once, one path it could take is:
// 
// R,8,R,8,R,4,R,4,R,8,L,6,L,2,R,4,R,4,R,8,R,8,R,8,L,6,L,2
// 
// Without the memory limit, you could just supply this whole string to
// function A and have the main routine call A once. However, you'll need
// to split it into smaller parts.
// 
// One approach is:
// 
// * Main routine: A,B,C,B,A,C
// (ASCII input: 65, 44, 66, 44, 67, 44, 66, 44, 65, 44, 67, 10)
// 
// * Function A: R,8,R,8
// (ASCII input: 82, 44, 56, 44, 82, 44, 56, 10)
// 
// * Function B: R,4,R,4,R,8
// (ASCII input: 82, 44, 52, 44, 82, 44, 52, 44, 82, 44, 56, 10)
// 
// * Function C: L,6,L,2
// (ASCII input: 76, 44, 54, 44, 76, 44, 50, 10)
// 
// Visually, this would break the desired path into the following parts:
// 
// A,        B,            C,        B,            A,        C
// R,8,R,8,  R,4,R,4,R,8,  L,6,L,2,  R,4,R,4,R,8,  R,8,R,8,  L,6,L,2
// 
// CCCCCCA...BBBBB
// C.....A...B...B
// C.....A...B...B
// ......A...B...B
// ......A...CCC.B
// ......A.....C.B
// ^AAAAAAAA...C.B
// ......A.A...C.B
// ......AAAAAA#AB
// ........A...C..
// ....BBBB#BBBB..
// ....B...A......
// ....B...A......
// ....B...A......
// ....BBBBA......
// 
// Of course, the scaffolding outside your ship is much more complex.
// 
// As the vacuum robot finds other robots and notifies them of the
// impending solar flare, it also can't help but leave them squeaky
// clean, collecting any space dust it finds. Once it finishes the
// programmed set of movements, assuming it hasn't drifted off into
// space, the cleaning robot will return to its docking station and
// report the amount of space dust it collected as a large, non-ASCII
// value in a single output instruction.
// 
// After visiting every part of the scaffold at least once, how much dust
// does the vacuum robot report it has collected?


#include "advent_of_code/2019/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Program {
  std::string main;
  std::string a;
  std::string b;
  std::string c;
  std::string video_feed = "n\n";

  std::string DebugString() {
    std::string ret;
    absl::StrAppend(&ret, "main(", main.size(), "):", main, "\n");
    absl::StrAppend(&ret, "a(", a.size(), "):", a, "\n");
    absl::StrAppend(&ret, "b(", b.size(), "):", b, "\n");
    absl::StrAppend(&ret, "c(", c.size(), "):", c, "\n");
    return ret;
  }
};

bool ValidProgram(Program* program) {
  if (!RE2::FullMatch(program->main, "[ABC,]+")) return false;
  if (program->main.size() > 20) return false;
  if (program->a.size() > 20) return false;
  if (program->b.size() > 20) return false;
  if (program->c.size() > 20) return false;
  return true;
}

bool FindC(Program* program) {
  int start_idx = 0;
  for (; start_idx < program->main.size(); ++start_idx) {
    if (program->main[start_idx] != 'A' && program->main[start_idx] != 'B' &&
        program->main[start_idx] != ',') {
      break;
    }
  }
  for (int i = start_idx; i < program->main.size(); ++i) {
    if (program->main[i] == ',') {
      Program save = *program;
      program->c = program->main.substr(start_idx, i - start_idx);
      RE2::GlobalReplace(&program->main, program->c, "C");
      if (ValidProgram(program)) return true;
      *program = save;
    }
  }
  Program save = *program;
  program->c =
      program->main.substr(start_idx, program->main.size() - start_idx);
  RE2::GlobalReplace(&program->main, program->c, "C");
  if (ValidProgram(program)) return true;
  *program = save;

  return false;
}

bool FindB(Program* program) {
  int start_idx = 0;
  for (; start_idx < program->main.size(); ++start_idx) {
    if (program->main[start_idx] != 'A' && program->main[start_idx] != ',') {
      break;
    }
  }
  for (int i = start_idx; i < program->main.size(); ++i) {
    if (program->main[i] == ',') {
      Program save = *program;
      program->b = program->main.substr(start_idx, i - start_idx);
      RE2::GlobalReplace(&program->main, program->b, "B");
      if (FindC(program)) return true;
      *program = save;
    }
  }

  return false;
}

bool FindA(Program* program) {
  int start_idx = 0;
  for (int i = start_idx; i < program->main.size(); ++i) {
    if (program->main[i] == ',') {
      Program save = *program;
      program->a = program->main.substr(start_idx, i - start_idx);
      RE2::GlobalReplace(&program->main, program->a, "A");
      if (FindB(program)) return true;
      *program = save;
    }
  }

  return false;
}

Program FindProgram(absl::string_view command) {
  Program ret;
  ret.main = command;
  if (FindA(&ret)) {
    ret.main.append("\n");
    ret.a.append("\n");
    ret.b.append("\n");
    ret.c.append("\n");
  }
  VLOG(1) << ret.DebugString();
  return ret;
}

class ViewPort : public IntCode::IOModule {
 public:
  bool PauseIntCode() override { return false; }

  int64_t dust_collected() const { return dust_collected_; }

  bool IsIntersection(Point p, const CharBoard& board) {
    if (!board.OnBoard(p)) return false;
    if (board[p] != '#') return false;
    for (Point d : Cardinal::kFourDirs) {
      Point n = p + d;
      if (!board.OnBoard(n) || board[n] != '#') return false;
    }
    return true;
  }

  absl::StatusOr<int> ComputeAlignment() {
    VLOG(1) << "\n" << board_;
    int ret = 0;
    for (Point p : board_.range()) {
      if (IsIntersection(p, board_)) {
        ret += p.x * p.y;
      }
    }
    return ret;
  }

  absl::StatusOr<int64_t> Fetch() override {
    if (current_output_pos_ >= current_output_.size()) {
      return absl::InvalidArgumentError("No input");
    }
    return current_output_[current_output_pos_++];
  }

  absl::Status ComputeProgram() {
    Point robot;
    absl::flat_hash_set<Point> intersections;
    for (Point p : board_.range()) {
      if (board_[p] != '#' && board_[p] != '.') {
        robot = p;
      }
      if (IsIntersection(p, board_)) {
        intersections.insert(p);
      }
    }
    std::string command;
    CharBoard scratch = board_;
    Point robot_dir;
    switch (board_[robot]) {
      case '^':
        robot_dir = Cardinal::kNorth;
        break;
      case 'v':
        robot_dir = Cardinal::kSouth;
        break;
      case '<':
        robot_dir = Cardinal::kWest;
        break;
      case '>':
        robot_dir = Cardinal::kEast;
        break;
      default:
        return absl::InvalidArgumentError("Bad robot direction");
    }
    while (true) {
      Point next_dir = Cardinal::kOrigin;
      for (Point d : Cardinal::kFourDirs) {
        Point check = robot + d;
        if (scratch.OnBoard(check) && scratch[check] == '#') {
          if (next_dir != Cardinal::kOrigin)
            return absl::InvalidArgumentError("Not Supported");
          next_dir = d;
        }
      }
      // No direction to go.
      if (next_dir == Point{}) break;

      VLOG(2) << robot_dir << " <O> " << next_dir;
      if (robot_dir == Cardinal::kNorth) {
        if (next_dir == Cardinal::kWest)
          command.append("L,");
        else if (next_dir == Cardinal::kEast)
          command.append("R,");
        else
          return absl::InvalidArgumentError("Can't turn");
      }
      if (robot_dir == Cardinal::kSouth) {
        if (next_dir == Cardinal::kWest)
          command.append("R,");
        else if (next_dir == Cardinal::kEast)
          command.append("L,");
        else
          return absl::InvalidArgumentError("Can't turn");
      }
      if (robot_dir == Cardinal::kWest) {
        if (next_dir == Cardinal::kNorth)
          command.append("R,");
        else if (next_dir == Cardinal::kSouth)
          command.append("L,");
        else
          return absl::InvalidArgumentError("Can't turn");
      }
      if (robot_dir == Cardinal::kEast) {
        if (next_dir == Cardinal::kNorth)
          command.append("L,");
        else if (next_dir == Cardinal::kSouth)
          command.append("R,");
        else
          return absl::InvalidArgumentError("Can't turn");
      }
      robot_dir = next_dir;

      int i = 0;
      for (Point check = robot + robot_dir; scratch.OnBoard(check);
           check += robot_dir) {
        if (scratch[check] != '#') break;
        robot = check;
        ++i;
        if (intersections.contains(check)) {
          intersections.erase(check);
        } else {
          scratch[check] = '_';
        }
      }
      command.append(absl::StrCat(i, ","));
    }
    command.resize(command.size() - 1);

    VLOG(1) << "\n" << board_;
    VLOG(1) << "Robot@" << robot;
    VLOG(1) << "Full Path: " << command;
    program_ = FindProgram(command);
    VLOG(1) << "Program: " << program_.DebugString();
    return absl::OkStatus();
  }

  absl::Status Put(int64_t val) override {
    if (val > 128) {
      VLOG(2) << "Dust collected: " << val;
      dust_collected_ = val;
      return absl::OkStatus();
    }
    if (val > 127 || val < 0)
      return absl::InvalidArgumentError("Bad ascii value");
    if (val == '\n') {
      VLOG(2) << current_input_;
      if (current_input_ == "Main:") {
        if (absl::Status st = ComputeProgram(); !st.ok()) return st;
        current_output_ = program_.main;
        current_output_pos_ = 0;
      } else if (current_input_ == "Function A:") {
        current_output_ = program_.a;
        current_output_pos_ = 0;
      } else if (current_input_ == "Function B:") {
        current_output_ = program_.b;
        current_output_pos_ = 0;
      } else if (current_input_ == "Function C:") {
        current_output_ = program_.c;
        current_output_pos_ = 0;
      } else if (current_input_ == "Continuous video feed?") {
        current_output_ = program_.video_feed;
        current_output_pos_ = 0;
      } else {
        if (!current_input_.empty()) {
          board_build_.push_back(std::move(current_input_));
          absl::StatusOr<CharBoard> next_board = CharBoard::Parse(board_build_);
          if (!next_board.ok()) return next_board.status();
          board_ = std::move(*next_board);
        }
      }
      current_input_ = "";
    } else {
      current_input_.append(1, static_cast<char>(val));
    }
    return absl::OkStatus();
  }

 private:
  Program program_;
  absl::string_view current_output_;
  int current_output_pos_;

  std::string current_input_;
  std::vector<std::string> board_build_;
  CharBoard board_{0, 0};
  int64_t dust_collected_ = -1;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_17::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  ViewPort view_port;
  if (absl::Status st = codes->Run(&view_port); !st.ok()) {
    return st;
  }

  return IntReturn(view_port.ComputeAlignment());
}

absl::StatusOr<std::string> Day_2019_17::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  if (absl::Status st = codes->Poke(0, 2); !st.ok()) return st;

  ViewPort view_port;
  if (absl::Status st = codes->Run(&view_port, &view_port, &view_port);
      !st.ok()) {
    return st;
  }

  return IntReturn(view_port.dust_collected());
}

}  // namespace advent_of_code
