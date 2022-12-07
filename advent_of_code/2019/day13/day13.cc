// https://adventofcode.com/2019/day/13
//
// --- Day 13: Care Package ---
// ----------------------------
// 
// As you ponder the solitude of space and the ever-increasing three-hour
// roundtrip for messages between you and Earth, you notice that the
// Space Mail Indicator Light is blinking. To help keep you sane, the
// Elves have sent you a care package.
// 
// It's a new game for the ship's arcade cabinet! Unfortunately, the
// arcade is all the way on the other end of the ship. Surely, it won't
// be hard to build your own - the care package even comes with
// schematics.
// 
// The arcade cabinet runs Intcode software like the game the Elves sent
// (your puzzle input). It has a primitive screen capable of drawing
// square tiles on a grid. The software draws tiles to the screen with
// output instructions: every three output instructions specify the x
// position (distance from the left), y position (distance from the top),
// and tile id. The tile id is interpreted as follows:
// 
// * 0 is an empty tile. No game object appears in this tile.
// 
// * 1 is a wall tile. Walls are indestructible barriers.
// 
// * 2 is a block tile. Blocks can be broken by the ball.
// 
// * 3 is a horizontal paddle tile. The paddle is indestructible.
// 
// * 4 is a ball tile. The ball moves diagonally and bounces off
// objects.
// 
// For example, a sequence of output values like 1,2,3,6,5,4 would draw a
// horizontal paddle tile (1 tile from the left and 2 tiles from the top)
// and a ball tile (6 tiles from the left and 5 tiles from the top).
// 
// Start the game. How many block tiles are on the screen when the game
// exits?
//
// --- Part Two ---
// ----------------
// 
// The game didn't run because you didn't put in any quarters.
// Unfortunately, you did not bring any quarters. Memory address 0
// represents the number of quarters that have been inserted; set it to 2
// to play for free.
// 
// The arcade cabinet has a joystick that can move left and right. The
// software reads the position of the joystick with input instructions:
// 
// * If the joystick is in the neutral position, provide 0.
// 
// * If the joystick is tilted to the left, provide -1.
// 
// * If the joystick is tilted to the right, provide 1.
// 
// The arcade cabinet also has a segment display capable of showing a
// single number that represents the player's current score. When three
// output instructions specify X=-1, Y=0, the third output instruction is
// not a tile; the value instead specifies the new score to show in the
// segment display. For example, a sequence of output values like
// -1,0,12345 would show 12345 as the player's current score.
// 
// Beat the game by breaking all the blocks. What is your score after the
// last block is broken?


#include "advent_of_code/2019/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"

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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  DrawBoard draw_board;
  if (absl::Status st = codes->Run(&draw_board); !st.ok()) return st;

  return IntReturn(draw_board.CountBlockTiles());
}

absl::StatusOr<std::string> Day_2019_13::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  if (absl::Status st = codes->Poke(0, 2); !st.ok()) return st;
  DrawBoard draw_board;
  if (absl::Status st = codes->Run(&draw_board); !st.ok()) return st;

  return IntReturn(draw_board.score());
}

}  // namespace advent_of_code
