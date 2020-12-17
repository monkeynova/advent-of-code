#include "advent_of_code/2020/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

class Grid3 {
 public:
  Grid3() = default;

  Grid3(const CharBoard& char_board) {
    for (Point p : char_board.range()) {
      if (char_board[p] == '#') {
        grid_.insert(Point3{p.x, p.y, 0});
      }
    }
  }

  Grid3 Step() const {
    Grid3 ret;
    absl::flat_hash_map<Point3, int> neighbors;
    for (Point3 p : grid_) {
      // TODO(@monkeynova): xHat should be kXHat.
      for (Point3 xdir : {-Cardinal3::xHat, Cardinal3::kOrigin, Cardinal3::xHat}) {
        for (Point3 ydir : {-Cardinal3::yHat, Cardinal3::kOrigin, Cardinal3::yHat}) {
          for (Point3 zdir : {-Cardinal3::zHat, Cardinal3::kOrigin, Cardinal3::zHat}) {
            Point3 total_dir = xdir + ydir + zdir;
            if (total_dir == Cardinal3::kOrigin) continue;
            ++neighbors[p + total_dir];
          }
        }
      }
    }
    for (const auto& [point, count] : neighbors) {
      if (count == 3) ret.grid_.insert(point);
      else if (grid_.contains(point) && count == 2) ret.grid_.insert(point);
    }

    return ret;
  }

  int CountTiles() const {
    return grid_.size();
  } 
  
 private:
  absl::flat_hash_set<Point3> grid_;
};

class Grid4 {
 public:
  Grid4() = default;

  Grid4(const CharBoard& char_board) {
    for (Point p : char_board.range()) {
      if (char_board[p] == '#') {
        grid_.insert(Point4{p.x, p.y, 0, 0});
      }
    }
  }

  Grid4 Step() const {
    Grid4 ret;
    absl::flat_hash_map<Point4, int> neighbors;
    for (Point4 p : grid_) {
      // TODO(@monkeynova): xHat should be kXHat.
      for (Point4 xdir : {-Cardinal4::xHat, Cardinal4::kOrigin, Cardinal4::xHat}) {
        for (Point4 ydir : {-Cardinal4::yHat, Cardinal4::kOrigin, Cardinal4::yHat}) {
          for (Point4 zdir : {-Cardinal4::zHat, Cardinal4::kOrigin, Cardinal4::zHat}) {
            for (Point4 wdir : {-Cardinal4::wHat, Cardinal4::kOrigin, Cardinal4::wHat}) {
              Point4 total_dir = xdir + ydir + zdir + wdir;
              if (total_dir == Cardinal4::kOrigin) continue;
              ++neighbors[p + total_dir];
            }
          }
        }
      }
    }
    for (const auto& [point, count] : neighbors) {
      if (count == 3) ret.grid_.insert(point);
      else if (grid_.contains(point) && count == 2) ret.grid_.insert(point);
    }

    return ret;
  }

  int CountTiles() const {
    return grid_.size();
  } 
  
 private:
  absl::flat_hash_set<Point4> grid_;
};


absl::StatusOr<std::vector<std::string>> Day17_2020::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> initial = CharBoard::Parse(input);
  if (!initial.ok()) return initial.status();
  Grid3 grid(*initial);
  for (int i = 0; i < 6; ++i) {
    grid = grid.Step();
  }
  return IntReturn(grid.CountTiles());
}

absl::StatusOr<std::vector<std::string>> Day17_2020::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> initial = CharBoard::Parse(input);
  if (!initial.ok()) return initial.status();
  Grid4 grid(*initial);
  for (int i = 0; i < 6; ++i) {
    grid = grid.Step();
  }
  return IntReturn(grid.CountTiles());
}
