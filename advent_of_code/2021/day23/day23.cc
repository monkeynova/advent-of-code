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
#include "advent_of_code/fast_board.h"
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

template <typename PointType>
struct ActorBase {
  char c;
  PointType cur;
  bool moved = false;
  bool done = false;

  bool operator==(const ActorBase& o) const {
    return c == o.c && cur == o.cur && moved == o.moved && done == o.done;
  }
  template <typename H>
  friend H AbslHashValue(H h, const ActorBase& a) {
    return H::combine(std::move(h), a.c, a.cur, a.moved, a.done);
  }
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const ActorBase& a) {
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

using Actor = ActorBase<Point>;
using FastActor = ActorBase<FastBoard::Point>;

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
        immutable_board_(board_),
        fb_(immutable_board_),
        all_paths_(fb_, FastBoard::PointMap<std::vector<FastBoard::Point>>(
                            fb_, std::vector<FastBoard::Point>{})) {
    for (const Actor& a : actors) {
      actors_.push_back(FastActor{
          .c = a.c, .cur = fb_.From(a.cur), .moved = a.moved, .done = a.done});
    }
    tmp_locations_ = FbFrom(tmp_locations);
    for (const auto& v : destinations) {
      destinations_.push_back(FbFrom(v));
    }
    for (const auto& [pair, v] : all_paths) {
      all_paths_.Get(fb_.From(pair.first)).Get(fb_.From(pair.second)) =
          FbFrom(v);
    }
  }

  const Stats& stats() const { return stats_; }

  bool IsFinal() const {
    return absl::c_all_of(actors_, [](const FastActor& a) { return a.done; });
  }

  std::optional<int> CanMove(const FastActor& actor,
                             FastBoard::Point dest) const {
    const std::vector<FastBoard::Point>& path =
        all_paths_.Get(actor.cur).Get(dest);
    for (FastBoard::Point p : path) {
      if (fb_[p] != '.') return {};
    }
    return path.size() * actor.cost();
  }

  FastBoard::Point Destination(const FastActor& actor) const {
    for (FastBoard::Point p : destinations_[actor.c]) {
      if (fb_[p] != actor.c) return p;
    }
    LOG(FATAL) << "No destination found: " << *this << "\n" << board_;
  }

  void MoveActor(FastActor& actor, FastBoard::Point to, bool done) {
    board_[fb_.To(actor.cur)] = '.';
    board_[fb_.To(to)] = actor.c;
    actor.cur = to;
    if (done) {
      actor.done = true;
    } else {
      CHECK(!actor.moved);
      actor.moved = true;
    }
  }

  void UnMoveActor(FastActor& actor, FastBoard::Point from, bool done) {
    board_[fb_.To(actor.cur)] = '.';
    board_[fb_.To(from)] = actor.c;
    actor.cur = from;
    if (done) {
      actor.done = false;
    } else {
      CHECK(actor.moved);
      actor.moved = false;
    }
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const State& s) {
    absl::Format(&sink, "Actors:\n%s", absl::StrJoin(s.actors_, "\n"));
  }

  std::optional<int> FindMinCostDFS(
      int budget = std::numeric_limits<int>::max());

 private:
  std::vector<FastBoard::Point> FbFrom(const std::vector<Point>& v) {
    std::vector<FastBoard::Point> ret;
    ret.reserve(v.size());
    for (Point p : v) ret.push_back(fb_.From(p));
    return ret;
  }

  CharBoard board_;
  ImmutableCharBoard immutable_board_;
  FastBoard fb_;
  std::vector<FastActor> actors_;
  std::vector<FastBoard::Point> tmp_locations_;
  std::vector<std::vector<FastBoard::Point>> destinations_;
  FastBoard::PointMap<FastBoard::PointMap<std::vector<FastBoard::Point>>>
      all_paths_;

  Stats stats_;
  absl::flat_hash_map<std::vector<FastActor>, std::optional<int>> memo_;
};

std::optional<int> State::FindMinCostDFS(int budget) {
  ++stats_.calls;
  if (IsFinal()) {
    return 0;
  }

  std::vector<FastActor> memo_actors = actors_;
  absl::c_sort(memo_actors, [](const FastActor& a, const FastActor& b) {
    if (a.c != b.c) return a.c < b.c;
    if (a.moved != b.moved) return a.moved < b.moved;
    if (a.done != b.done) return a.done < b.done;
    return a.cur < b.cur;
  });

  auto it = memo_.find(memo_actors);
  if (it != memo_.end()) {
    ++stats_.memo;
    return it->second;
  }

  std::optional<int> best;

  for (FastActor& actor : actors_) {
    if (actor.done) continue;

    FastBoard::Point from = actor.cur;
    // Can we go to the final location?
    FastBoard::Point to = Destination(actor);
    std::optional<int> cost = CanMove(actor, to);
    if (cost) {
      if (*cost > budget) {
        ++stats_.budget_skips;
        continue;
      }
      // We can reach the final destination. Go there and stop.
      MoveActor(actor, to, /*done=*/true);
      std::optional<int> sub = FindMinCostDFS(budget - *cost);
      if (sub) {
        sub = *sub + *cost;
        if (!best || *best > *sub) {
          best = sub;
          budget = *best;
        }
      }
      UnMoveActor(actor, from, /*done=*/true);
      continue;
    }
    // If we have alread moved, we can't move again.
    if (actor.moved) continue;

    // If we haven't moved, and we can't get to the final destionation,
    // try each candidate temporary location.
    for (FastBoard::Point to : tmp_locations_) {
      std::optional<int> cost = CanMove(actor, to);
      if (cost) {
        if (*cost > budget) {
          ++stats_.budget_skips;
          continue;
        }
        MoveActor(actor, to, /*done=*/false);
        std::optional<int> sub = FindMinCostDFS(budget - *cost);
        if (sub) {
          sub = *sub + *cost;
          if (!best || *best > *sub) {
            best = sub;
            budget = *best;
          }
        }
        UnMoveActor(actor, from, /*done=*/false);
      }
    }
  }

  return memo_[memo_actors] = best;
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
