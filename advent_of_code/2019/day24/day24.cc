#include "advent_of_code/2019/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/opt_cmp.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

absl::StatusOr<CharBoard> ParseBoard(absl::Span<std::string_view> input) {
  absl::StatusOr<CharBoard> board = CharBoard::Parse(input);
  if (!board.ok()) return board;
  if (board->height() != 5) {
    return absl::InvalidArgumentError("Board isn't of height 5");
  }
  if (board->width() != 5) {
    return absl::InvalidArgumentError("Board isn't of width 5");
  }
  return board;
}

class Part1Conway : public ConwaySet<Point> {
 public:
  Part1Conway(const CharBoard& board)
   : ConwaySet(ToSet(board)), bounds_(board.range()) {}

  std::vector<Point> Neighbors(const Point& p) const override {
    std::vector<Point> ret(Cardinal::kFourDirs.begin(), Cardinal::kFourDirs.end());
    for (Point& r : ret) r += p;
    ret.erase(
      std::remove_if(
        ret.begin(), ret.end(), [&](Point p) { return !bounds_.Contains(p); }),
      ret.end());
    return ret;
  }

  bool IsLive(bool is_live, int neighbors) const override {
    if (is_live) return neighbors == 1;
    return neighbors == 1 || neighbors == 2;
  }

  std::vector<Point> Identifier() const {
    std::vector<Point> ret(set().begin(), set().end());
    absl::c_sort(ret, PointYThenXLT());
    return ret;
  }

  int64_t BioDiversity() {
    int64_t ret = 0;
    for (Point p : set()) {
      ret |= (1ll << (p.y * (bounds_.max.x + 1) + p.x));
    }
    return ret;
  }

 private:
  static absl::flat_hash_set<Point> ToSet(const CharBoard& board) {
    return board.Find('#');
  }

  PointRectangle bounds_;
};

class Part2Conway : public ConwaySet<Point3> {
 public:
  Part2Conway(const CharBoard& board)
   : ConwaySet(ToSet(board)), bounds_(board.range()) {}

  std::vector<Point3> Neighbors(const Point3& p) const override {
    std::vector<Point3> ret;
    for (Point dir2 : Cardinal::kFourDirs) {
      Point3 dir3 = Point3{dir2.x, dir2.y, 0};
      Point3 n = dir3 + p;
      if (n.x == 2 && n.y == 2) {
        // In the center, reach down a level.
        if (dir2 == Cardinal::kNorth) { 
          for (int i = 0; i <= bounds_.max.x; ++i) {
            ret.push_back(Point3{i, bounds_.max.y, p.z + 1});
          }
        } else if (dir2 == Cardinal::kSouth) { 
          for (int i = 0; i <= bounds_.max.x; ++i) {
            ret.push_back(Point3{i, bounds_.min.y, p.z + 1});
          }
        } else if (dir2 == Cardinal::kWest) { 
          for (int i = 0; i <= bounds_.max.y; ++i) {
            ret.push_back(Point3{bounds_.max.x, i, p.z + 1});
          }
        } else if (dir2 == Cardinal::kEast) { 
          for (int i = 0; i <= bounds_.max.y; ++i) {
            ret.push_back(Point3{bounds_.min.x, i, p.z + 1});
          }
        } else {
          LOG(FATAL) << "Impossible dir";
        }
        
      } else if (!bounds_.Contains({n.x, n.y})) {
        // Out of edge, reach up a level.
        ret.push_back(Point3{2, 2, p.z - 1} + dir3);
      } else {
        ret.push_back(n);
      }
    }
    return ret;
  }

  bool IsLive(bool is_live, int neighbors) const override {
    if (is_live) return neighbors == 1;
    return neighbors == 1 || neighbors == 2;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Part2Conway& c) {
    std::map<int, CharBoard> boards;
    for (Point3 p : c.set()) {
      if (boards.find(p.z) == boards.end()) {
        CharBoard tmp(5, 5);
        tmp[{2, 2}] = '?';
        boards.emplace(p.z, std::move(tmp));
      }
      CHECK(c.bounds_.Contains({p.x, p.y})) << p;
      boards[p.z][{p.x, p.y}] = '#';
    }
    for (const auto& [z, b] : boards) {
      absl::Format(&sink, "%d\n%v\n", z, b);
    }
  }

 private:
  static absl::flat_hash_set<Point3> ToSet(const CharBoard& board) {
    CHECK_EQ(board.height(), 5);
    CHECK_EQ(board.width(), 5);
    absl::flat_hash_set<Point3> ret;
    for (Point p : board.Find('#')) {
      ret.insert({p.x, p.y, 0});
    }
    return ret;
  }

  PointRectangle bounds_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_24::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseBoard(input);
  if (!board.ok()) return board.status();

  Part1Conway p1c(*board);
  absl::flat_hash_set<std::vector<Point>> hist;
  while (hist.insert(p1c.Identifier()).second) {
    p1c.Advance();
  }
  return AdventReturn(p1c.BioDiversity());
}

absl::StatusOr<std::string> Day_2019_24::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> cur = ParseBoard(input);
  if (!cur.ok()) return cur.status();

  Part2Conway p2c(*cur);
  p2c.AdvanceN(200);
  return AdventReturn(p2c.CountLive());
}

}  // namespace advent_of_code
