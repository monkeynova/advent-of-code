// http://adventofcode.com/2022/day/24

#include "advent_of_code/2022/day24/day24.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct BlizzardSet {
  absl::flat_hash_set<Point> up;
  absl::flat_hash_set<Point> down;
  absl::flat_hash_set<Point> left;
  absl::flat_hash_set<Point> right;

  bool AnyOn(Point p) const {
    if (up.contains(p)) return true;
    if (down.contains(p)) return true;
    if (left.contains(p)) return true;
    if (right.contains(p)) return true;
    return false;
  }

  CharBoard Draw(const CharBoard& board) const {
    CharBoard draw = board;
    for (Point p : up) draw[p] = '^';
    for (Point p : down) draw[p] = 'v';
    for (Point p : left) draw[p] = '<';
    for (Point p : right) draw[p] = '>';
    return draw;
  }

  BlizzardSet Next(const CharBoard& board) const {
    BlizzardSet next;
    for (Point p : up) {
      Point np = board.TorusPoint(p + Cardinal::kNorth);
      while (board[np] == '#') {
        np = board.TorusPoint(np + Cardinal::kNorth);
      }
      next.up.insert(np);
    }
    for (Point p : down) {
      Point np = board.TorusPoint(p + Cardinal::kSouth);
      while (board[np] == '#') {
        np = board.TorusPoint(np + Cardinal::kSouth);
      }
      next.down.insert(np);
    }
    for (Point p : left) {
      Point np = board.TorusPoint(p + Cardinal::kWest);
      while (board[np] == '#') {
        np = board.TorusPoint(np + Cardinal::kWest);
      }
      next.left.insert(np);
    }
    for (Point p : right) {
      Point np = board.TorusPoint(p + Cardinal::kEast);
      while (board[np] == '#') {
        np = board.TorusPoint(np + Cardinal::kEast);
      }
      next.right.insert(np);
    }
    VLOG(2) << "Wind:\n" << next.Draw(board);
    return next;
  }
};

struct BlizzardHistory {
  std::vector<BlizzardSet> sets;

  void BuildNext(const CharBoard& board) {
    sets.push_back(sets.back().Next(board));
  }
};

// We use a Point3 for state because the world changes with each step, so we
// use the step # as a z coordinate to consider {x,y},step1 distinct from
// {x,y},step2.
class BFSWalk : public BFSInterface<BFSWalk, Point3> { 
 public:
  BFSWalk(Point start, Point end, const CharBoard& b, BlizzardHistory& h)
   : cur_({start.x, start.y, 0}), end_(end), b_(b), h_(h) {}

  Point3 identifier() const override { return cur_; }

  Point cur() const { return Point{cur_.x, cur_.y}; }

  bool IsFinal() const override { return cur() == end_; }
  void AddNextSteps(State* state) const override {
    while (h_.sets.size() <= num_steps() + 1) {
      h_.BuildNext(b_);
    }
    const BlizzardSet& blizzards = h_.sets[num_steps() + 1];
    if (!blizzards.AnyOn(cur())) {
      // Wait on.
      VLOG(2) << num_steps() << ": " << cur_ << " (wait)";
      BFSWalk next = *this;
      ++next.cur_.z;
      state->AddNextStep(next);
    }
    for (Point d : Cardinal::kFourDirs) {
      Point t = cur() + d;
      if (!b_.OnBoard(t)) continue;
      if (b_[t] == '#') continue;
      if (blizzards.AnyOn(t)) continue;
      VLOG(2) << num_steps() << ": " << cur_ << " -> " << t;
      BFSWalk next = *this;
      next.cur_ = {t.x, t.y, num_steps() + 1};
      state->AddNextStep(next);
    }
  }

 private:
  Point3 cur_;
  Point end_;
  const CharBoard& b_;
  BlizzardHistory& h_;
};

struct Start {
  std::optional<Point> start;
  std::optional<Point> end;
  BlizzardSet blizzards;
};

Start ParseAndClearBoard(CharBoard& board) {
  Start ret;
  for (Point p : board.range()) {
    switch (board[p]) {
      case '^': {
        ret.blizzards.up.insert(p);
        board[p] = '.';
        break;
      }
      case 'v': {
        ret.blizzards.down.insert(p);
        board[p] = '.';
        break;
      }
      case '<': {
        ret.blizzards.left.insert(p);
        board[p] = '.';
        break;
      }
      case '>': {
        ret.blizzards.right.insert(p);
        board[p] = '.';
        break;
      }
      case '.': {
        if (p.y == 0) ret.start = p;
        if (p.y == board.height() - 1) ret.end = p;
      }
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  Start start = ParseAndClearBoard(*board);
  if (!start.start) return Error("No start");
  if (!start.end) return Error("No end");
  VLOG(1) << "Route: " << *start.start << " -> " << *start.end;

  BlizzardHistory hist;
  hist.sets.push_back(std::move(start.blizzards));

  return IntReturn(
      BFSWalk(*start.start, *start.end, *board, hist).FindMinSteps());
}

absl::StatusOr<std::string> Day_2022_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> board = ParseAsBoard(input);
  if (!board.ok()) return board.status();

  Start start = ParseAndClearBoard(*board);
  if (!start.start) return Error("No start");
  if (!start.end) return Error("No end");
  VLOG(1) << "Route: " << *start.start << " -> " << *start.end;

  BlizzardHistory hist;
  hist.sets.push_back(std::move(start.blizzards));
  std::optional<int> stage_1 =
      BFSWalk(*start.start, *start.end, *board, hist).FindMinSteps();
  if (!stage_1) return Error("stage 1 not found");

  BlizzardHistory hist2;
  hist2.sets.push_back(hist.sets.back());
  std::optional<int> stage_2 =
      BFSWalk(*start.end, *start.start, *board, hist2).FindMinSteps();
  if (!stage_2) return Error("stage 2 not found");

  BlizzardHistory hist3;
  hist3.sets.push_back(hist2.sets.back());
  std::optional<int> stage_3 =
      BFSWalk(*start.start, *start.end, *board, hist3).FindMinSteps();
  if (!stage_3) return Error("stage 3 not found");

  return IntReturn(*stage_1 + *stage_2 + *stage_3);
}

}  // namespace advent_of_code
