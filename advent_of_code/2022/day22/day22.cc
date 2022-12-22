// http://adventofcode.com/2022/day/22

#include "advent_of_code/2022/day22/day22.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

using StitchMap = absl::flat_hash_map<std::pair<Point, Point>, std::pair<Point, Point>>;

void Stitch(StitchMap& ret, Point s1, Point s2, Point d1, Point e1, Point e2, Point d2) {
  CHECK_EQ((s2 - s1).dist(), (e2 - e1).dist()) << s1 << "-" << s2 << "; " << e1 << "-" << e2;
  Point ds = (s2 - s1).min_step();
  Point de = (e2 - e1).min_step();

  for (Point s = s1, e = e1; true; s += ds, e += de) {
    ret[{s, d1}] = {e, d2};
    ret[{e, -d2}] = {s, -d1};
    VLOG(2) << s << "," << d1 << " => " << e << "," << d2;
    VLOG(2) << e << "," << -d2 << " => " << s << "," << -d1;
    if (s == s2) break;
  }
}

StitchMap BuildStitchMap(const CharBoard& board) {
  int tile_width = std::gcd(board.width(), board.height());

  StitchMap ret;
  if (tile_width == 4) {
    // ..1.
    // 234.
    // ..56
    // 1-3
    Stitch(ret, {8, 0}, {8, 3}, Cardinal::kWest, {4, 4}, {7, 4}, Cardinal::kSouth);
    // 1-2
    Stitch(ret, {8, 0}, {11, 0}, Cardinal::kNorth, {3, 3}, {0, 3}, Cardinal::kSouth);
    // 1-6
    Stitch(ret, {11, 0}, {11, 3}, Cardinal::kWest, {15, 11}, {12, 11}, Cardinal::kEast);
    // 4-6
    Stitch(ret, {11, 4}, {11, 7}, Cardinal::kEast, {15, 8}, {12, 8}, Cardinal::kSouth);
    // 3-5
    Stitch(ret, {4, 7}, {7, 7}, Cardinal::kSouth, {8, 11}, {8, 8}, Cardinal::kEast);
    // 2-5
    Stitch(ret, {0, 7}, {3, 7}, Cardinal::kSouth, {11, 11}, {8, 11}, Cardinal::kNorth);
    // 2-6
    Stitch(ret, {15, 11}, {12, 11}, Cardinal::kSouth, {0, 4}, {0, 7}, Cardinal::kEast);
  }

  if (tile_width == 50) {
    // .12
    // .3.
    // 45.
    // 6..
  
    // 3-4
    Stitch(ret, {0, 100}, {49, 100}, Cardinal::kNorth, {50, 50}, {50, 99}, Cardinal::kEast);
    // 5-6
    Stitch(ret, {50, 149}, {99, 149}, Cardinal::kSouth, {49, 150}, {49, 199}, Cardinal::kWest);
    // 2-3
    Stitch(ret, {100, 49}, {149, 49}, Cardinal::kSouth, {99, 50}, {99, 99}, Cardinal::kWest);
    // 5-2
    Stitch(ret, {99, 100}, {99, 149}, Cardinal::kEast, {149, 49}, {149, 0}, Cardinal::kWest);
    // 1-4
    Stitch(ret, {50, 0}, {50, 49}, Cardinal::kWest, {0, 149}, {0, 100}, Cardinal::kEast);
    // 1-6
    Stitch(ret, {0, 150}, {0, 199}, Cardinal::kWest, {50, 0}, {99, 0}, Cardinal::kSouth);
    // 2-6
    Stitch(ret, {0, 199}, {49, 199}, Cardinal::kSouth, {100, 0}, {149, 0}, Cardinal::kSouth);
  }

  return ret;

  for (Point p : board.range()) {
    if (board[p] != ' ') continue;
    for (Point c1 : {Cardinal::kSouth, Cardinal::kNorth}) {
      if (!board.OnBoard(p + c1)) continue;
      if (board[p + c1] == ' ') continue;
      for (Point c2 : {Cardinal::kWest, Cardinal::kEast}) {
        if (!board.OnBoard(p + c2)) continue;
        if (board[p + c2] == ' ') continue;

        for (int i = 0; i < tile_width; ++i) {
          std::pair<Point, Point> from;
          from.first = p + c1 - i * c2;
          from.second = -c1;
          std::pair<Point, Point> to;
          to.first = p + c2 - i * c1;
          to.second = c2;
          ret[from] = to;
          VLOG(1) << from.first << "/" << from.second << " -> " << to.first << "/" << to.second;
          std::swap(from, to);
          from.second = -from.second;
          to.second = -to.second;
          ret[from] = to;
          VLOG(1) << from.first << "/" << from.second << " -> " << to.first << "/" << to.second;
        }
      }
    }
  }
  // TODO: Handle non-corners...
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_22::Part1(
    absl::Span<absl::string_view> input) const {
  absl::string_view path = input.back();
  input = input.subspan(0, input.size() - 2);
  
  int max = 0;
  for (absl::string_view line : input) {
    max = std::max<int>(max, line.size());
  }
  std::vector<std::string> copy;
  for (absl::string_view line : input) {
    copy.push_back(std::string(line));
    copy.back().resize(max, ' ');
  }

  absl::StatusOr<CharBoard> board_or = CharBoard::Parse(copy);
  if (!board_or.ok()) return board_or.status();
  CharBoard board = std::move(*board_or);

  std::optional<Point> start;
  for (Point p : board.range()) {
    if (board[p] == '.') {
      start = p;
      break;
    }
  }
  if (!start) return Error("No start");


  Point cur = *start;
  Point dir = Cardinal::kEast;
  int dist = 0;

  auto move = [&]() {
    VLOG(2) << cur << " d=" << dir;
    VLOG(2) << "move: " << dist;
      for (int i = 0; i < dist; ++i) {
        Point next = board.TorusPoint(cur + dir);
        while (board[next] == ' ') {
          next = board.TorusPoint(next + dir);
        }
        if (board[next] == '#') break;
        cur = next;
      }
    dist = 0;
    VLOG(2) << cur;
  };

  for (char c : path) {
    if (c >= '0' && c <= '9') dist = dist * 10 + c - '0';
    else if (c == 'R') {
      move();
      dir = dir.rotate_right();
    } else if (c == 'L') {
      move();
      dir = dir.rotate_left();
    } else {
      return Error("Bad path");
    }
  }
  move();

  int score = 0;
  score += 1000 * (cur.y + 1) + 4 * (cur.x + 1);
  if (dir == Cardinal::kEast) score += 0;
  else if (dir == Cardinal::kSouth) score += 1;
  else if (dir == Cardinal::kWest) score += 2;
  else if (dir == Cardinal::kNorth) score += 3;
  else return Error("Bad dir");

  return IntReturn(score);
}

absl::StatusOr<std::string> Day_2022_22::Part2(
    absl::Span<absl::string_view> input) const {
  absl::string_view path = input.back();
  input = input.subspan(0, input.size() - 2);
  
  int max = 0;
  for (absl::string_view line : input) {
    max = std::max<int>(max, line.size());
  }
  std::vector<std::string> copy;
  for (absl::string_view line : input) {
    copy.push_back(std::string(line));
    copy.back().resize(max, ' ');
  }

  absl::StatusOr<CharBoard> board_or = CharBoard::Parse(copy);
  if (!board_or.ok()) return board_or.status();
  CharBoard board = std::move(*board_or);

  std::optional<Point> start;
  for (Point p : board.range()) {
    if (board[p] == '.') {
      start = p;
      break;
    }
  }
  if (!start) return Error("No start");

  Point cur = *start;
  Point dir = Cardinal::kEast;
  int dist = 0;

  StitchMap stitched = BuildStitchMap(board);

  Point next;
  Point next_dir;

  auto cube_move = [&]() {
    if (stitched.contains({cur, dir})) {
      auto pair = stitched[{cur, dir}];
      next = pair.first;
      next_dir = pair.second;
      VLOG(2) << "Stitch: " << cur << "->" << next;
    } else {
      next = cur + dir;
      next_dir = dir;
    }
    CHECK(board.OnBoard(next)) << next;
    CHECK(board[next] != ' ') << next;
  };

  auto move = [&]() {
    VLOG(2) << cur << " d=" << dir;
    VLOG(2) << "move: " << dist;
      for (int i = 0; i < dist; ++i) {
        cube_move();
        if (board[next] == '#') break;
        cur = next;
        dir = next_dir;
      }
    dist = 0;
    VLOG(2) << cur;
  };

  for (char c : path) {
    if (c >= '0' && c <= '9') dist = dist * 10 + c - '0';
    else if (c == 'R') {
      move();
      dir = dir.rotate_right();
    } else if (c == 'L') {
      move();
      dir = dir.rotate_left();
    } else {
      return Error("Bad path");
    }
  }
  move();

  int score = 0;
  score += 1000 * (cur.y + 1) + 4 * (cur.x + 1);
  if (dir == Cardinal::kEast) score += 0;
  else if (dir == Cardinal::kSouth) score += 1;
  else if (dir == Cardinal::kWest) score += 2;
  else if (dir == Cardinal::kNorth) score += 3;
  else return Error("Bad dir");

  return IntReturn(score);
}

}  // namespace advent_of_code
