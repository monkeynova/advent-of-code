#include "advent_of_code/2019/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"
#include "re2/re2.h"

class ViewPort : public IntCode::InputSource, public IntCode::OutputSink, public IntCode::PauseCondition {
 public:
  bool PauseIntCode() override { return false; } 

  std::string DebugBoard() const {
    return absl::StrJoin(board_, "\n");
  }

  absl::StatusOr<int> ComputeAlignment() {
    int ret = 0;
    for (int i = 1; i < board_.size() - 1; ++i) {
      for (int j = 1; j < board_[i].size() - 1; ++j) {
        if (board_[i][j] == '#' && 
            board_[i-1][j] == '#' && board_[i][j-1] == '#' &&
            board_[i][j+1] == '#' && board_[i+1][j] == '#') {
          ret += i * j;
        }
      }
    }
    return ret;
  }

  absl::StatusOr<int64_t> Fetch() override {
    return absl::InvalidArgumentError("No input");
  }

  absl::Status Put(int64_t val) override {
    if (val > 127 || val < 0) return absl::InvalidArgumentError("Bad ascii value");
    if (val == '\n') {
      if (board_.back().size() == 0) {
        rendered_ = true;
        return absl::OkStatus();
      }
      if (board_.size() > 1) {
        int this_line_size = (board_.end() - 1)->size();
        int prev_line_size = (board_.end() - 2)->size();
        if (this_line_size != prev_line_size) {
          return absl::InvalidArgumentError(
            absl::StrCat("Bad board: ", this_line_size, " != ", prev_line_size, ";\n", DebugBoard()));
        }
      }
      board_.push_back("");
    } else {
      board_.back().append(1, static_cast<char>(val));
    }
    return absl::OkStatus();
  }

 private:
  std::vector<std::string> board_{""};
  bool rendered_ = false;
};

absl::StatusOr<std::vector<std::string>> Day17_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  ViewPort view_port;
  if (absl::Status st = codes->Run(&view_port, &view_port, &view_port); !st.ok()) {
    return st;
  }
  LOG(WARNING) << "\n" << view_port.DebugBoard();
  absl::StatusOr<int> alignment = view_port.ComputeAlignment();
  if (!alignment.ok()) return alignment.status();

  return std::vector<std::string>{absl::StrCat(*alignment)};
}

absl::StatusOr<std::vector<std::string>> Day17_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
