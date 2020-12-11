#include "advent_of_code/2019/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

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
  LOG(WARNING) << ret.DebugString();
  return ret;
}

class ViewPort : public IntCode::IOModule {
 public:
  bool PauseIntCode() override { return false; }

  std::string DebugBoard() const { return absl::StrJoin(board_, "\n"); }

  int64_t dust_collected() const { return dust_collected_; }

  bool OnBoard(Point check, const std::vector<std::string>& board) {
    return check.y >= 0 && check.x >= 0 && check.y < board.size() &&
           check.x < board[0].size();
  }

  bool IsIntersection(Point p, const std::vector<std::string>& board) {
    if (!OnBoard(p, board)) return false;
    if (board_[p.y][p.x] != '#') return false;
    if (!OnBoard(Point{p.x, p.y - 1}, board) || board_[p.y - 1][p.x] != '#')
      return false;
    if (!OnBoard(Point{p.x, p.y + 1}, board) || board_[p.y + 1][p.x] != '#')
      return false;
    if (!OnBoard(Point{p.x - 1, p.y}, board) || board_[p.y][p.x - 1] != '#')
      return false;
    if (!OnBoard(Point{p.x + 1, p.y}, board) || board_[p.y][p.x + 1] != '#')
      return false;
    return true;
  }

  absl::StatusOr<int> ComputeAlignment() {
    LOG(WARNING) << "\n" << DebugBoard();
    int ret = 0;
    for (int i = 1; i < board_.size() - 1; ++i) {
      for (int j = 1; j < board_[i].size() - 1; ++j) {
        if (IsIntersection(Point{j, i}, board_)) {
          ret += i * j;
        }
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
    for (int y = 0; y < board_.size(); ++y) {
      for (int x = 0; x < board_[y].size(); ++x) {
        if (board_[y][x] != '#' && board_[y][x] != '.') {
          robot.x = x;
          robot.y = y;
        }
        if (IsIntersection(Point{x, y}, board_)) {
          intersections.insert(Point{x, y});
        }
      }
    }
    std::string command;
    std::vector<std::string> scratch = board_;
    Point robot_dir;
    switch (board_[robot.y][robot.x]) {
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
        if (OnBoard(check, scratch) && scratch[check.y][check.x] == '#') {
          if (next_dir != Cardinal::kOrigin)
            return absl::InvalidArgumentError("Not Supported");
          next_dir = d;
        }
      }
      // No direction to go.
      if (next_dir == Point{}) break;

      LOG(WARNING) << robot_dir << " <O> " << next_dir;
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
      for (Point check = robot + robot_dir; OnBoard(check, scratch);
           check += robot_dir) {
        if (scratch[check.y][check.x] != '#') break;
        robot = check;
        ++i;
        if (intersections.contains(check)) {
          intersections.erase(check);
        } else {
          scratch[check.y][check.x] = '_';
        }
      }
      command.append(absl::StrCat(i, ","));
    }
    command.resize(command.size() - 1);

    LOG(WARNING) << "\n" << DebugBoard();
    LOG(WARNING) << "Robot@" << robot;
    LOG(WARNING) << "Full Path: " << command;
    program_ = FindProgram(command);
    LOG(WARNING) << "Program: " << program_.DebugString();
    return absl::OkStatus();
  }

  absl::Status Put(int64_t val) override {
    if (val > 128) {
      LOG(WARNING) << "Dust collected: " << val;
      dust_collected_ = val;
      return absl::OkStatus();
    }
    if (val > 127 || val < 0)
      return absl::InvalidArgumentError("Bad ascii value");
    if (val == '\n') {
      LOG(WARNING) << current_input_;
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
          board_.push_back(std::move(current_input_));
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
  std::vector<std::string> board_;
  int64_t dust_collected_ = -1;
};

absl::StatusOr<std::vector<std::string>> Day17_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  ViewPort view_port;
  if (absl::Status st = codes->Run(&view_port); !st.ok()) {
    return st;
  }

  return IntReturn(view_port.ComputeAlignment());
}

absl::StatusOr<std::vector<std::string>> Day17_2019::Part2(
    const std::vector<absl::string_view>& input) const {
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
