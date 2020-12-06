#include "advent_of_code/2019/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

bool IsCapAlpha(char c) { return c >= 'A' && c <= 'Z'; }

class Maze {
 public:
  Maze(const std::vector<absl::string_view>& input) : input_(input) {}

  absl::Status Initialize() {
    if (input_[0] != "Buffer") {
      return absl::InvalidArgumentError("Add buffer to avoid FBTD killing useful whitespace");
    }
    absl::flat_hash_map<std::string, std::vector<Point>> portals;
    board_.clear();
    for (int y = 3; y < input_.size() - 2; ++y) {
      std::string next_line;
      next_line.resize(input_[y].size() - 4, ' ');
      for (int x = 2; x < input_[y].size() - 2; ++x) {
        next_line[x-2] = input_[y][x];
        if (input_[y][x] == '.') {
          if (IsCapAlpha(input_[y - 1][x])) {
            char portal_name[] = {input_[y - 2][x], input_[y - 1][x], '\0'};
            portals[std::string(portal_name)].push_back(Point{.x = x - 2, .y = y - 3});
          }
          if (IsCapAlpha(input_[y + 1][x])) {
            char portal_name[] = {input_[y + 1][x], input_[y + 2][x], '\0'};
            portals[std::string(portal_name)].push_back(Point{.x = x - 2, .y = y - 3});
          }
          if (IsCapAlpha(input_[y][x - 1])) {
            char portal_name[] = {input_[y][x - 2], input_[y][x - 1], '\0'};
            portals[std::string(portal_name)].push_back(Point{.x = x - 2, .y = y - 3});
          }
          if (IsCapAlpha(input_[y][x + 1])) {
            char portal_name[] = {input_[y][x + 1], input_[y][x + 2], '\0'};
            portals[std::string(portal_name)].push_back(Point{.x = x - 2, .y = y - 3});
          }
        }
      }
      board_.push_back(std::move(next_line));
    }
    {
      auto it = portals.find("AA");
      if (it == portals.end()) return absl::InvalidArgumentError("No start");
      if (it->second.size() != 1) return absl::InvalidArgumentError("Bad start");
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
      if (points.size() != 2) return absl::InvalidArgumentError("Portal not connected right");
      portals_.emplace(points[0], points[1]);
      portals_.emplace(points[1], points[0]);
    }

    VLOG(1) << "\nBoard:\n" << absl::StrJoin(board_, "\n")
    << "\nStart: " << start_ << "; End: " << end_
    << "\nPortals:\n"
     << absl::StrJoin(portals_, "\n",
      [](std::string* out, const std::pair<Point, Point>& portal) {
        absl::StrAppend(out, portal.first.DebugString(), "<->", portal.second.DebugString());
      });
    return absl::OkStatus();
  }

  bool CanStand(Point p) {
    if (p.x < 0) return false;
    if (p.y < 0) return false;
    if (p.y >= board_.size()) return false;
    if (p.x >= board_[0].size()) return false;
    return board_[p.y][p.x] == '.'; 
  }

  absl::StatusOr<int> FindPath() {
    struct Path {
      Point pos;
      int steps = 0;
    };
    absl::flat_hash_set<Point> hist;
    std::deque<Path> frontier;
    frontier.push_back(Path{.pos = start_, .steps = 0});
    while (!frontier.empty()) {
      const Path& cur = frontier.front();
      if (cur.pos == end_) return cur.steps;
      hist.insert(cur.pos);
      for (Point dir : Cardinal::kAll) {
        Point next = cur.pos + dir;
        if (hist.contains(next)) continue;
        if (CanStand(next)) {
          frontier.push_back(Path{.pos = next, .steps = cur.steps + 1});
        }
      }
      if (auto it = portals_.find(cur.pos); it != portals_.end()) {
        Point next = it->second;
        if (!hist.contains(next)) {
          frontier.push_back(Path{.pos = next, .steps = cur.steps + 1});
        }
      }
      frontier.pop_front();
    }
    return absl::InvalidArgumentError("Could not find path");
  }

 private:
  const std::vector<absl::string_view>& input_;

  std::vector<std::string> board_;
  absl::flat_hash_map<Point, Point> portals_;
  Point start_;
  Point end_;
};

absl::StatusOr<std::vector<std::string>> Day20_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  Maze maze(input);
  if (absl::Status st = maze.Initialize(); !st.ok()) return st;
  absl::StatusOr<int> path_length = maze.FindPath();
  if (!path_length.ok()) return path_length.status();

  return std::vector<std::string>{absl::StrCat(*path_length)};
}

absl::StatusOr<std::vector<std::string>> Day20_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
