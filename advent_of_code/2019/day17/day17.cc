// http://adventofcode.com/2019/day/17

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
