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
  CanMoveBFS(const CharBoard* b, Point cur, Point end,
             std::vector<Point>* path_out)
      : b_(b), cur_(cur), end_(end), path_out_(path_out) {
    path_.push_back(cur_);
  }

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
      if (!b_->OnBoard(cur_ + d)) continue;
      if (!kMovable.contains((*b_)[cur_ + d])) continue;
      CanMoveBFS next = *this;
      next.cur_ += d;
      next.path_.push_back(next.cur_);
      state->AddNextStep(next);
    }
  }

 private:
  const CharBoard* b_;
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
      std::vector<Point>& path = ret[std::make_pair(from, to)];
      if (!CanMoveBFS(&b, from, to, &path).FindMinSteps()) {
        return absl::NotFoundError("No path found");
      }
      if (path[0] != from) {
        return absl::InternalError(absl::StrFormat("%v != %v", path[0], from));
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

  Point Destination(
      const absl::flat_hash_map<char, std::vector<Point>>& destinations,
      const CharBoard& b) const {
    auto it = destinations.find(c);
    CHECK(it != destinations.end());
    for (Point p : it->second) {
      if (b[p] != c) return p;
    }
    LOG(FATAL) << "No destination found: " << *this << "\n" << b;
  }

  std::optional<int> CanMove(Point dest, const CharBoard& b,
                             const AllPathsMap& all_paths) const {
    auto it = all_paths.find(std::make_pair(cur, dest));
    if (it == all_paths.end()) {
      VLOG(1) << cur;
      VLOG(1) << dest;
      LOG(FATAL) << "Could not find path";
    }
    const std::vector<Point>& path = it->second;
    for (Point p : path) {
      if (p == cur) {
        CHECK_EQ(b[p], c);
      } else {
        if (b[p] != '.') return {};
      }
    }
    return (path.size() - 1) * cost();
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

struct State {
  CharBoard board;
  std::vector<Actor> actors;
  int cost = 0;

  bool operator==(const State& o) const {
    return actors == o.actors && cost == o.cost;
  }
  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.actors, s.cost);
  }

  bool IsFinal() const {
    for (const auto& a : actors) {
      if (!a.done) return false;
    }
    return true;
  }

  State MoveActor(int actor_idx, Point to, int cost, bool done) const {
    State new_state = *this;
    CHECK_LT(actor_idx, actors.size());
    CHECK_GE(actor_idx, 0);
    new_state.board[new_state.actors[actor_idx].cur] = '.';
    new_state.board[to] = new_state.actors[actor_idx].c;
    new_state.actors[actor_idx].cur = to;
    if (done) {
      new_state.actors[actor_idx].done = true;
    } else {
      CHECK(!new_state.actors[actor_idx].moved);
      new_state.actors[actor_idx].moved = true;
    }
    new_state.cost += cost;
    return new_state;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const State& s) {
    absl::Format(&sink, "Cost=%d; Board:\n%v\nActors:\n%s", s.cost, s.board,
                 absl::StrJoin(s.actors, "\n"));
  }
};

std::optional<int> FindMinCost(
    State initial_state, const std::vector<Point>& empty,
    const absl::flat_hash_map<char, std::vector<Point>>& destinations,
    const AllPathsMap& all_paths) {
  VLOG(1) << "Start: " << initial_state;
  // We use a `node_hash_set` so that `history` can be the owning storage
  // for the queue and use pointers to not 2x storage and copies.
  absl::node_hash_set<State> history = {initial_state};
  std::optional<int> best;
  for (std::deque<const State*> queue = {&initial_state}; !queue.empty();
       history.erase(*queue.front()), queue.pop_front()) {
    const State& cur = *queue.front();
    if (cur.IsFinal()) {
      if (!best || *best > cur.cost) {
        VLOG(1) << "Found better final cost: " << cur.cost;
        VLOG(2) << cur;
        best = cur.cost;
      }
      continue;
    }
    // Fork off test paths for the next best action of each actor.
    for (int i = 0; i < cur.actors.size(); ++i) {
      if (cur.actors[i].done) continue;
      // Can we go to the final location?
      Point p = cur.actors[i].Destination(destinations, cur.board);
      std::optional<int> cost = cur.actors[i].CanMove(p, cur.board, all_paths);
      if (cost) {
        if (best && cur.cost + *cost >= *best) continue;
        // We can reach the final destination. Go there and stop.
        const auto [it, inserted] =
            history.insert(cur.MoveActor(i, p, *cost, /*done=*/true));
        if (!inserted) continue;
        VLOG(3) << cur << " => " << *it;
        queue.push_back(&*it);
        continue;
      }
      // If we have alread moved, we can't move again.
      if (cur.actors[i].moved) continue;

      // If we haven't moved, and we can't get to the final destionation,
      // fork off test paths for each candidate temporary location.
      for (Point p : empty) {
        std::optional<int> cost =
            cur.actors[i].CanMove(p, cur.board, all_paths);
        if (cost) {
          if (best && cur.cost + *cost >= *best) continue;
          const auto [it, inserted] =
              history.insert(cur.MoveActor(i, p, *cost, /*done=*/false));
          if (!inserted) continue;
          VLOG(3) << cur << " => " << *it;
          queue.push_back(&*it);
        }
      }
    }
  }
  return best;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_23::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));

  State s{.board = b};

  // Find the initial locations of actors and the temporary places in which to
  // store them.
  std::vector<Point> empty;
  std::vector<Point> srcs;
  for (const auto [p, c] : b) {
    if (c >= 'A' && c <= 'D') {
      s.actors.push_back({.c = c, .cur = p});
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
  absl::flat_hash_map<char, std::vector<Point>> destinations;
  auto it = srcs.begin();
  for (char c : {'A', 'B', 'C', 'D'}) {
    for (int i = 0; i < srcs.size() / 4; ++i) {
      destinations[c].push_back(*it);
      ++it;
    }
  }
  if (it != srcs.end()) return Error("Bad destination creation");

  // Identify which actors are already in their final destination.
  for (const auto& [c, v] : destinations) {
    for (Point test : v) {
      if (b[test] != c) break;
      bool found = false;
      for (auto& a : s.actors) {
        if (a.cur == test) {
          if (found) return Error("Double found");
          if (a.c != c) return Error("Bad A");
          found = true;
          a.done = true;
        }
      }
    }
  }

  return AdventReturn(
      FindMinCost(std::move(s), empty, destinations, all_paths));
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
    /*year=*/2021, /*day=*/23, []() {
  return std::unique_ptr<AdventDay>(new Day_2021_23());
});

}  // namespace advent_of_code
