#include "advent_of_code/2019/day13/day13.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

struct Point {
  int x;
  int y;

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Point& other) const { return !operator==(other); }

  Point operator-(const Point& other) const {
    return {.x = x - other.x, .y = y - other.y};
  }

  Point operator+(const Point& other) const {
    return {.x = x + other.x, .y = y + other.y};
  }

  Point& operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Point min_step() const {
    int gcd = std::gcd(abs(x), abs(y));
    return {.x = x / gcd, .y = y / gcd};
  }
};

template <typename H>
H AbslHashValue(H h, const Point& p) {
  return H::combine(std::move(h), p.x, p.y);
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
  out << "{" << p.x << "," << p.y << "}";
  return out;
}

class DrawBoard : public IntCode::InputSource, public IntCode::OutputSink {
 public:
  absl::StatusOr<int64_t> Fetch() override {
    return absl::InvalidArgumentError("Fetch not supported");
  }

  absl::Status Put(int64_t val) override {
    switch (put_mode) {
      case 0: {
        cur_point.x = val;
        break;
      }
      case 1: {
        cur_point.y = val;
        break;
      }
      case 2: {
        if (board_.contains(cur_point)) {
          return absl::InvalidArgumentError("Overpaint!");
        }
        board_[cur_point] = val;
        break;
      }
      default: return absl::InternalError("Bad put_mode");
    }
    put_mode = (put_mode + 1) % 3;
    return absl::OkStatus();
  }

 int CountBlockTiles() const {
   int block_tiles = 0;
   for (const auto& point_and_tile : board_ ) {
     if (point_and_tile.second == 2) {
       ++block_tiles;
     }
   }
   return block_tiles;
 }

 private:
  int put_mode = 0;
  Point cur_point;

  absl::flat_hash_map<Point, int> board_;
};

absl::StatusOr<std::vector<std::string>> Day13_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  DrawBoard draw_board;
  if (absl::Status st = codes->Run(&draw_board, &draw_board); !st.ok()) return st;

  return std::vector<std::string>{absl::StrCat(draw_board.CountBlockTiles())};
}

absl::StatusOr<std::vector<std::string>> Day13_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
