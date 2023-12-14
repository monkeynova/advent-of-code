// http://adventofcode.com/2023/day/14

#include "advent_of_code/2023/day14/day14.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

void RollNorth(CharBoard& b) {
  char* raw_access = b.mutable_row(0);
  int stride = b.width() + 1;

  for (int x = 0; x < b.width(); ++x) {
    int dest_idx = x;
    int src_idx = x;
    for (int y = 0; y < b.height(); ++y, src_idx += stride) {
      switch (raw_access[src_idx]) {
        case 'O': {
          if (src_idx != dest_idx) {        
            raw_access[dest_idx] = 'O';
            raw_access[src_idx] = '.';
          }
          dest_idx += stride;
          break;
        }
        case '#': {
          dest_idx = src_idx + stride;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

void RollSouth(CharBoard& b) {
  char* raw_access = b.mutable_row(0);
  int stride = b.width() + 1;

  for (int x = 0; x < b.width(); ++x) {
    int dest_idx = (b.height() - 1) * stride + x;
    int src_idx = dest_idx;
    for (int y = b.height() - 1; y >= 0; --y, src_idx -= stride) {
      switch (raw_access[src_idx]) {
        case 'O': {
          if (src_idx != dest_idx) {        
            raw_access[dest_idx] = 'O';
            raw_access[src_idx] = '.';
          }
          dest_idx -= stride;
          break;
        }
        case '#': {
          dest_idx = src_idx - stride;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

void RollWest(CharBoard& b) {
  for (int y = 0; y < b.height(); ++y) {
    char* row = b.mutable_row(y);
    int dest = 0;
    for (int x = 0; x < b.width(); ++x) {
      switch (row[x]) {
        case 'O': {
          if (dest != x) {        
            row[dest] = 'O';
            row[x] = '.';
          }
          ++dest;
          break;
        }
        case '#': {
          dest = x + 1;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

void RollEast(CharBoard& b) {
  for (int y = 0; y < b.height(); ++y) {
    char* row = b.mutable_row(y);
    int dest = b.width() - 1;
    for (int x = b.width() - 1; x >= 0; --x) {
      switch (row[x]) {
        case 'O': {
          if (dest != x) {        
            row[dest] = 'O';
            row[x] = '.';
          }
          --dest;
          break;
        }
        case '#': {
          dest = x - 1;
          break;
        }
        case '.': {
          break;
        }
      }
    }
  }
}

class Dish {
 public:
  explicit Dish(const CharBoard& b)
   : rollers_(b.FindVec('O')), stops_(b.FindVec('#')),
     width_(b.width()), height_(b.height()) {
    for (int x = 0; x < width_; ++x) {
      stops_.push_back({x, -1});
      stops_.push_back({x, height_});
    }  
    for (int y = 0; y < height_; ++y) {
      stops_.push_back({-1, y});
      stops_.push_back({width_, y});
    }  
  }

  int NorthLoad() const { return NorthLoad(rollers_); }
  int NorthLoad(const std::vector<Point>& rollers) const;

  void RollFirstNorth() {
    RollNorth();
  }
  void RollCycle();

  const std::vector<Point>& Summary() const {
    return rollers_;
  }

 private:
  void RollNorth();
  void RollSouth();
  void RollWest();
  void RollEast();

  static bool NorthCmp(Point a, Point b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
  }
  static bool SouthCmp(Point a, Point b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y > b.y;
  }
  static bool WestCmp(Point a, Point b) {
    if (a.y != b.y) return a.y < b.y;
    return a.x < b.x;
  };
  static bool EastCmp(Point a, Point b) {
    if (a.y != b.y) return a.y < b.y;
    return a.x > b.x;
  };

  std::vector<Point> rollers_;
  std::vector<Point> stops_;
  std::vector<Point> stops_for_north_;
  std::vector<Point> stops_for_south_;
  std::vector<Point> stops_for_west_;
  std::vector<Point> stops_for_east_;
  int width_;
  int height_;
};

int Dish::NorthLoad(const std::vector<Point>& rollers) const {
  int load = 0;
  for (Point p : rollers) {
    load += height_ - p.y;
  }
  return load;
}

void Dish::RollCycle() {
  RollNorth();
  RollWest();
  RollSouth();
  RollEast();
}

void Dish::RollNorth() {
  absl::c_sort(rollers_, NorthCmp);
  if (stops_for_north_.empty()) {
    stops_for_north_ = stops_;
    absl::c_sort(stops_for_north_, NorthCmp);
  }
  auto roller_it = rollers_.begin();
  auto stops_it = stops_for_north_.begin();
  Point stop;
  while (roller_it != rollers_.end()) {
    if (stops_it->x < roller_it->x) {
      ++stops_it;
      continue;
    }
    CHECK_EQ(stops_it->x, roller_it->x);
    if (stops_it->y < roller_it->y) {
      stop = *stops_it;
      ++stops_it;
      continue;
    }
    ++stop.y;
    *roller_it = stop;
    ++roller_it;
  }
}

void Dish::RollWest() {
  absl::c_sort(rollers_, WestCmp);
  if (stops_for_west_.empty()) {
    stops_for_west_ = stops_;
    absl::c_sort(stops_for_west_, WestCmp);
  }
  auto roller_it = rollers_.begin();
  auto stops_it = stops_for_west_.begin();
  Point stop;
  while (roller_it != rollers_.end()) {
    if (stops_it->y < roller_it->y) {
      ++stops_it;
      continue;
    }
    CHECK_EQ(stops_it->y, roller_it->y);
    if (stops_it->x < roller_it->x) {
      stop = *stops_it;
      ++stops_it;
      continue;
    }
    ++stop.x;
    *roller_it = stop;
    ++roller_it;
  }
}

void Dish::RollSouth() {
  absl::c_sort(rollers_, SouthCmp);
  if (stops_for_south_.empty()) {
    stops_for_south_ = stops_;
    absl::c_sort(stops_for_south_, SouthCmp);
  }
  auto roller_it = rollers_.begin();
  auto stops_it = stops_for_south_.begin();
  Point stop;
  while (roller_it != rollers_.end()) {
    if (stops_it->x < roller_it->x) {
      ++stops_it;
      continue;
    }
    CHECK_EQ(stops_it->x, roller_it->x);
    if (stops_it->y > roller_it->y) {
      stop = *stops_it;
      ++stops_it;
      continue;
    }
    --stop.y;
    *roller_it = stop;
    ++roller_it;
  }
}

void Dish::RollEast() {
  absl::c_sort(rollers_, EastCmp);
  if (stops_for_east_.empty()) {
    stops_for_east_ = stops_;
    absl::c_sort(stops_for_east_, EastCmp);
  }
  auto roller_it = rollers_.begin();
  auto stops_it = stops_for_east_.begin();
  Point stop;
  while (roller_it != rollers_.end()) {
    if (stops_it->y < roller_it->y) {
      ++stops_it;
      continue;
    }
    CHECK_EQ(stops_it->y, roller_it->y);
    if (stops_it->x > roller_it->x) {
      stop = *stops_it;
      ++stops_it;
      continue;
    }
    --stop.x;
    *roller_it = stop;
    ++roller_it;
  }
}


int CountLoad(const CharBoard& b) {
  std::vector<Point> rocks = b.FindVec('O');
  int load = 0;
  for (Point p : rocks) {
    load += b.height() - p.y;
  }
  return load;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_14::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  Dish dish(b);
  dish.RollFirstNorth();
  return AdventReturn(dish.NorthLoad());

  RollNorth(b);
  VLOG(2) << "Moved:\n" << b;
  return AdventReturn(CountLoad(b));
}

absl::StatusOr<std::string> Day_2023_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  VLOG(1) << b.range();
  VLOG(1) << b.Find('O').size() << " rollers";
  VLOG(1) << b.Find('#').size() << " stationary";
  
  if (false) {
    Dish dish(b);
    LoopHistory<std::vector<Point>> hist;
    for (int i = 0; i < 1000000000; ++i) {
       if (hist.AddMaybeNew(dish.Summary())) {
        VLOG(1) << hist;
        return AdventReturn(dish.NorthLoad(hist.FindInLoop(1000000000)));
      }
      dish.RollCycle();
    }
    return AdventReturn(dish.NorthLoad());
  }

  LoopHistory<CharBoard> hist;
  for (int i = 0; i < 1000000000; ++i) {
    if (hist.AddMaybeNew(b)) {
      VLOG(1) << hist;
      return AdventReturn(CountLoad(hist.FindInLoop(1000000000)));
    }
    RollNorth(b);
    RollWest(b);
    RollSouth(b);
    RollEast(b);
  }
  return AdventReturn(CountLoad(b));
}

}  // namespace advent_of_code
