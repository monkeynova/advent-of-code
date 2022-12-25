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
#include "advent_of_code/point3.h"
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

class StitchMap {
 public:
  StitchMap() = default;

  void Add(Point s1, Point s2, Point d1, Point e1, Point e2, Point d2);
  std::optional<PointAndDir> Find(PointAndDir src) const {
    if (auto it = map_.find(src); it != map_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

 private:
  absl::flat_hash_map<PointAndDir, PointAndDir> map_;
};

void StitchMap::Add(Point s1, Point s2, Point d1,
                    Point e1, Point e2, Point d2) {
  CHECK_EQ((s2 - s1).dist(), (e2 - e1).dist())
      << s1 << "-" << s2 << "; " << e1 << "-" << e2;
  Point ds = (s2 - s1).min_step();
  Point de = (e2 - e1).min_step();

  VLOG(1) << "Stitch: " << s1 << "-" << s2 << " (@" << d1 << ")" << " to "
          << e1 << "-" << e2 << " (@" << d2 << ")";

  for (Point s = s1, e = e1; true; s += ds, e += de) {
    map_[{.p = s, .d = d1}] = {.p = e, .d = d2};
    map_[{.p = e, .d = -d2}] = {.p = s, .d = -d1};
    VLOG(2) << s << "," << d1 << " => " << e << "," << d2;
    VLOG(2) << e << "," << -d2 << " => " << s << "," << -d1;
    if (s == s2) break;
  }
}

struct StitchFace {
  Point board_coord;
  Orientation3 to_cube;
};

struct PaintCorner {
  Point3 cube;
  Point board;
  Point board_out1;
  Point board_out2;

  void Paint(
      absl::flat_hash_map<Point3, std::vector<PointAndDir>>& corner_maps,
      const StitchFace& stitch_face, int tile_width) const {
    Point test = stitch_face.board_coord + board * (tile_width - 1);
    Point3 to_paint = stitch_face.to_cube.Apply(cube);
    corner_maps[to_paint].push_back({.p = test, .d = board_out1});
    corner_maps[to_paint].push_back({.p = test, .d = board_out2});
    VLOG(1) << "Paint " << test << " on " << to_paint;
  }
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

  // Map a 3d cube corner to the set of square corners on the board along with
  // an 'outward' facing direction. This means each corner is added twice (once
  // for each 'outward' direction).
  absl::flat_hash_map<Point3, std::vector<PointAndDir>> corner_maps;

  const std::array<PaintCorner, 4> kPaintCorners = {
    PaintCorner{.cube = {-1, -1, 1}, .board = Cardinal::kOrigin,
                .board_out1 = Cardinal::kNorth, .board_out2 = Cardinal::kWest},
    PaintCorner{.cube = {1, -1, 1}, .board = Cardinal::kEast,
                .board_out1 = Cardinal::kNorth, .board_out2 = Cardinal::kEast},
    PaintCorner{.cube = {-1, 1, 1}, .board = Cardinal::kSouth,
                .board_out1 = Cardinal::kWest, .board_out2 = Cardinal::kSouth},
    PaintCorner{.cube = {1, 1, 1}, .board = Cardinal::kSouthEast,
                .board_out1 = Cardinal::kEast, .board_out2 = Cardinal::kSouth},
  };

  const absl::flat_hash_map<Point, Orientation3> kRotations = {
    {Cardinal::kXHat, Orientation3(-Cardinal3::kZHat, Cardinal3::kYHat)},
    {-Cardinal::kXHat, Orientation3(Cardinal3::kZHat, Cardinal3::kYHat)},
    {Cardinal::kYHat, Orientation3(Cardinal3::kXHat, -Cardinal3::kZHat)},
    {-Cardinal::kYHat, Orientation3(Cardinal3::kXHat, Cardinal3::kZHat)},
  };

  absl::flat_hash_set<Point> hist = {*face_start};
  for (std::deque<StitchFace> queue = {{*face_start, Orientation3()}};
       !queue.empty(); queue.pop_front()) {
    const StitchFace& stitch_face = queue.front();
    for (const PaintCorner& corner : kPaintCorners) {
      corner.Paint(corner_maps, stitch_face, tile_width);
    }

    for (Point dir : Cardinal::kFourDirs) {
      Point next_face = stitch_face.board_coord + tile_width * dir;
      if (!board.OnBoard(next_face)) continue;
      if (board[next_face] == ' ') continue;
      if (hist.contains(next_face)) continue;
      hist.insert(next_face);
      auto it = kRotations.find(dir);
      CHECK(it != kRotations.end());
      queue.push_back({next_face, stitch_face.to_cube.Apply(it->second)});
    }
  }

  // Every corner should have 3 faces attached, each face attached in two
  // directions.
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

  // Find the entry in `set` on the same face with the same direction as `f`.
  auto same_edge = [&](std::vector<PointAndDir>& set, PointAndDir f) {
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

  StitchMap stitch_map;
  for (auto [s, e] : cube_edges) {
    CHECK(corner_maps.contains(s));
    CHECK(corner_maps.contains(e));
    std::vector<PointAndDir> s_set = corner_maps[s];
    std::vector<PointAndDir> e_set = corner_maps[e];
    int stitch_count = 0;
    for (int i = 0; i < 6; ++i) {
      PointAndDir s1 = s_set[i];
      std::optional<PointAndDir> e1 = same_edge(e_set, s1);
      if (!e1) continue;
      for (int j = i + 1; j < 6; ++j) {
        PointAndDir s2 = s_set[j];
        std::optional<PointAndDir> e2 = same_edge(e_set, s2);
        if (!e2) continue;
        ++stitch_count;
        stitch_map.Add(s1.p, e1->p, s1.d, s2.p, e2->p, -s2.d);
      }
    }
    CHECK_EQ(stitch_count, 1)
        << "s_set: " << absl::StrJoin(s_set, ",") << "; "
        << "e_set: " << absl::StrJoin(e_set, ",");
  }
  return stitch_map;
}

absl::StatusOr<CharBoard> PadAndParseBoard(
    absl::Span<absl::string_view> input, std::vector<std::string>& storage) {
  int max = 0;
  for (absl::string_view line : input) {
    max = std::max<int>(max, line.size());
  }
  for (absl::string_view line : input) {
    storage.push_back(std::string(line));
    storage.back().resize(max, ' ');
  }

  return CharBoard::Parse(storage);
}

absl::StatusOr<PointAndDir> FindStart(const CharBoard& board) {
  std::optional<Point> start;
  for (Point p : board.range()) {
    if (board[p] == '.') {
      start = p;
      break;
    }
  }
  if (!start) return Error("No start");
  return PointAndDir{.p = *start, .d = Cardinal::kEast};
}

absl::StatusOr<PointAndDir> MovePath(
  absl::string_view path, PointAndDir cur,
  absl::FunctionRef<PointAndDir(PointAndDir, int)> move) {
  int dist = 0;
  for (char c : path) {
    if (c >= '0' && c <= '9') dist = dist * 10 + c - '0';
    else if (c == 'R') {
      cur = move(cur, dist);
      dist = 0;
      cur = cur.rotate_right();
    } else if (c == 'L') {
      cur = move(cur, dist);
      dist = 0;
      cur = cur.rotate_left();
    } else {
      return Error("Bad path");
    }
  }
  return move(cur, dist);
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
  if (input.size() < 2) return Error("Bad input");
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::string_view path = input.back();
  input = input.subspan(0, input.size() - 2);
  
  std::vector<std::string> storage;
  absl::StatusOr<CharBoard> board = PadAndParseBoard(input, storage);

  auto move = [&board](PointAndDir cur, int dist) {
    VLOG(2) << cur << "; move=" << dist;
    for (int i = 0; i < dist; ++i) {
      PointAndDir next = cur.Advance();
      next.p = board->TorusPoint(next.p);
      while ((*board)[next.p] == ' ') {
        next = next.Advance();
        next.p = board->TorusPoint(next.p);
      }
      if ((*board)[next.p] == '#') break;
      cur = next;
    }
    VLOG(2) << cur;
    return cur;
  };

  absl::StatusOr<PointAndDir> start = FindStart(*board);
  if (!start.ok()) return start.status();
  absl::StatusOr<PointAndDir> end = MovePath(path, *start, move);
  if (!end.ok()) return end.status();
  return AdventReturn(Score(*end));
}

absl::StatusOr<std::string> Day_2022_22::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 2) return Error("Bad input");
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::string_view path = input.back();
  input = input.subspan(0, input.size() - 2);
  
  std::vector<std::string> storage;
  absl::StatusOr<CharBoard> board = PadAndParseBoard(input, storage);
  if (!board.ok()) return board.status();

  StitchMap stitch_map = BuildStitchMap(*board);

  auto move = [&board, &stitch_map](PointAndDir cur, int dist) {
    VLOG(2) << cur << " move: " << dist;
    for (int i = 0; i < dist; ++i) {
      std::optional<PointAndDir> next = stitch_map.Find(cur);
      if (!next) {
        next = cur.Advance();
      }
      CHECK(board->OnBoard(next->p)) << *next;
      CHECK((*board)[next->p] != ' ') << *next;
      if ((*board)[next->p] == '#') break;
      cur = *next;
    }
    return cur;
  };

  absl::StatusOr<PointAndDir> start = FindStart(*board);
  if (!start.ok()) return start.status();
  absl::StatusOr<PointAndDir> end = MovePath(path, *start, move);
  if (!end.ok()) return end.status();
  return AdventReturn(Score(*end));
}

}  // namespace advent_of_code
