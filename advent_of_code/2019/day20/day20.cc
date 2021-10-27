#include "advent_of_code/2019/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

bool IsCapAlpha(char c) { return c >= 'A' && c <= 'Z'; }

class Maze {
 public:
  static absl::StatusOr<Maze> Create(absl::Span<absl::string_view> input) {
    while (!input.empty() && input.back() == "") {
      input = input.subspan(0, input.size() - 1); 
    }
    absl::StatusOr<CharBoard> with_portals = CharBoard::Parse(input);
    if (!with_portals.ok()) return with_portals.status();

    absl::flat_hash_map<std::string, std::vector<Point>> portals;

    PointRectangle range = with_portals->range();
    range.min += Point{2, 2};
    range.max -= Point{2, 2};

    Maze ret(with_portals->width() - 4, with_portals->height() - 4);
    for (Point p : range) {
      ret.board_[p - Point{2, 2}] = with_portals->at(p);
      if (with_portals->at(p) == '.') {
        if (IsCapAlpha(with_portals->at(p + Point{0, -1}))) {
          char portal_name[] = {with_portals->at(p + Point{0, -2}), with_portals->at(p + Point{0, -1}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
        if (IsCapAlpha(with_portals->at(p + Point{0, 1}))) {
          char portal_name[] = {with_portals->at(p + Point{0, 1}), with_portals->at(p + Point{0, 2}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
        if (IsCapAlpha(with_portals->at(p + Point{-1, 0}))) {
          char portal_name[] = {with_portals->at(p + Point{-2, 0}), with_portals->at(p + Point{-1, 0}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
        if (IsCapAlpha(with_portals->at(p + Point{1, 0}))) {
          char portal_name[] = {with_portals->at(p + Point{1, 0}), with_portals->at(p + Point{2, 0}), '\0'};
          portals[std::string(portal_name)].push_back(p - Point{2, 2});
        }
      }
    }
    {
      auto it = portals.find("AA");
      if (it == portals.end()) return absl::InvalidArgumentError("No start");
      if (it->second.size() != 1)
        return absl::InvalidArgumentError("Bad start");
      ret.start_ = it->second[0];
      portals.erase(it);
    }
    {
      auto it = portals.find("ZZ");
      if (it == portals.end()) return absl::InvalidArgumentError("No end");
      if (it->second.size() != 1) return absl::InvalidArgumentError("Bad end");
      ret.end_ = it->second[0];
      portals.erase(it);
    }
    for (const auto& name_to_points : portals) {
      const std::vector<Point>& points = name_to_points.second;
      if (points.size() != 2)
        return absl::InvalidArgumentError("Portal not connected right");
      ret.portals_.emplace(points[0], points[1]);
      ret.portals_.emplace(points[1], points[0]);
    }

    VLOG(1) << "\nBoard:\n" << ret.board_
            << "\nStart: " << ret.start_
            << "; End: " << ret.end_ << "\nPortals:\n"
            << absl::StrJoin(
                   ret.portals_, "\n",
                   [](std::string* out, const std::pair<Point, Point>& portal) {
                     absl::StrAppend(out, portal.first.DebugString(), "<->",
                                     portal.second.DebugString());
                   });
    return ret;
  }

  bool CanStand(Point p) const {
    if (!board_.OnBoard(p)) return false;
    return board_[p] == '.';
  }

  bool OnEdge(Point p) const {
    if (p.x == 0) return true;
    if (p.y == 0) return true;
    if (p.y == board_.height() - 1) return true;
    if (p.x == board_.width() - 1) return true;
    return false;
  }

  absl::optional<int> FindPath() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const Maze& maze, Point start, Point end)
          : maze_(maze), cur_(start), end_(end) {}

      Point identifier() const override { return cur_; }
      bool IsFinal() override { return cur_ == end_; }

      void AddNextSteps(State* state) override {
        for (Point dir : Cardinal::kFourDirs) {
          Point next_cur = cur_ + dir;
          if (maze_.CanStand(next_cur)) {
            PathWalk next = *this;
            next.cur_ = next_cur;
            state->AddNextStep(next);
          }
        }
        if (auto it = maze_.portals().find(cur_); it != maze_.portals().end()) {
          PathWalk next = *this;
          next.cur_ = it->second;
          state->AddNextStep(next);
        }
      }

     private:
      const Maze& maze_;
      Point cur_;
      Point end_;
    };
    return PathWalk(*this, start_, end_).FindMinSteps();
  }

  absl::optional<int> FindRecursivePath() {
    class PathWalk : public BFSInterface<PathWalk, Point3> {
     public:
      PathWalk(const Maze& maze, Point start, Point end)
          : maze_(maze), cur_({start.x, start.y, 0}), end_({end.x, end.y, 0}) {}

      Point3 identifier() const override { return cur_; }
      bool IsFinal() override { return cur_ == end_; }

      void AddNextSteps(State* state) override {
        for (Point dir : Cardinal::kFourDirs) {
          Point3 next_cur = cur_ + Point3{dir.x, dir.y, 0};
          if (maze_.CanStand({next_cur.x, next_cur.y})) {
            PathWalk next = *this;
            next.cur_ = next_cur;
            state->AddNextStep(next);
          }
        }
        if (auto it = maze_.portals().find({cur_.x, cur_.y});
            it != maze_.portals().end()) {
          PathWalk next = *this;
          next.cur_ = {it->second.x, it->second.y,
                       cur_.z + (maze_.OnEdge({cur_.x, cur_.y}) ? -1 : +1)};
          if (next.cur_.z >= 0) {
            state->AddNextStep(next);
          }
        }
      }

     private:
      const Maze& maze_;
      Point3 cur_;
      Point3 end_;
    };
    return PathWalk(*this, start_, end_).FindMinSteps();
  }

  const absl::flat_hash_map<Point, Point>& portals() const { return portals_; }

 private:
  Maze(int width, int height) : board_(width, height) {}

  CharBoard board_;
  absl::flat_hash_map<Point, Point> portals_;
  Point start_;
  Point end_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Maze> maze = Maze::Create(input);
  if (!maze.ok()) return maze.status();

  return IntReturn(maze->FindPath());
}

absl::StatusOr<std::string> Day_2019_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<Maze> maze = Maze::Create(input);
  if (!maze.ok()) return maze.status();

  return IntReturn(maze->FindRecursivePath());
}

}  // namespace advent_of_code
