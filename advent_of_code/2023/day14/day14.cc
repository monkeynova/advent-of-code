// http://adventofcode.com/2023/day/14

#include "advent_of_code/2023/day14/day14.h"

#include "absl/log/log.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

namespace {

class Dish {
 public:
  explicit Dish(const ImmutableCharBoard& b)
   : rollers_by_x_(b.width()),
     rollers_by_y_(b.height()),
     stops_by_x_(b.width()),
     stops_by_y_(b.height()),
     width_(b.width()),
     height_(b.height()) { 
    for (Point r : b.FindVec('O')) {
      rollers_by_x_[r.x].push_back(r.y);
    }
    for (int x = 0; x < width_; ++x) {
      stops_by_x_[x].push_back(-1);
    }
    for (int y = 0; y < height_; ++y) {
      stops_by_y_[y].push_back(-1);
    }
    for (Point s : b.FindVec('#')) {
      stops_by_x_[s.x].push_back(s.y);
      stops_by_y_[s.y].push_back(s.x);
    }
    for (int x = 0; x < width_; ++x) {
      stops_by_x_[x].push_back(height_);
    }
    for (int y = 0; y < height_; ++y) {
      stops_by_y_[y].push_back(width_);
    }
    rollers_by_x_valid_ = true;
  }

  int NorthLoadPart1() const;
  int NorthLoad(const std::vector<std::vector<int>>& rollers_by_x) const;

  void RollFirstNorth() {
    RollNorth();
  }
  void RollCycle();

  std::vector<std::vector<int>> Summary() const {
    return rollers_by_x_;
  }

 private:
  void RollNorth();
  void RollSouth();
  void RollWest();
  void RollEast();

  std::vector<std::vector<int>> rollers_by_x_;
  std::vector<std::vector<int>> rollers_by_y_;
  std::vector<std::vector<int>> stops_by_x_;
  std::vector<std::vector<int>> stops_by_y_;

  int width_;
  int height_;
  bool rollers_by_x_valid_;
};

int Dish::NorthLoadPart1() const {
  int load = 0;
  for (int y = 0; y < height_; ++y) {
    load += rollers_by_y_[y].size() * (height_ - y);
  }
  return load;
}


int Dish::NorthLoad(const std::vector<std::vector<int>>& rollers_by_x) const {
  int load = 0;
  for (int x = 0; x < width_; ++x) {
    for (int y : rollers_by_x[x]) {
      load += height_ - y;
    }
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
  for (int y = 0; y < height_; ++y) {
    rollers_by_y_[y].clear();
  }
  // Next is West, sort by x ASC.
  for (int x = 0; x < width_; ++x) {
    auto roller_it = rollers_by_x_[x].begin();
    auto stop_it = stops_by_x_[x].begin();
    int stop_y = *stop_it;
    while (roller_it != rollers_by_x_[x].end()) {
      if (*stop_it < *roller_it) {
        stop_y = *stop_it;
        ++stop_it;
        continue;
      }
      rollers_by_y_[++stop_y].push_back(x);
      ++roller_it;
    }
  }
}

void Dish::RollWest() {
  for (int x = 0; x < width_; ++x) {
    rollers_by_x_[x].clear();
  }
  // Next is South, sort by y DESC.
  for (int y = height_ - 1; y >= 0; --y) {
    auto roller_it = rollers_by_y_[y].begin();
    auto stop_it = stops_by_y_[y].begin();
    int stop_x = *stop_it;
    while (roller_it != rollers_by_y_[y].end()) {
      if (*stop_it < *roller_it) {
        stop_x = *stop_it;
        ++stop_it;
        continue;
      }
      rollers_by_x_[++stop_x].push_back(y);
      ++roller_it;
    }
  }
}

void Dish::RollSouth() {
  for (int y = 0; y < height_; ++y) {
    rollers_by_y_[y].clear();
  }
  // Next is East, sort by x DESC.
  for (int x = width_ - 1; x >= 0; --x) {
    auto roller_it = rollers_by_x_[x].begin();
    auto stop_it = stops_by_x_[x].rbegin();
    int stop_y = *stop_it;
    while (roller_it != rollers_by_x_[x].end()) {
      if (*stop_it > *roller_it) {
        stop_y = *stop_it;
        ++stop_it;
        continue;
      }
      rollers_by_y_[--stop_y].push_back(x);
      ++roller_it;
    }
  }
}

void Dish::RollEast() {
  for (int x = 0; x < width_; ++x) {
    rollers_by_x_[x].clear();
  }
  // Next is North, sort by y ASC.
  for (int y = 0; y < height_; ++y) {
    auto roller_it = rollers_by_y_[y].begin();
    auto stop_it = stops_by_y_[y].rbegin();
    int stop_x = *stop_it;
    while (roller_it != rollers_by_y_[y].end()) {
      if (*stop_it > *roller_it) {
        stop_x = *stop_it;
        ++stop_it;
        continue;
      }
      rollers_by_x_[--stop_x].push_back(y);
      ++roller_it;
    }
  }
}

}  // namespace

absl::StatusOr<std::string> Day_2023_14::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  Dish dish(b);
  dish.RollFirstNorth();
  return AdventReturn(dish.NorthLoadPart1());
}

absl::StatusOr<std::string> Day_2023_14::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  VLOG(1) << b.range();
  VLOG(1) << b.Find('O').size() << " rollers";
  VLOG(1) << b.Find('#').size() << " stationary";
  
  Dish dish(b);
  LoopHistory<decltype(dish.Summary())> hist;
  for (int i = 0; i < 1000000000; ++i) {
     if (hist.AddMaybeNew(dish.Summary())) {
      VLOG(1) << hist;
      return AdventReturn(dish.NorthLoad(hist.FindInLoop(1000000000)));
    }
    dish.RollCycle();
  }
  return AdventReturn(dish.NorthLoad(dish.Summary()));
}

}  // namespace advent_of_code
