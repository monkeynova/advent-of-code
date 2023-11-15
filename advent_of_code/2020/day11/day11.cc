#include "advent_of_code/2020/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

class SeatConway : public ConwayMultiSet<Point, 2> {
 public:
  SeatConway(const CharBoard& board)
      : ConwayMultiSet({board.Find('L'), board.Find('#')}),
        bounds_(board.range()) {}

  int64_t CountFilled() const {
    return CountState(1);
  }

  std::vector<Point> Neighbors(const Point& p) const override {
    std::vector<Point> ret;
    for (Point d : Cardinal::kEightDirs) {
      Point n = p + d;
      if (!bounds_.Contains(n)) continue;
      ret.push_back(n);
    }
    return ret;
  }

  int NextState(int cur_state, std::array<int, 2> neighbors) const override {
    if (cur_state < 0) return cur_state;
    if (cur_state == 1) return neighbors[1] <= 3 ? 1 : 0;
    return neighbors[1] == 0 ? 1 : 0;
  }

  std::vector<Point> Identifier() const {
    return StablePointSet(sets()[1]);
  }

  std::string Draw() const override {
    CharBoard draw(bounds_);
    draw.DrawOn(sets()[0], 'L');
    draw.DrawOn(sets()[1], '#');
    return absl::StrCat(draw);
  }

 private:
  PointRectangle bounds_;
};

class SeatConwayVismap : public SeatConway {
 public:
  SeatConwayVismap(const CharBoard& b)
      : SeatConway(b), vismap_(ComputeVismap(b)) {}

  std::vector<Point> Neighbors(const Point& p) const override {
    auto it = vismap_.find(p);
    if (it == vismap_.end()) return {};
    return it->second;
  }

  int NextState(int cur_state, std::array<int, 2> neighbors) const override {
    if (cur_state < 0) return cur_state;
    if (cur_state == 1) return neighbors[1] <= 4 ? 1 : 0;
    return neighbors[1] == 0 ? 1 : 0;
  }

 private:
  using VisMap = absl::flat_hash_map<Point, std::vector<Point>>;
  static VisMap ComputeVismap(const CharBoard& in) {
    VisMap vis_map;
    for (Point c : in.range()) {
      if (in[c] == '.') continue;
      for (Point dir : Cardinal::kEightDirs) {
        Point n = c + dir;
        while (in.OnBoard(n)) {
          if (in[n] != '.') {
            vis_map[c].push_back(n);
            break;
          }
          n += dir;
        }
      }
    }
    return vis_map;
  }

  VisMap vismap_;
};

}  // namespace

absl::StatusOr<std::string> Day_2020_11::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> parsed = CharBoard::Parse(input);
  if (!parsed.ok()) return parsed.status();

  SeatConway seat_conway(*parsed);
  VLOG(2) << seat_conway;
  while (seat_conway.Advance()) {
    VLOG(2) << seat_conway;
  }
  return AdventReturn(seat_conway.CountFilled());
}

absl::StatusOr<std::string> Day_2020_11::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> parsed = CharBoard::Parse(input);
  if (!parsed.ok()) return parsed.status();

  SeatConwayVismap seat_conway(*parsed);
  VLOG(2) << seat_conway;
  while (seat_conway.Advance()) {
    VLOG(2) << seat_conway;
  }
  return AdventReturn(seat_conway.CountFilled());
}

}  // namespace advent_of_code
