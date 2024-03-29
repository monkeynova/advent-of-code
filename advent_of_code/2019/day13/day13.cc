#include "advent_of_code/2019/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/point.h"

namespace advent_of_code {
namespace {

class DrawBoard : public IntCode::IOModule {
 public:
  bool PauseIntCode() override { return false; }

  absl::StatusOr<int64_t> Fetch() override {
    // Returns Joystick position.
    VLOG(2) << "Cursor: " << cursor_position_;
    VLOG(2) << "Ball: " << ball_position_;
    VLOG(2) << "Board:\n" << DebugBoard();
    if (cursor_position_.x > ball_position_.x) {
      return -1;
    } else if (cursor_position_.x < ball_position_.x) {
      return 1;
    }
    return 0;
  }

  absl::Status Put(int64_t val) override {
    switch (put_mode) {
      case 0: {
        cur_point_.x = val;
        break;
      }
      case 1: {
        cur_point_.y = val;
        break;
      }
      case 2: {
        if (cur_point_ == Point{.x = -1, .y = 0}) {
          VLOG(1) << "Score: " << score_;
          score_ = val;
        } else {
          if (val > 4 || val < 0) {
            return absl::InvalidArgumentError("Bad render");
          }
          min_.x = std::min(min_.x, cur_point_.x);
          min_.y = std::min(min_.y, cur_point_.y);
          max_.x = std::max(max_.x, cur_point_.x);
          max_.y = std::max(max_.y, cur_point_.y);
          if (val == 3) {
            cursor_position_ = cur_point_;
          } else if (val == 4) {
            ball_position_ = cur_point_;
          }
          board_[cur_point_] = val;
        }
        break;
      }
      default:
        return absl::InternalError("Bad put_mode");
    }
    put_mode = (put_mode + 1) % 3;
    return absl::OkStatus();
  }

  int score() const { return score_; }

  int CountBlockTiles() const {
    int block_tiles = 0;
    for (const auto& point_and_tile : board_) {
      if (point_and_tile.second == 2) {
        ++block_tiles;
      }
    }
    return block_tiles;
  }

  std::string DebugBoard() const {
    char render[] = {' ', '#', 'x', '=', 'o'};
    std::string ret;
    ret.resize((max_.y - min_.y + 1) * (max_.x - min_.x + 2));
    int mark = 0;
    for (int y = min_.y; y <= max_.y; ++y) {
      for (int x = min_.x; x <= max_.x; ++x) {
        auto it = board_.find(Point{.x = x, .y = y});
        ret[mark++] = it == board_.end() ? '?' : render[it->second];
      }
      ret[mark++] = '\n';
    }
    return ret;
  }

 private:
  int put_mode = 0;
  Point cur_point_;
  Point cursor_position_;
  Point ball_position_;
  int score_ = 0;

  Point min_ = {.x = std::numeric_limits<int>::max(),
                .y = std::numeric_limits<int>::max()};
  Point max_ = {.x = std::numeric_limits<int>::min(),
                .y = std::numeric_limits<int>::min()};
  absl::flat_hash_map<Point, int> board_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_13::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  DrawBoard draw_board;
  RETURN_IF_ERROR(codes.Run(&draw_board));

  return AdventReturn(draw_board.CountBlockTiles());
}

absl::StatusOr<std::string> Day_2019_13::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  RETURN_IF_ERROR(codes.Poke(0, 2));
  DrawBoard draw_board;
  RETURN_IF_ERROR(codes.Run(&draw_board));

  return AdventReturn(draw_board.score());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/13,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_13()); });

}  // namespace advent_of_code
