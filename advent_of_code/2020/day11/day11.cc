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

class SeatConway : public ConwaySet<Point> {
 public:
  SeatConway(const CharBoard& board)
      : ConwaySet(ToSet(board)),
        bounds_(board.range()),
        allowed_(FindAllowed(board)) {}

  const absl::flat_hash_set<Point>& EmptyAllowed() const override {
    return allowed_;
  }

  std::vector<Point> Neighbors(const Point& p) const override {
    std::vector<Point> ret;
    for (Point d : Cardinal::kEightDirs) {
      Point n = p + d;
      if (!bounds_.Contains(n)) continue;
      if (!allowed_.contains(n)) continue;
      ret.push_back(n);
    }
    return ret;
  }

  bool IsLive(bool is_alive, int neighbors) const override {
    if (is_alive) return neighbors <= 3;
    return neighbors == 0;
  }

  std::vector<Point> Identifier() const {
    return StablePointSet(set());
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const SeatConway& c) {
    CharBoard draw(c.bounds_);
    for (Point p : c.allowed_) draw[p] = 'L';
    for (Point p : c.set()) draw[p] = '#';
    AbslStringify(sink, draw);
  }

 private:
  static absl::flat_hash_set<Point> ToSet(const CharBoard& board) {
    return board.Find('#');
  }
  static absl::flat_hash_set<Point> FindAllowed(const CharBoard& board) {
    absl::flat_hash_set<Point> ret = board.Find('#');
    for (Point p : board.Find('L')) ret.insert(p);
    return ret;
  }

  PointRectangle bounds_;
  absl::flat_hash_set<Point> allowed_;
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

  bool IsLive(bool is_alive, int neighbors) const override {
    if (is_alive) return neighbors <= 4;
    return neighbors == 0;
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
  return AdventReturn(seat_conway.CountLive());
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
  return AdventReturn(seat_conway.CountLive());
}

}  // namespace advent_of_code
