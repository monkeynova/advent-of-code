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

struct PointAndDir {
  Point p;
  Point d;

  PointAndDir Advance() const {
    return {.p = p + d, .d = d};
  }

  PointAndDir rotate_right() const {
    return {.p = p, .d = d.rotate_right()};
  }

  PointAndDir rotate_left() const {
    return {.p = p, .d = d.rotate_left()};
  }

  template <typename H>
  friend H AbslHashValue(H h, const PointAndDir& pd) {
    return H::combine(std::move(h), pd.p, pd.d);
  }

  bool operator==(const PointAndDir& o) const {
    return p == o.p && d == o.d;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const PointAndDir& pd) {
    absl::Format(&sink, "%v (@%v)", pd.p, pd.d);
  }
};

using StitchMap = absl::flat_hash_map<PointAndDir, PointAndDir>;

void Stitch(StitchMap& ret, Point s1, Point s2, Point d1, Point e1, Point e2, Point d2) {
  CHECK_EQ((s2 - s1).dist(), (e2 - e1).dist()) << s1 << "-" << s2 << "; " << e1 << "-" << e2;
  Point ds = (s2 - s1).min_step();
  Point de = (e2 - e1).min_step();

  VLOG(1) << "Stitch: " << s1 << "-" << s2 << " (@" << d1 << ")" << " to " << e1 << "-" << e2 << " (@" << d2 << ")";

  for (Point s = s1, e = e1; true; s += ds, e += de) {
    ret[{.p = s, .d = d1}] = {.p = e, .d = d2};
    ret[{.p = e, .d = -d2}] = {.p = s, .d = -d1};
    VLOG(2) << s << "," << d1 << " => " << e << "," << d2;
    VLOG(2) << e << "," << -d2 << " => " << s << "," << -d1;
    if (s == s2) break;
  }
}

struct Transform {
  Point3 x_dest = Cardinal3::kXHat;
  Point3 y_dest = Cardinal3::kYHat;
  Point3 z_dest = Cardinal3::kZHat;

  Point3 Apply(Point3 p) const {
    return x_dest * p.x + y_dest * p.y + z_dest * p.z;
  }
};

struct StitchFace {
  Point board_coord;
  Transform to_cube;
};

StitchMap BuildStitchMap(const CharBoard& board) {
  int tile_width = std::gcd(board.width(), board.height());

  std::optional<Point> face_start;
  for (int y = 0; y < board.width(); y += tile_width) {
    for (int x = 0; x < board.width(); x += tile_width) {
      if (face_start) break;
      if (board[{x, y}] == ' ') continue;
      face_start = Point{x, y};
    }
  }

  absl::flat_hash_map<Point3, std::vector<PointAndDir>> corner_maps;

  absl::flat_hash_set<Point> hist = {*face_start};
  auto try_dir = [&](Point p, Point d) -> std::optional<Point> {
    Point next_face = p + tile_width * d;
    if (!board.OnBoard(next_face)) return std::nullopt;
    if (board[next_face] == ' ') return std::nullopt;
    if (hist.contains(next_face)) return std::nullopt;
    hist.insert(next_face);
    return next_face;
  };

  std::vector<Point3> paint_corners = {
    {-1, -1, 1}, {1, -1, 1}, {-1, 1, 1}, {1, 1, 1}
  };
  for (std::deque<StitchFace> queue = {{*face_start, Transform()}};
       !queue.empty(); queue.pop_front()) {
    Point on_board = queue.front().board_coord;
    Transform to_cube = queue.front().to_cube;
    std::vector<Point> face_corners = {
      on_board,
      on_board + (tile_width - 1) * Cardinal::kXHat,
      on_board + (tile_width - 1) * Cardinal::kYHat,
      on_board + (tile_width - 1) * (Cardinal::kXHat + Cardinal::kYHat),
    };
    std::vector<Point> face_dirs = {
      Cardinal::kNorth, Cardinal::kWest,
      Cardinal::kNorth, Cardinal::kEast,
      Cardinal::kWest, Cardinal::kSouth,
      Cardinal::kEast, Cardinal::kSouth,
    };
    for (int i = 0; i < face_corners.size(); ++i) {
      Point3 to_paint = to_cube.Apply(paint_corners[i]);
      corner_maps[to_paint].push_back({.p = face_corners[i], .d = face_dirs[2 * i]});
      corner_maps[to_paint].push_back({.p = face_corners[i], .d = face_dirs[2 * i + 1]});
      VLOG(1) << "Paint " << face_corners[i] << " on " << to_paint;
    }

    if (std::optional<Point> next_face = try_dir(on_board, Cardinal::kXHat)) {
      Transform new_t {
        .x_dest = -to_cube.z_dest,
        .y_dest = to_cube.y_dest,
        .z_dest = to_cube.x_dest,
      };
      queue.push_back({*next_face, new_t});
    }
    if (std::optional<Point> next_face = try_dir(on_board, -Cardinal::kXHat)) {
      Transform new_t {
        .x_dest = to_cube.z_dest,
        .y_dest = to_cube.y_dest,
        .z_dest = -to_cube.x_dest,
      };
      queue.push_back({*next_face, new_t});
    }
    if (std::optional<Point> next_face = try_dir(on_board, Cardinal::kYHat)) {
      Transform new_t {
        .x_dest = to_cube.x_dest,
        .y_dest = -to_cube.z_dest,
        .z_dest = to_cube.y_dest,
      };
      queue.push_back({*next_face, new_t});
    }
    if (std::optional<Point> next_face = try_dir(on_board, -Cardinal::kXHat)) {
      Transform new_t {
        .x_dest = to_cube.x_dest,
        .y_dest = to_cube.z_dest,
        .z_dest = -to_cube.y_dest,
      };
      queue.push_back({*next_face, new_t});
    }
  }

  for (auto [p3, p2v] : corner_maps) {
    CHECK_EQ(p2v.size(), 6);
  }

  std::vector<std::pair<Point3, Point3>> cube_edges = {
    // Top.
    {{-1, -1, 1}, {1, -1, 1}},
    {{1, -1, 1}, {1, 1, 1}},
    {{1, 1, 1}, {-1, 1, 1}},
    {{-1, 1, 1}, {-1, -1, 1}},

    // Bottom.
    {{1, -1, -1}, {-1, -1, -1}},
    {{1, 1, -1}, {1, -1, -1}},
    {{-1, 1, -1}, {1, 1, -1}},
    {{-1, -1, -1}, {-1, 1, -1}},

    // Struts.
    {{-1, -1, -1}, {-1, -1, 1}},
    {{-1, 1, -1}, {-1, 1, 1}},
    {{1, -1, -1}, {1, -1, 1}},
    {{1, 1, -1}, {1, 1, 1}},
  };

  StitchMap ret;
  auto same_face = [&](std::vector<PointAndDir>& set, PointAndDir f) {
    std::optional<PointAndDir> ret;
    for (PointAndDir pd : set) {
      if (f.d != pd.d) continue;
      if (f.p.x / tile_width == pd.p.x / tile_width &&
          f.p.y / tile_width == pd.p.y / tile_width) {
        CHECK(!ret);
        ret = pd;
      }
    }
    return ret;
  };

  for (auto [s, e] : cube_edges) {
    CHECK(corner_maps.contains(s));
    CHECK(corner_maps.contains(e));
    std::vector<PointAndDir> s_set = corner_maps[s];
    std::vector<PointAndDir> e_set = corner_maps[e];
    int stitch_count = 0;
    for (int i = 0; i < 6; ++i) {
      PointAndDir s1 = s_set[i];
      std::optional<PointAndDir> e1 = same_face(e_set, s1);
      if (!e1) continue;
      for (int j = i + 1; j < 6; ++j) {
        PointAndDir s2 = s_set[j];
        std::optional<PointAndDir> e2 = same_face(e_set, s2);
        if (!e2) continue;
        ++stitch_count;
        Stitch(ret, s1.p, e1->p, s1.d, s2.p, e2->p, -s2.d);
      }
    }
    CHECK_EQ(stitch_count, 1)
        << "s_set: " << absl::StrJoin(s_set, ",") << "; e_set: " << absl::StrJoin(e_set, ",");
  }
  return ret;
}

absl::StatusOr<int> Score(PointAndDir pd) {
  int score = 0;
  score += 1000 * (pd.p.y + 1) + 4 * (pd.p.x + 1);
  if (pd.d == Cardinal::kEast) score += 0;
  else if (pd.d == Cardinal::kSouth) score += 1;
  else if (pd.d == Cardinal::kWest) score += 2;
  else if (pd.d == Cardinal::kNorth) score += 3;
  else return Error("Bad dir");

  return score;
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

  PointAndDir cur = {.p = *start, .d = Cardinal::kEast};

  int dist = 0;

  auto move = [&]() {
    VLOG(2) << cur << "; move=" << dist;
      for (int i = 0; i < dist; ++i) {
        PointAndDir next = cur.Advance();
        next.p = board.TorusPoint(next.p);
        while (board[next.p] == ' ') {
          next = next.Advance();
          next.p = board.TorusPoint(next.p);
        }
        if (board[next.p] == '#') break;
        cur = next;
      }
    dist = 0;
    VLOG(2) << cur;
  };

  for (char c : path) {
    if (c >= '0' && c <= '9') dist = dist * 10 + c - '0';
    else if (c == 'R') {
      move();
      cur = cur.rotate_right();
    } else if (c == 'L') {
      move();
      cur = cur.rotate_left();
    } else {
      return Error("Bad path");
    }
  }
  move();

  return IntReturn(Score(cur));
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

  PointAndDir cur = {.p = *start, .d = Cardinal::kEast};
  int dist = 0;

  StitchMap stitched = BuildStitchMap(board);

  PointAndDir next;

  auto cube_move = [&]() {
    if (stitched.contains(cur)) {
      next = stitched[{cur.p, cur.d}];
      VLOG(2) << "Stitch: " << cur << "->" << next;
    } else {
      next = cur.Advance();
    }
    CHECK(board.OnBoard(next.p)) << next;
    CHECK(board[next.p] != ' ') << next;
  };

  auto move = [&]() {
    VLOG(2) << cur << " move: " << dist;
      for (int i = 0; i < dist; ++i) {
        cube_move();
        if (board[next.p] == '#') break;
        cur = next;
      }
    dist = 0;
  };

  for (char c : path) {
    if (c >= '0' && c <= '9') dist = dist * 10 + c - '0';
    else if (c == 'R') {
      move();
      cur = cur.rotate_right();
    } else if (c == 'L') {
      move();
      cur = cur.rotate_left();
    } else {
      return Error("Bad path");
    }
  }
  move();

  return IntReturn(Score(cur));
}

}  // namespace advent_of_code
