// https://adventofcode.com/2016/day/22
//
// --- Day 22: Grid Computing ---
// ------------------------------
// 
// You gain access to a massive storage cluster arranged in a grid; each
// storage node is only connected to the four nodes directly adjacent to
// it (three if the node is on an edge, two if it's in a corner).
// 
// You can directly access data only on node /dev/grid/node-x0-y0, but
// you can perform some limited actions on the other nodes:
// 
// * You can get the disk usage of all nodes (via df). The result of
// doing this is in your puzzle input.
// 
// * You can instruct a node to move (not copy) all of its data to an
// adjacent node (if the destination node has enough space to receive
// the data). The sending node is left empty after this operation.
// 
// Nodes are named by their position: the node named node-x10-y10 is
// adjacent to nodes node-x9-y10, node-x11-y10, node-x10-y9, and
// node-x10-y11.
// 
// Before you begin, you need to understand the arrangement of data on
// these nodes. Even though you can only move data between directly
// connected nodes, you're going to need to rearrange a lot of the data
// to get access to the data you need. Therefore, you need to work out
// how you might be able to shift data around.
// 
// To do this, you'd like to count the number of viable pairs of nodes. A
// viable pair is any two nodes (A,B), regardless of whether they are
// directly connected, such that:
// 
// * Node A is not empty (its Used is not zero).
// 
// * Nodes A and B are not the same node.
// 
// * The data on node A (its Used) would fit on node B (its Avail).
// 
// How many viable pairs of nodes are there?
//
// --- Part Two ---
// ----------------
// 
// Now that you have a better understanding of the grid, it's time to get
// to work.
// 
// Your goal is to gain access to the data which begins in the node with
// y=0 and the highest x (that is, the node in the top-right corner).
// 
// For example, suppose you have the following grid:
// 
// Filesystem            Size  Used  Avail  Use%
// /dev/grid/node-x0-y0   10T    8T     2T   80%
// /dev/grid/node-x0-y1   11T    6T     5T   54%
// /dev/grid/node-x0-y2   32T   28T     4T   87%
// /dev/grid/node-x1-y0    9T    7T     2T   77%
// /dev/grid/node-x1-y1    8T    0T     8T    0%
// /dev/grid/node-x1-y2   11T    7T     4T   63%
// /dev/grid/node-x2-y0   10T    6T     4T   60%
// /dev/grid/node-x2-y1    9T    8T     1T   88%
// /dev/grid/node-x2-y2    9T    6T     3T   66%
// 
// In this example, you have a storage grid 3 nodes wide and 3 nodes
// tall. The node you can access directly, node-x0-y0, is almost full.
// The node containing the data you want to access, node-x2-y0 (because
// it has y=0 and the highest x value), contains 6 terabytes of data -
// enough to fit on your node, if only you could make enough space to
// move it there.
// 
// Fortunately, node-x1-y1 looks like it has enough free space to enable
// you to move some of this data around. In fact, it seems like all of
// the nodes have enough space to hold any node's data (except node-x0-y2,
// which is much larger, very full, and not moving any time soon). So,
// initially, the grid's capacities and connections look like this:
// 
// ( 8T/10T) --  7T/ 9T -- [ 6T/10T]
// |           |           |
// 6T/11T  --  0T/ 8T --   8T/ 9T
// |           |           |
// 28T/32T  --  7T/11T --   6T/ 9T
// 
// The node you can access directly is in parentheses; the data you want
// starts in the node marked by square brackets.
// 
// In this example, most of the nodes are interchangable: they're full
// enough that no other node's data would fit, but small enough that
// their data could be moved around. Let's draw these nodes as .. The
// exceptions are the empty node, which we'll draw as _, and the very
// large, very full node, which we'll draw as #. Let's also draw the goal
// data as G. Then, it looks like this:
// 
// (.) .  G
// .  _  .
// #  .  .
// 
// The goal is to move the data in the top right, G, to the node in
// parentheses. To do this, we can issue some commands to the grid and
// rearrange the data:
// 
// * 
// 
// Move data from node-y0-x1 to node-y1-x1, leaving node node-y0-x1
// empty:
// 
// (.) _  G
// .  .  .
// #  .  .     
// 
// * 
// 
// Move the goal data from node-y0-x2 to node-y0-x1:
// 
// (.) G  _
// .  .  .
// #  .  .     
// 
// * 
// 
// At this point, we're quite close. However, we have no deletion
// command, so we have to move some more data around. So, next, we
// move the data from node-y1-x2 to node-y0-x2:
// 
// (.) G  .
// .  .  _
// #  .  .     
// 
// * 
// 
// Move the data from node-y1-x1 to node-y1-x2:
// 
// (.) G  .
// .  _  .
// #  .  .     
// 
// * 
// 
// Move the data from node-y1-x0 to node-y1-x1:
// 
// (.) G  .
// _  .  .
// #  .  .     
// 
// * 
// 
// Next, we can free up space on our node by moving the data from
// node-y0-x0 to node-y1-x0:
// 
// (_) G  .
// .  .  .
// #  .  .     
// 
// * 
// 
// Finally, we can access the goal data by moving the it from
// node-y0-x1 to node-y0-x0:
// 
// (G) _  .
// .  .  .
// #  .  .     
// 
// So, after 7 steps, we've accessed the data we want. Unfortunately,
// each of these moves takes time, and we need to be efficient:
// 
// What is the fewest number of steps required to move your goal data to
// node-x0-y0?

#include "advent_of_code/2016/day22/day22.h"

#include <queue>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Node {
  int used;
  int avail;
  bool operator==(const Node& o) const {
    return used == o.used && avail == o.avail;
  }
  bool operator!=(const Node& o) const { return !operator==(o); }
};

template <typename H>
H AbslHashValue(H h, const Node& n) {
  return H::combine(std::move(h), n.used, n.avail);
}

struct State {
  absl::flat_hash_map<Point, Node> grid;
  Point data_loc;
  Point grid_end;
  int num_steps = 0;
  bool operator<(const State& o) const {
    // Greater than for use in a priority queue...
    return o.num_steps + o.data_loc.dist() < num_steps + data_loc.dist();
  }
  bool operator==(const State& o) const {
    return grid == o.grid && data_loc == o.data_loc;
  }
  bool operator!=(const State& o) const { return !operator==(o); }
};

template <typename H>
H AbslHashValue(H h, const State& s) {
  h = H::combine(std::move(h), s.data_loc);
  for (Point p : PointRectangle{{0, 0}, s.grid_end}) {
    auto it = s.grid.find(p);
    if (it != s.grid.end()) {
      h = H::combine(std::move(h), it->second);
    }
  }
  return h;
}

absl::StatusOr<int> FindMinPath(absl::flat_hash_map<Point, Node> grid) {
  State initial_state{
      .grid = grid, .data_loc = {0, 0}, .grid_end = {0, 0}, .num_steps = 0};
  for (const auto& [p, _] : grid) {
    if (p.y == 0 && p.x > initial_state.data_loc.x) {
      initial_state.data_loc = p;
    }
    if (p.y > initial_state.grid_end.y) {
      initial_state.grid_end.y = p.y;
    }
    if (p.x > initial_state.grid_end.x) {
      initial_state.grid_end.x = p.x;
    }
  }
  VLOG(1) << "Moving from " << initial_state.data_loc << " to " << Point{0, 0};
  std::priority_queue<State> frontier;
  absl::flat_hash_set<State> hist;
  hist.insert(initial_state);
  frontier.push(std::move(initial_state));
  while (!frontier.empty()) {
    State cur = frontier.top();
    frontier.pop();
    VLOG_EVERY_N(2, 777) << "data @" << cur.data_loc << " after "
                         << cur.num_steps << " steps";
    for (const auto& [src_p, src_node] : cur.grid) {
      for (Point dir : Cardinal::kFourDirs) {
        Point dst_p = src_p + dir;
        auto it = cur.grid.find(dst_p);
        if (it == cur.grid.end()) continue;
        const Node& dest_node = it->second;
        if (dest_node.avail >= src_node.used) {
          State next = cur;
          ++next.num_steps;
          next.grid[dst_p].used += next.grid[src_p].used;
          next.grid[dst_p].avail -= next.grid[src_p].used;
          next.grid[src_p].avail += next.grid[src_p].used;
          next.grid[src_p].used = 0;
          if (src_p == next.data_loc) {
            next.data_loc = dst_p;
            if (next.data_loc == Point{0, 0}) {
              return next.num_steps;
            }
          }
          if (!hist.contains(next)) {
            hist.insert(next);
            frontier.push(std::move(next));
          }
        }
      }
    }
  }
  return Error("No path found;");
}

struct PathState {
  CharBoard b;
  int num_steps;
  bool operator==(const PathState& o) const { return b == o.b; }
};

template <typename H>
H AbslHashValue(H h, const PathState& s) {
  return H::combine(std::move(h), s.b);
}

absl::StatusOr<int> RunHacks(const absl::flat_hash_map<Point, Node>& grid) {
  Point max = {0, 0};
  for (const auto& [p, n] : grid) {
    max.x = std::max(max.x, p.x);
    max.y = std::max(max.y, p.y);
  }
  CharBoard board(max.x + 1, max.y + 1);
  for (Point p : board.range()) board[p] = '?';
  int min_used = std::numeric_limits<int>::max();
  int min_space = std::numeric_limits<int>::max();
  int max_used = std::numeric_limits<int>::min();
  int max_space = std::numeric_limits<int>::min();
  Point goal_loc = {0, 0};
  Point empty;
  for (const auto& [p, n] : grid) {
    if (p.y == 0 && p.x > goal_loc.x) goal_loc.x = p.x;
    // TODO(@monkeynova): Need better heuristic...
    if (n.used > 400)
      board[p] = '#';
    else if (n.used == 0) {
      board[p] = '_';
      empty = p;
    } else {
      board[p] = '.';
      min_used = std::min(min_used, n.used);
      max_used = std::max(max_used, n.used);
      min_space = std::min(min_space, n.used + n.avail);
      max_space = std::max(max_space, n.used + n.avail);
    }
  }
  if (2 * min_used < min_space) return Error("Invalid assumption");
  if (max_used > min_space) return Error("Invalid assumption");
  board[goal_loc] = 'G';
  LOG(INFO) << "Empty at " << empty;
  LOG(INFO) << "Used: (" << min_used << "," << max_used << ") out of ("
            << min_space << "," << max_space << ")";
  LOG(INFO) << "Board:\n" << board;

  std::deque<PathState> frontier = {{.b = board, .num_steps = 0}};
  absl::flat_hash_set<PathState> hist;
  hist.insert(frontier.front());
  while (!frontier.empty()) {
    const PathState& path_state = frontier.front();
    // TODO(@monkeynova): This doesn't need to be a linear scan.
    for (Point p : path_state.b.range()) {
      if (path_state.b[p] == '_') {
        for (Point dir : Cardinal::kFourDirs) {
          Point next = p + dir;
          if (path_state.b.OnBoard(next) && path_state.b[next] != '#') {
            PathState new_state = path_state;
            ++new_state.num_steps;
            std::swap(new_state.b[next], new_state.b[p]);
            if (new_state.b[Point{0, 0}] == 'G') return new_state.num_steps;
            if (!hist.contains(new_state)) {
              hist.insert(new_state);
              frontier.push_back(new_state);
            }
          }
        }
      }
    }
    frontier.pop_front();
  }

  return Error("No path found");
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_22::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<Point, Node> grid;
  for (absl::string_view str : input) {
    Point p;
    if (RE2::FullMatch(str, "root@ebhq-gridcenter# df -h")) continue;
    // Filesystem            Size  Used  Avail  Use%
    if (RE2::FullMatch(str, "Filesystem\\s+Size\\s+Used\\s+Avail\\s+Use%")) {
      continue;
    }
    Node n;
    int size;
    if (!RE2::FullMatch(
            str,
            "/dev/grid/"
            "node-x(\\d+)-y(\\d+)\\s+(\\d+)T\\s+(\\d+)T\\s+(\\d+)T\\s+\\d+%",
            &p.x, &p.y, &size, &n.used, &n.avail)) {
      return Error("Bad line: ", str);
    }
    if (size != n.used + n.avail) return Error("Bad used: ", str);
    if (grid.contains(p)) return Error("Dupe point: ", p);
    grid[p] = n;
  }
  int viable = 0;
  for (const auto& [p1, n1] : grid) {
    for (const auto& [p2, n2] : grid) {
      if (p1 == p2) continue;
      if (n1.used > 0 && n1.used < n2.avail) ++viable;
    }
  }
  return IntReturn(viable);
}

absl::StatusOr<std::string> Day_2016_22::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<Point, Node> grid;
  for (absl::string_view str : input) {
    Point p;
    if (RE2::FullMatch(str, "root@ebhq-gridcenter# df -h")) continue;
    // Filesystem            Size  Used  Avail  Use%
    if (RE2::FullMatch(str, "Filesystem\\s+Size\\s+Used\\s+Avail\\s+Use%")) {
      continue;
    }
    Node n;
    int size;
    if (!RE2::FullMatch(
            str,
            "/dev/grid/"
            "node-x(\\d+)-y(\\d+)\\s+(\\d+)T\\s+(\\d+)T\\s+(\\d+)T\\s+\\d+%",
            &p.x, &p.y, &size, &n.used, &n.avail)) {
      return Error("Bad line: ", str);
    }
    if (size != n.used + n.avail) return Error("Bad used: ", str);
    if (n.used > 0 && 2 * n.used < size)
      return Error("Only work with over half full disks");
    if (grid.contains(p)) return Error("Dupe point: ", p);
    grid[p] = n;
  }
  if (grid.size() > 100) {
    return IntReturn(RunHacks(grid));
  }

  return IntReturn(FindMinPath(grid));
}

}  // namespace advent_of_code
