#include "advent_of_code/2019/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

// TODO(@monkeynova): Move to CharBoard.

bool IsCapAlpha(char c) { return c >= 'A' && c <= 'Z'; }

class Maze {
 public:
  Maze(absl::Span<absl::string_view> input) : input_(input) {}

  absl::Status Initialize() {
    if (input_[0] != "HACK: Ignore Starting Whitespace") {
      return absl::InvalidArgumentError(
          "Add buffer to avoid FBTD killing useful whitespace");
    }
    absl::flat_hash_map<std::string, std::vector<Point>> portals;
    board_.clear();
    for (int y = 3; y < input_.size() - 2; ++y) {
      std::string next_line;
      next_line.resize(input_[y].size() - 4, ' ');
      for (int x = 2; x < input_[y].size() - 2; ++x) {
        next_line[x - 2] = input_[y][x];
        if (input_[y][x] == '.') {
          if (IsCapAlpha(input_[y - 1][x])) {
            char portal_name[] = {input_[y - 2][x], input_[y - 1][x], '\0'};
            portals[std::string(portal_name)].push_back(
                Point{.x = x - 2, .y = y - 3});
          }
          if (IsCapAlpha(input_[y + 1][x])) {
            char portal_name[] = {input_[y + 1][x], input_[y + 2][x], '\0'};
            portals[std::string(portal_name)].push_back(
                Point{.x = x - 2, .y = y - 3});
          }
          if (IsCapAlpha(input_[y][x - 1])) {
            char portal_name[] = {input_[y][x - 2], input_[y][x - 1], '\0'};
            portals[std::string(portal_name)].push_back(
                Point{.x = x - 2, .y = y - 3});
          }
          if (IsCapAlpha(input_[y][x + 1])) {
            char portal_name[] = {input_[y][x + 1], input_[y][x + 2], '\0'};
            portals[std::string(portal_name)].push_back(
                Point{.x = x - 2, .y = y - 3});
          }
        }
      }
      board_.push_back(std::move(next_line));
    }
    {
      auto it = portals.find("AA");
      if (it == portals.end()) return absl::InvalidArgumentError("No start");
      if (it->second.size() != 1)
        return absl::InvalidArgumentError("Bad start");
      start_ = it->second[0];
      portals.erase(it);
    }
    {
      auto it = portals.find("ZZ");
      if (it == portals.end()) return absl::InvalidArgumentError("No end");
      if (it->second.size() != 1) return absl::InvalidArgumentError("Bad end");
      end_ = it->second[0];
      portals.erase(it);
    }
    for (const auto& name_to_points : portals) {
      const std::vector<Point>& points = name_to_points.second;
      if (points.size() != 2)
        return absl::InvalidArgumentError("Portal not connected right");
      portals_.emplace(points[0], points[1]);
      portals_.emplace(points[1], points[0]);
    }

    VLOG(1) << "\nBoard:\n"
            << absl::StrJoin(board_, "\n") << "\nStart: " << start_
            << "; End: " << end_ << "\nPortals:\n"
            << absl::StrJoin(
                   portals_, "\n",
                   [](std::string* out, const std::pair<Point, Point>& portal) {
                     absl::StrAppend(out, portal.first.DebugString(), "<->",
                                     portal.second.DebugString());
                   });
    return absl::OkStatus();
  }

  bool CanStand(Point p) const {
    if (p.x < 0) return false;
    if (p.y < 0) return false;
    if (p.y >= board_.size()) return false;
    if (p.x >= board_[0].size()) return false;
    return board_[p.y][p.x] == '.';
  }

  bool OnEdge(Point p) const {
    if (p.x == 0) return true;
    if (p.y == 0) return true;
    if (p.y == board_.size() - 1) return true;
    if (p.x == board_[0].size() - 1) return true;
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
  const absl::Span<absl::string_view> input_;

  std::vector<std::string> board_;
  absl::flat_hash_map<Point, Point> portals_;
  Point start_;
  Point end_;
};

}  // namespace

absl::StatusOr<std::vector<std::string>> Day20_2019::Part1(
    absl::Span<absl::string_view> input) const {
  Maze maze(input);
  if (absl::Status st = maze.Initialize(); !st.ok()) return st;

  return IntReturn(maze.FindPath());
}

absl::StatusOr<std::vector<std::string>> Day20_2019::Part2(
    absl::Span<absl::string_view> input) const {
  Maze maze(input);
  if (absl::Status st = maze.Initialize(); !st.ok()) return st;

  return IntReturn(maze.FindRecursivePath());
}

}  // namespace advent_of_code
