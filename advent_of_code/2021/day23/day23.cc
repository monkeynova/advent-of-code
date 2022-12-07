// https://adventofcode.com/2021/day/23
//
// --- Day 23: Amphipod ---
// ------------------------
// 
// A group of amphipods notice your fancy submarine and flag you down.
// "With such an impressive shell," one amphipod says, "surely you can
// help us with a question that has stumped our best scientists."
// 
// They go on to explain that a group of timid, stubborn amphipods live
// in a nearby burrow. Four types of amphipods live there: Amber (A),
// Bronze (B), Copper (C), and Desert (D). They live in a burrow that
// consists of a hallway and four side rooms. The side rooms are
// initially full of amphipods, and the hallway is initially empty.
// 
// They give you a diagram of the situation (your puzzle input),
// including locations of each amphipod (A, B, C, or D, each of which is
// occupying an otherwise open space), walls (#), and open space (.).
// 
// For example:
// 
// #############
// #...........#
// ###B#C#B#D###
// #A#D#C#A#
// #########
// 
// The amphipods would like a method to organize every amphipod into side
// rooms so that each side room contains one type of amphipod and the
// types are sorted A-D going left to right, like this:
// 
// #############
// #...........#
// ###A#B#C#D###
// #A#B#C#D#
// #########
// 
// Amphipods can move up, down, left, or right so long as they are moving
// into an unoccupied open space. Each type of amphipod requires a
// different amount of energy to move one step: Amber amphipods require 1
// energy per step, Bronze amphipods require 10 energy, Copper amphipods
// require 100, and Desert ones require 1000. The amphipods would like
// you to find a way to organize the amphipods that requires the least
// total energy.
// 
// However, because they are timid and stubborn, the amphipods have some
// extra rules:
// 
// * Amphipods will never stop on the space immediately outside any
// room. They can move into that space so long as they immediately
// continue moving. (Specifically, this refers to the four open
// spaces in the hallway that are directly above an amphipod starting
// position.)
// 
// * Amphipods will never move from the hallway into a room unless that
// room is their destination room and that room contains no amphipods
// which do not also have that room as their own destination. If an
// amphipod's starting room is not its destination room, it can stay
// in that room until it leaves the room. (For example, an Amber
// amphipod will not move from the hallway into the right three
// rooms, and will only move into the leftmost room if that room is
// empty or if it only contains other Amber amphipods.)
// 
// * Once an amphipod stops moving in the hallway, it will stay in that
// spot until it can move into a room. (That is, once any amphipod
// starts moving, any other amphipods currently in the hallway are
// locked in place and will not move again until they can move fully
// into a room.)
// 
// In the above example, the amphipods can be organized using a minimum
// of 12521 energy. One way to do this is shown below.
// 
// Starting configuration:
// 
// #############
// #...........#
// ###B#C#B#D###
// #A#D#C#A#
// #########
// 
// One Bronze amphipod moves into the hallway, taking 4 steps and using
// 40 energy:
// 
// #############
// #...B.......#
// ###B#C#.#D###
// #A#D#C#A#
// #########
// 
// The only Copper amphipod not in its side room moves there, taking 4
// steps and using 400 energy:
// 
// #############
// #...B.......#
// ###B#.#C#D###
// #A#D#C#A#
// #########
// 
// A Desert amphipod moves out of the way, taking 3 steps and using 3000
// energy, and then the Bronze amphipod takes its place, taking 3 steps
// and using 30 energy:
// 
// #############
// #.....D.....#
// ###B#.#C#D###
// #A#B#C#A#
// #########
// 
// The leftmost Bronze amphipod moves to its room using 40 energy:
// 
// #############
// #.....D.....#
// ###.#B#C#D###
// #A#B#C#A#
// #########
// 
// Both amphipods in the rightmost room move into the hallway, using 2003
// energy in total:
// 
// #############
// #.....D.D.A.#
// ###.#B#C#.###
// #A#B#C#.#
// #########
// 
// Both Desert amphipods move into the rightmost room using 7000 energy:
// 
// #############
// #.........A.#
// ###.#B#C#D###
// #A#B#C#D#
// #########
// 
// Finally, the last Amber amphipod moves into its room, using 8 energy:
// 
// #############
// #...........#
// ###A#B#C#D###
// #A#B#C#D#
// #########
// 
// What is the least energy required to organize the amphipods?
//
// --- Part Two ---
// ----------------
// 
// As you prepare to give the amphipods your solution, you notice that
// the diagram they handed you was actually folded up. As you unfold it,
// you discover an extra part of the diagram.
// 
// Between the first and second lines of text that contain amphipod
// starting positions, insert the following lines:
// 
// #D#C#B#A#
// #D#B#A#C#
// 
// So, the above example now becomes:
// 
// #############
// #...........#
// ###B#C#B#D###
// #D#C#B#A#
// #D#B#A#C#     #A#D#C#A#
// #########
// 
// The amphipods still want to be organized into rooms similar to before:
// 
// #############
// #...........#
// ###A#B#C#D###
// #A#B#C#D#
// #A#B#C#D#
// #A#B#C#D#
// #########
// 
// In this updated example, the least energy required to organize these
// amphipods is 44169:
// 
// #############
// #...........#
// ###B#C#B#D###
// #D#C#B#A#
// #D#B#A#C#
// #A#D#C#A#
// #########
// 
// #############
// #..........D#
// ###B#C#B#.###
// #D#C#B#A#
// #D#B#A#C#
// #A#D#C#A#
// #########
// 
// #############
// #A.........D#
// ###B#C#B#.###
// #D#C#B#.#
// #D#B#A#C#
// #A#D#C#A#
// #########
// 
// #############
// #A........BD#
// ###B#C#.#.###
// #D#C#B#.#
// #D#B#A#C#
// #A#D#C#A#
// #########
// 
// #############
// #A......B.BD#
// ###B#C#.#.###
// #D#C#.#.#
// #D#B#A#C#
// #A#D#C#A#
// #########
// 
// #############
// #AA.....B.BD#
// ###B#C#.#.###
// #D#C#.#.#
// #D#B#.#C#
// #A#D#C#A#
// #########
// 
// #############
// #AA.....B.BD#
// ###B#.#.#.###
// #D#C#.#.#
// #D#B#C#C#
// #A#D#C#A#
// #########
// 
// #############
// #AA.....B.BD#
// ###B#.#.#.###
// #D#.#C#.#
// #D#B#C#C#
// #A#D#C#A#
// #########
// 
// #############
// #AA...B.B.BD#
// ###B#.#.#.###
// #D#.#C#.#
// #D#.#C#C#
// #A#D#C#A#
// #########
// 
// #############
// #AA.D.B.B.BD#
// ###B#.#.#.###
// #D#.#C#.#
// #D#.#C#C#
// #A#.#C#A#
// #########
// 
// #############
// #AA.D...B.BD#
// ###B#.#.#.###
// #D#.#C#.#
// #D#.#C#C#
// #A#B#C#A#
// #########
// 
// #############
// #AA.D.....BD#
// ###B#.#.#.###
// #D#.#C#.#
// #D#B#C#C#
// #A#B#C#A#
// #########
// 
// #############
// #AA.D......D#
// ###B#.#.#.###
// #D#B#C#.#
// #D#B#C#C#
// #A#B#C#A#
// #########
// 
// #############
// #AA.D......D#
// ###B#.#C#.###
// #D#B#C#.#
// #D#B#C#.#
// #A#B#C#A#
// #########
// 
// #############
// #AA.D.....AD#
// ###B#.#C#.###
// #D#B#C#.#
// #D#B#C#.#
// #A#B#C#.#
// #########
// 
// #############
// #AA.......AD#
// ###B#.#C#.###
// #D#B#C#.#
// #D#B#C#.#
// #A#B#C#D#
// #########
// 
// #############
// #AA.......AD#
// ###.#B#C#.###
// #D#B#C#.#
// #D#B#C#.#
// #A#B#C#D#
// #########
// 
// #############
// #AA.......AD#
// ###.#B#C#.###
// #.#B#C#.#
// #D#B#C#D#
// #A#B#C#D#
// #########
// 
// #############
// #AA.D.....AD#
// ###.#B#C#.###
// #.#B#C#.#
// #.#B#C#D#
// #A#B#C#D#
// #########
// 
// #############
// #A..D.....AD#
// ###.#B#C#.###
// #.#B#C#.#
// #A#B#C#D#
// #A#B#C#D#
// #########
// 
// #############
// #...D.....AD#
// ###.#B#C#.###
// #A#B#C#.#
// #A#B#C#D#
// #A#B#C#D#
// #########
// 
// #############
// #.........AD#
// ###.#B#C#.###
// #A#B#C#D#
// #A#B#C#D#
// #A#B#C#D#
// #########
// 
// #############
// #..........D#
// ###A#B#C#.###
// #A#B#C#D#
// #A#B#C#D#
// #A#B#C#D#
// #########
// 
// #############
// #...........#
// ###A#B#C#D###
// #A#B#C#D#
// #A#B#C#D#
// #A#B#C#D#
// #########
// 
// Using the initial configuration from the full diagram, what is the
// least energy required to organize the amphipods?


#include "advent_of_code/2021/day23/day23.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/node_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "absl/log/log.h"
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

AllPathsMap ComputeAllPaths(const CharBoard& b,
                            const std::vector<Point>& point_list) {
  AllPathsMap ret;
  for (Point from : point_list) {
    for (Point to : point_list) {
      std::vector<Point>& path = ret[std::make_pair(from, to)];
      CHECK(CanMoveBFS(&b, from, to, &path).FindMinSteps());
      CHECK_EQ(path[0], from);
      CHECK_EQ(path.back(), to);
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
  friend std::ostream& operator<<(std::ostream& o, const Actor& a) {
    return o << absl::string_view(&a.c, 1) << ": " << a.cur << " " << a.moved
             << " " << a.done;
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

  absl::optional<int> CanMove(Point dest, const CharBoard& b,
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

  friend std::ostream& operator<<(std::ostream& o, const State& s) {
    o << "Cost=" << s.cost << "; Board:\n"
      << s.board << "\n"
      << "Actors:\n";
    for (const auto& a : s.actors) {
      o << a << "\n";
    }
    return o;
  }
};

absl::optional<int> FindMinCost(
    State initial_state, const std::vector<Point>& empty,
    const absl::flat_hash_map<char, std::vector<Point>>& destinations,
    const AllPathsMap& all_paths) {
  VLOG(1) << "Start: " << initial_state;
  // We use a `node_hash_set` so that `history` can be the owning storage
  // for the queue and use pointers to not 2x storage and copies.
  absl::node_hash_set<State> history = {initial_state};
  absl::optional<int> best;
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
      absl::optional<int> cost = cur.actors[i].CanMove(p, cur.board, all_paths);
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
        absl::optional<int> cost =
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = ParseAsBoard(input);
  if (!b.ok()) return b.status();

  State s{.board = *b};

  // Find the initial locations of actors and the temporary places in which to
  // store them.
  std::vector<Point> empty;
  std::vector<Point> srcs;
  for (Point p : b->range()) {
    if ((*b)[p] >= 'A' && (*b)[p] <= 'D') {
      s.actors.push_back({.c = (*b)[p], .cur = p});
      srcs.push_back(p);
    }
    // p + south = '#' is the stupid test for not stopping at a hallway
    // entrance.
    if ((*b)[p] == '.' && (*b)[p + Cardinal::kSouth] == '#') {
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
  AllPathsMap all_paths = ComputeAllPaths(*b, all_points);

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
      if ((*b)[test] != c) break;
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

  return IntReturn(FindMinCost(std::move(s), empty, destinations, all_paths));
}

absl::StatusOr<std::string> Day_2021_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 5) return Error("Bad input");
  const std::string insert1 = "  #D#C#B#A#  ";
  const std::string insert2 = "  #D#B#A#C#  ";
  std::vector<absl::string_view> spliced = {
      input[0], input[1], input[2], insert1, insert2, input[3], input[4],
  };
  return Part1(absl::MakeSpan(spliced));
}

}  // namespace advent_of_code
