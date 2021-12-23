#include "advent_of_code/2021/day23/day23.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class CanMoveBFS : public BFSInterface<CanMoveBFS, Point> {
 public:
  CanMoveBFS(const CharBoard* b, Point cur, Point end) : b_(b), cur_(cur), end_(end) {}

  Point identifier() const override { return cur_; }

  bool IsFinal() const override { return cur_ == end_; }

  void AddNextSteps(State* state) const override {
    for (Point d : Cardinal::kFourDirs) {
      if (!b_->OnBoard(cur_ + d)) continue;
      if ((*b_)[cur_ + d] != '.') continue;
      CanMoveBFS next = *this;
      next.cur_ += d;
      state->AddNextStep(next);
    }
  }

 private:
  const CharBoard* b_;
  Point cur_;
  Point end_;
};

struct State {
  struct Actor {
    char c;
    Point cur;
    bool moved;
    bool done;
    bool operator==(const Actor& o) const {
      return c == o.c && cur == o.cur && moved == o.moved && done == o.done;
    }
    template <typename H>
    friend H AbslHashValue(H h, const Actor& a) {
      return H::combine(std::move(h), a.c, a.cur, a.moved, a.done);
    }
    friend std::ostream& operator<<(std::ostream& o, const Actor& a) {
      return o << absl::string_view(&a.c, 1) << ": " << a.cur << " " << a.moved << " " << a.done;
    }
    Point Destination(const absl::flat_hash_map<char, std::vector<Point>>& destinations,
                      const CharBoard& b) const {
      auto it = destinations.find(c);
      CHECK(it != destinations.end());
      for (Point p : it->second) {
        if (b[p] != c) return p;
      }
      LOG(FATAL) << "No destination found: " << *this << "\n" << b;
    }
    absl::optional<int> CanMove(Point dest, const CharBoard& b) const {
      absl::optional<int> dist = CanMoveBFS(&b, cur, dest).FindMinSteps();
      if (!dist) return {};
      return *dist * cost();
    }
    int cost() const {
      switch (c) {
        case 'A': return 1;
        case 'B': return 10;
        case 'C': return 100;
        case 'D': return 1000;
      }
      LOG(FATAL) << "Bad char";
    }
  };
  bool IsFinal() const {
    for (const auto& a : actors) {
      if (!a.done) return false;
    }
    return true;
  }
  CharBoard board;
  std::vector<Actor> actors;
  int cost = 0;

  bool operator==(const State& o) const {
    return board == o.board && actors == o.actors && cost == o.cost;
  }
  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.board, s.actors, s.cost);
  }

  friend std::ostream& operator<<(std::ostream& o, const State& s) {
    o << "Cost=" << s.cost << "; Board:\n" << s.board << "\n" << "Actors:\n";
    for (const auto& a : s.actors) {
      o << a << "\n";
    }
    return o;
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2021_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> b = CharBoard::Parse(input);
  if (!b.ok()) return b.status();

  State s{.board = *b};

  // Find the initial locations of actors and the temporary places in which to
  // store them. 
  absl::flat_hash_set<Point> empty;
  std::vector<Point> srcs;
  for (Point p : b->range()) {
    if ((*b)[p] >= 'A' && (*b)[p] <= 'D') {
      s.actors.push_back({.c = (*b)[p], .cur = p, .moved = false, .done = false});
      srcs.push_back(p);
    }
    // p + south = '#' is the stupid test for not stopping at a hallway
    // entrance.
    if ((*b)[p] == '.' && (*b)[p + Cardinal::kSouth] == '#') {
      empty.insert(p);
    }
  };

  // Identify the destination locations for each set of actors.
  if (srcs.size() % 4 != 0) return Error("Bad srcs");
  absl::c_sort(srcs, [](Point a, Point b) { if (a.x != b.x) return a.x < b.x; return a.y > b.y; });
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

  VLOG(1) << "Start: " << s;
  absl::flat_hash_set<State> history = {s};
  int64_t best = std::numeric_limits<int64_t>::max();
  for (std::deque<State> queue = {s}; !queue.empty(); queue.pop_front()) {
    const State& cur = queue.front();
    if (cur.IsFinal()) {
      if (best > cur.cost) {
        VLOG(1) << "Found final: " << cur;
        best = cur.cost;
      }
      continue;
    }
    // Fork off test paths for the next best action of each actor. 
    for (int i = 0; i < cur.actors.size(); ++i) {
      if (cur.actors[i].done) continue;
      // Can we go to the final location?
      Point p = cur.actors[i].Destination(destinations, cur.board);
      absl::optional<int> cost = cur.actors[i].CanMove(p, cur.board);
      if (cost) {
        // We can reach the final destination. Go there and stop.
        State new_state = cur;
        new_state.board[new_state.actors[i].cur] = '.';
        new_state.actors[i].cur = p;
        new_state.actors[i].done = true;
        new_state.board[p] = new_state.actors[i].c;
        new_state.cost += *cost;
        if (history.contains(new_state)) continue;
        history.insert(new_state);
        // VLOG(1) << cur << " => " << new_state;
        queue.push_back(new_state);
        continue;
      }
      // If we have alread moved, we can't move again.
      if (cur.actors[i].moved) continue;

      // If we haven't moved, and we can't get to the final destionation,
      // fork off test paths for each candidate temporary location.
      for (Point p : empty) {
        absl::optional<int> cost = cur.actors[i].CanMove(p, cur.board);
        if (cost) {
          State new_state = cur;
          new_state.board[new_state.actors[i].cur] = '.';
          new_state.actors[i].cur = p;
          new_state.actors[i].moved = true;
          new_state.board[p] = new_state.actors[i].c;
          new_state.cost += *cost;
          if (history.contains(new_state)) continue;
          history.insert(new_state);
          // VLOG(1) << cur << " => " << new_state;
          queue.push_back(new_state);
        }
      }
    }
  }
  return IntReturn(best);
}

absl::StatusOr<std::string> Day_2021_23::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 5) return Error("Bad input");
  std::string insert1 = "  #D#C#B#A#  ";
  std::string insert2 = "  #D#B#A#C#  ";
  std::vector<absl::string_view> spliced = {
    input[0], input[1], input[2], insert1, insert2, input[3], input[4],
  };
  return Part1(absl::MakeSpan(spliced));
}

}  // namespace advent_of_code
