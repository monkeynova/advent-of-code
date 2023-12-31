#include "advent_of_code/2021/day23/day23.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/node_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class CanMoveBFS : public BFSInterface<CanMoveBFS, Point> {
 public:
  CanMoveBFS(const CharBoard& b, Point cur, Point end,
             std::vector<Point>* path_out)
      : b_(b), cur_(cur), end_(end), path_out_(path_out) {}

  Point identifier() const override { return cur_; }

  bool IsFinal() const override {
    if (cur_ == end_) {
      *path_out_ = path_;
    }
    return cur_ == end_;
  }

  int min_steps_to_final() const override { return (cur_ - end_).dist(); }

  void AddNextSteps(State* state) const override {
    const absl::flat_hash_set<char> kMovable = {'.', 'A', 'B', 'C', 'D'};
    for (Point d : Cardinal::kFourDirs) {
      if (!b_.OnBoard(cur_ + d)) continue;
      if (!kMovable.contains((b_)[cur_ + d])) continue;
      CanMoveBFS next = *this;
      next.cur_ += d;
      next.path_.push_back(next.cur_);
      state->AddNextStep(next);
    }
  }

 private:
  const CharBoard& b_;
  Point cur_;
  Point end_;
  std::vector<Point>* path_out_;
  std::vector<Point> path_;
};

using AllPathsMap =
    absl::flat_hash_map<std::pair<Point, Point>, std::vector<Point>>;

absl::StatusOr<AllPathsMap> ComputeAllPaths(
    const CharBoard& b, const std::vector<Point>& point_list) {
  AllPathsMap ret;
  for (Point from : point_list) {
    for (Point to : point_list) {
      if (from == to) continue;
      std::vector<Point>& path = ret[{from, to}];
      if (!CanMoveBFS(b, from, to, &path).FindMinSteps()) {
        return absl::NotFoundError("No path found");
      }
      if (path.back() != to) {
        return absl::InternalError(
            absl::StrFormat("%v != %v", path.back(), to));
      }
    }
  }

  return ret;
}

struct Actor {
  char c;
  Point cur;
  bool moved = false;
  bool done = false;

  bool operator==(const Actor& o) const {
    return c == o.c && cur == o.cur && moved == o.moved && done == o.done;
  }
  template <typename H>
  friend H AbslHashValue(H h, const Actor& a) {
    return H::combine(std::move(h), a.c, a.cur, a.moved, a.done);
  }
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Actor& a) {
    absl::Format(&sink, "%c: %v %v %v", a.c, a.cur, a.moved, a.done);
  }

  int cost() const {
    switch (c) {
      case 'A':
        return 1;
      case 'B':
        return 10;
      case 'C':
        return 100;
      case 'D':
        return 1000;
    }
    LOG(FATAL) << "Bad char";
  }
};

struct Stats {
  int calls = 0;
  int memo = 0;
  int budget_skips = 0;
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Stats& stats) {
    absl::Format(&sink, "calls:%d; memo=%d, budget_skips:%d", stats.calls,
                 stats.memo, stats.budget_skips);
  }
};

class State {
 public:
  explicit State(CharBoard board, std::vector<Actor> actors,
                 const std::vector<Point>& tmp_locations,
                 const std::vector<std::vector<Point>>& destinations,
                 const AllPathsMap& all_paths)
      : board_(std::move(board)),
        actors_(std::move(actors)),
        tmp_locations_(tmp_locations),
        destinations_(destinations),
        all_paths_(all_paths) {}

  const Stats& stats() const { return stats_; }

  bool IsFinal() const {
    return absl::c_all_of(actors_, [](const Actor& a) { return a.done; });
  }

  std::optional<int> CanMove(const Actor& actor, Point dest) const {
    auto it = all_paths_.find(std::make_pair(actor.cur, dest));
    if (it == all_paths_.end()) {
      VLOG(1) << actor.cur;
      VLOG(1) << dest;
      LOG(FATAL) << "Could not find path";
    }
    const std::vector<Point>& path = it->second;
    for (Point p : path) {
      if (p == actor.cur) {
        CHECK_EQ(board_[p], actor.c);
      } else {
        if (board_[p] != '.') return {};
      }
    }
    return path.size() * actor.cost();
  }

  Point Destination(const Actor& actor) const {
    for (Point p : destinations_[actor.c]) {
      if (board_[p] != actor.c) return p;
    }
    LOG(FATAL) << "No destination found: " << *this << "\n" << board_;
  }

  void MoveActor(int actor_idx, Point to, int cost, bool done) {
    CHECK_LT(actor_idx, actors_.size());
    CHECK_GE(actor_idx, 0);
    board_[actors_[actor_idx].cur] = '.';
    board_[to] = actors_[actor_idx].c;
    actors_[actor_idx].cur = to;
    if (done) {
      actors_[actor_idx].done = true;
    } else {
      CHECK(!actors_[actor_idx].moved);
      actors_[actor_idx].moved = true;
    }
  }

  void UnMoveActor(int actor_idx, Point from, int cost, bool done) {
    CHECK_LT(actor_idx, actors_.size());
    CHECK_GE(actor_idx, 0);
    board_[actors_[actor_idx].cur] = '.';
    board_[from] = actors_[actor_idx].c;
    actors_[actor_idx].cur = from;
    if (done) {
      actors_[actor_idx].done = false;
    } else {
      CHECK(actors_[actor_idx].moved);
      actors_[actor_idx].moved = false;
    }
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const State& s) {
    absl::Format(&sink, "Actors:\n%s", absl::StrJoin(s.actors_, "\n"));
  }

  std::optional<int> FindMinCostDFS(
      int budget = std::numeric_limits<int>::max());

 private:
  CharBoard board_;
  std::vector<Actor> actors_;
  const std::vector<Point>& tmp_locations_;
  const std::vector<std::vector<Point>>& destinations_;
  const AllPathsMap& all_paths_;
  Stats stats_;
  absl::flat_hash_map<std::vector<Actor>, std::optional<int>> memo_;
};

std::optional<int> State::FindMinCostDFS(int budget) {
  ++stats_.calls;
  if (IsFinal()) {
    return 0;
  }

  auto it = memo_.find(actors_);
  if (it != memo_.end()) {
    ++stats_.memo;
    return it->second;
  }

  std::optional<int> best;

  for (int i = 0; i < actors_.size(); ++i) {
    const Actor& actor = actors_[i];
    if (actor.done) continue;

    Point from = actor.cur;
    // Can we go to the final location?
    Point to = Destination(actor);
    std::optional<int> cost = CanMove(actor, to);
    if (cost) {
      if (*cost > budget) {
        ++stats_.budget_skips;
        continue;
      }
      // We can reach the final destination. Go there and stop.
      MoveActor(i, to, *cost, /*done=*/true);
      std::optional<int> sub = FindMinCostDFS(budget - *cost);
      if (sub) {
        sub = *sub + *cost;
        if (!best || *best > *sub) {
          best = sub;
          budget = *best;
        }
      }
      UnMoveActor(i, from, *cost, /*done=*/true);
      continue;
    }
    // If we have alread moved, we can't move again.
    if (actor.moved) continue;

    // If we haven't moved, and we can't get to the final destionation,
    // try each candidate temporary location.
    for (Point to : tmp_locations_) {
      std::optional<int> cost = CanMove(actor, to);
      if (cost) {
        if (*cost > budget) {
          ++stats_.budget_skips;
          continue;
        }
        MoveActor(i, to, *cost, /*done=*/false);
        std::optional<int> sub = FindMinCostDFS(budget - *cost);
        if (sub) {
          sub = *sub + *cost;
          if (!best || *best > *sub) {
            best = sub;
            budget = *best;
          }
        }
        UnMoveActor(i, from, *cost, /*done=*/false);
      }
    }
  }

  return memo_[actors_] = best;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_23::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));

  // Find the initial locations of actors and the temporary places in which to
  // store them.
  std::vector<Point> empty;
  std::vector<Point> srcs;
  std::vector<Actor> actors;
  for (const auto [p, c] : b) {
    if (c >= 'A' && c <= 'D') {
      actors.push_back({.c = c, .cur = p});
      srcs.push_back(p);
    }
    // p + south = '#' is the stupid test for not stopping at a hallway
    // entrance.
    if (c == '.' && b[p + Cardinal::kSouth] == '#') {
      empty.push_back(p);
    }
  };

  // Compute the paths from all points we might want to move an actor between.
  // These paths are unique (TODO(@monkeynova): verify this) so future checks
  // for motion between them only needs to check for blockages, rather than
  // finding routes.
  std::vector<Point> all_points;
  all_points.insert(all_points.end(), empty.begin(), empty.end());
  all_points.insert(all_points.end(), srcs.begin(), srcs.end());
  ASSIGN_OR_RETURN(AllPathsMap all_paths, ComputeAllPaths(b, all_points));

  // Identify the destination locations for each set of actors.
  if (srcs.size() % 4 != 0) return Error("Bad srcs");
  absl::c_sort(srcs, [](Point a, Point b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y > b.y;
  });
  std::vector<std::vector<Point>> destinations(128);
  auto it = srcs.begin();
  for (char c : {'A', 'B', 'C', 'D'}) {
    for (int i = 0; i < srcs.size() / 4; ++i) {
      destinations[c].push_back(*it);
      ++it;
    }
  }
  if (it != srcs.end()) return Error("Bad destination creation");

  // Identify which actors are already in their final destination.
  for (char c : {'A', 'B', 'C', 'D'}) {
    for (Point test : destinations[c]) {
      if (b[test] != c) break;
      bool found = false;
      for (auto& a : actors) {
        if (a.cur == test) {
          if (found) return Error("Double found");
          if (a.c != c) return Error("Bad A");
          found = true;
          a.done = true;
        }
      }
    }
  }

  absl::c_sort(actors, [](const Actor& a, const Actor& b) {
    return a.cost() < b.cost();
  });

  State s(std::move(b), std::move(actors), empty, destinations, all_paths);
  std::optional<int> ret = s.FindMinCostDFS();
  VLOG(1) << "stats = " << s.stats();
  return AdventReturn(ret);
}

absl::StatusOr<std::string> Day_2021_23::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 5) return Error("Bad input");
  const std::string insert1 = "  #D#C#B#A#  ";
  const std::string insert2 = "  #D#B#A#C#  ";
  std::vector<std::string_view> spliced = {
      input[0], input[1], input[2], insert1, insert2, input[3], input[4],
  };
  return Part1(absl::MakeSpan(spliced));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/23,
    []() { return std::unique_ptr<AdventDay>(new Day_2021_23()); });

}  // namespace advent_of_code
