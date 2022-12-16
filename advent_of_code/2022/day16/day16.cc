// http://adventofcode.com/2022/day/16

#include "advent_of_code/2022/day16/day16.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Valve {
  absl::string_view name;
  int flow;
};

absl::StatusOr<DirectedGraph<Valve>> ParseGraph(
    absl::Span<absl::string_view> input) {
  DirectedGraph<Valve> graph;
  int full_flow = 0;
  for (absl::string_view line : input) {
    Valve v;
    absl::string_view out;
    if (!RE2::FullMatch(line, "Valve ([A-Z]+) has flow rate=(\\d+); tunnels? leads? to valves? ([A-Z ,]+)",
                        &v.name, &v.flow, &out)) {
      return Error("Bad line: ", line);
    }
    full_flow += v.flow;
    graph.AddNode(v.name, v);
    for (absl::string_view out_name : absl::StrSplit(out, ", ")) {
      graph.AddEdge(v.name, out_name);
    }
  }
  return graph;
}

struct State {
  int open_set;
  absl::string_view me;
  absl::string_view el;
  int flow = 0;

  bool TryOpenMe(const DirectedGraph<Valve>& graph, 
                 const absl::flat_hash_map<absl::string_view, int>& pack) {
    auto bit_it = pack.find(me);
    if (bit_it == pack.end()) return false;
    int64_t bit = 1ll << bit_it->second;
    if (open_set & bit) return false;
    flow += graph.GetData(me)->flow;
    open_set |= bit;
    return true;
  }

  void MoveMe(absl::string_view dest) {
    me = dest;
  }

  bool TryOpenEl(const DirectedGraph<Valve>& graph, 
                 const absl::flat_hash_map<absl::string_view, int>& pack) {
    auto bit_it = pack.find(el);
    if (bit_it == pack.end()) return false;
    int64_t bit = 1ll << bit_it->second;
    if (open_set & bit) return false;
    flow += graph.GetData(el)->flow;
    open_set |= bit;
    return true;
  }

  void MoveEl(absl::string_view dest) {
    el = dest;
  }

  bool operator==(const State& o) const {
    return open_set == o.open_set && me == o.me && el == o.el;
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.open_set, s.me, s.el);
  }
};

absl::StatusOr<int> BestPath(
    const DirectedGraph<Valve>& graph, int minutes, bool use_elephant) {
  absl::flat_hash_map<absl::string_view, int> pack;
  int64_t all_on = 0;
  int full_flow = 0;
  for (absl::string_view n : graph.nodes()) {
    if (graph.GetData(n)->flow == 0) continue;
    all_on |= 1ll << pack.size();
    full_flow += graph.GetData(n)->flow;
    pack[n] = pack.size();
  }
  if (pack.size() > 30) return Error("Can't fit");
  VLOG(1) << "Max cardinality: "
          << graph.nodes().size() * (1 << pack.size()) *
             (use_elephant ? graph.nodes().size() : 1);

  int best_known = 0;
  if (use_elephant) { 
    absl::StatusOr<int> just_me =
        BestPath(graph, minutes, /*use_elephant=*/false);
    if (!just_me.ok()) return just_me.status();
    best_known = *just_me;
  }

  absl::flat_hash_map<State, int> state_to_flow =
      {{State{.me = "AA", .el = "AA", .open_set = 0, .flow = 0}, 0}};
  for (int r = 0; r < minutes; ++r) {
    VLOG(1) << r << ": " << state_to_flow.size();
    absl::flat_hash_map<State, int> new_state_to_flow;
    int best_flow = best_known;
    for (const auto& [s, f] : state_to_flow) {
      best_flow = std::max(best_flow, f + s.flow * (minutes - r));
    }
    for (const auto& [s, f] : state_to_flow) {
      if (f + full_flow * (minutes - r) < best_flow) continue;
      int new_flow = f + s.flow;
      if (s.open_set == all_on) {
        new_state_to_flow[s] = std::max(new_state_to_flow[s], new_flow);
        continue;
      }
      {
        State news = s;
        if (news.TryOpenMe(graph, pack)) {
          if (use_elephant) {
            State newes = news;
            if (newes.TryOpenEl(graph, pack)) {
              new_state_to_flow[newes] = std::max(new_state_to_flow[newes], new_flow);
            }
            for (absl::string_view out : *graph.Outgoing(s.el)) {
              State newes = news;
              newes.MoveEl(out);
              new_state_to_flow[newes] = std::max(new_state_to_flow[newes], new_flow);
            }
          } else {
            new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
          }
        }
      }
      for (absl::string_view out : *graph.Outgoing(s.me)) {
        State news = s;
        news.MoveMe(out);
        if (use_elephant) {
          State newes = news;
          if (newes.TryOpenEl(graph, pack)) {
            new_state_to_flow[newes] = std::max(new_state_to_flow[newes], new_flow);
          }
          for (absl::string_view out : *graph.Outgoing(s.el)) {
            State newes = news;
            newes.MoveEl(out);
            new_state_to_flow[newes] = std::max(new_state_to_flow[newes], new_flow);
          }
        } else {
          new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
        }
      }
    }
    state_to_flow = new_state_to_flow;
  }
  int max = 0;
  for (const auto& [s, f] : state_to_flow) {
    max = std::max(f, max);
  }
  return max;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_16::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<Valve>> graph = ParseGraph(input);
  if (!graph.ok()) return graph.status();

  return IntReturn(BestPath(*graph, 30, /*use_elephant=*/false));
}

absl::StatusOr<std::string> Day_2022_16::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<Valve>> graph = ParseGraph(input);
  if (!graph.ok()) return graph.status();

  return IntReturn(BestPath(*graph, 26, /*use_elephant=*/true));
}

}  // namespace advent_of_code
