// http://adventofcode.com/2023/day/17

#include "advent_of_code/2023/day17/day17.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"

namespace advent_of_code {

namespace {

class FastBoard {
 public:
  enum Dir {
    kNorth = 0,
    kSouth = 1,
    kWest = 2,
    kEast = 3,
  };

  static constexpr std::array<Dir, 4> kRotateLeft = {kWest, kEast, kSouth, kNorth};
  static constexpr std::array<Dir, 4> kRotateRight = {kEast, kWest, kNorth, kSouth};

  class Point {
   public:
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;

   private:
    friend class FastBoard;
    template <typename Storage>
    friend class PointMap;

    explicit Point(int idx) : idx_(idx) {}

    int idx_;
  };
  
  template <typename Storage>
  class PointMap {
   public:
    PointMap(const FastBoard& b, Storage init) : map_(b.size_, init) {}

    Storage Get(Point p) const { return map_[p.idx_]; }
    void Set(Point p, Storage s) { map_[p.idx_] = s; }

   private:
    friend class FastBoard;
    std::vector<Storage> map_;
  };

  template <typename Storage>
  class PointDirMap {
   public:
    PointDirMap(const FastBoard& b, Storage init) : map_(4 * b.size_, init) {}

    Storage Get(Point p, Dir d) const { return map_[p.idx_ * 4 + d]; }
    void Set(Point p, Dir d, Storage s) { map_[p.idx_ * 4 + d] = s; }

   private:
    friend class FastBoard;
    std::vector<Storage> map_;
  };

  explicit FastBoard(const ImmutableCharBoard& b)
   : base_(b.row(0).data()), stride_(b.row(1).data() - b.row(0).data()),
     size_(b.height() * stride_), dir_delta_({-stride_, stride_, -1, 1}),
     on_board_(size_, true) {
    for (int i = stride_ - 1; i < size_; i += stride_) {
      on_board_[i] = false;
    }
  }

  Point From(advent_of_code::Point in) const {
    return Point(in.y * stride_ + in.x);
  }

  bool OnBoard(Point p) const {
    if (p.idx_ < 0) return false;
    if (p.idx_ >= size_) return false;
    return on_board_[p.idx_];
  }

  Point Add(Point p, Dir d) const {
    return Point(p.idx_ + dir_delta_[d]);
  }

  char operator[](Point p) const {
    return base_[p.idx_];
  }

 private:
  const char* base_;
  int stride_;
  int size_;
  std::array<int, 4> dir_delta_;
  std::vector<bool> on_board_;
};

int MinCartPath(const ImmutableCharBoard& b, int min, int max) {
  using State = std::pair<FastBoard::Point, FastBoard::Dir>;

  std::deque<State> queue;
  FastBoard fb(b);
  FastBoard::PointDirMap<int> heat_map(fb, std::numeric_limits<int>::max());
  FastBoard::PointDirMap<bool> in_queue(fb, false);

  auto add_range = [&](State s, FastBoard::Dir dir) {
    int heat = heat_map.Get(s.first, s.second);
    CHECK_NE(heat, std::numeric_limits<int>::max());
    CHECK(s.second != dir);
    s.second = dir;
    for (int i = 0; i < max; ++i) {
      s.first = fb.Add(s.first, s.second);
      if (!fb.OnBoard(s.first)) break;
      heat += fb[s.first] - '0';
      if (i + 1 < min) continue;
  
      if (heat < heat_map.Get(s.first, s.second)) {
        heat_map.Set(s.first, s.second, heat);
        if (!in_queue.Get(s.first, s.second)) {
          in_queue.Set(s.first, s.second, true);
          queue.push_back(s); 
        }
      }
    }
  };

  FastBoard::Point start = fb.From({0, 0});
  for (FastBoard::Dir d : {FastBoard::kEast, FastBoard::kSouth}) {
    heat_map.Set(start, FastBoard::kNorth, 0);
    add_range({start, FastBoard::kNorth}, d);
  }
  for (;!queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    in_queue.Set(cur.first, cur.second, false);
    add_range(cur, FastBoard::kRotateLeft[cur.second]);
    add_range(cur, FastBoard::kRotateRight[cur.second]);
  }
  FastBoard::Point end = fb.From({b.width() - 1, b.height() - 1});
  int answer = std::numeric_limits<int>::max();
  answer = std::min(answer, heat_map.Get(end, FastBoard::kEast));
  answer = std::min(answer, heat_map.Get(end, FastBoard::kSouth));
  return answer;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_17::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(MinCartPath(b, 1, 3));
}

absl::StatusOr<std::string> Day_2023_17::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  return AdventReturn(MinCartPath(b, 4, 10));
}

}  // namespace advent_of_code
