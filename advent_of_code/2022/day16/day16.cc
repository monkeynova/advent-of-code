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
  bool open;
};

struct State {
  int64_t open;
  absl::string_view cur;
  int flow = 0;

  bool operator==(const State& o) const {
    return open == o.open && cur == o.cur;
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    return H::combine(std::move(h), s.open, s.cur);
  }
};

struct State2 {
  int64_t open;
  absl::string_view me;
  absl::string_view el;
  int flow = 0;

  bool operator==(const State2& o) const {
    return open == o.open && me == o.me && el == o.el;
  }

  template <typename H>
  friend H AbslHashValue(H h, const State2& s) {
    return H::combine(std::move(h), s.open, s.me, s.el);
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2022_16::Part1(
    absl::Span<absl::string_view> input) const {
  DirectedGraph<Valve> graph;
  int closed_count = 0;
  int full_flow = 0;
  for (absl::string_view line : input) {
    Valve v;
    absl::string_view out;
    if (!RE2::FullMatch(line, "Valve ([A-Z]+) has flow rate=(\\d+); tunnels? leads? to valves? ([A-Z ,]+)",
                        &v.name, &v.flow, &out)) {
      return Error("Bad line: ", line);
    }
    v.open = false;
    if (v.flow == 0) {
      v.open = true;
      ++closed_count;
    }
    full_flow += v.flow;
    graph.AddNode(v.name, v);
    for (absl::string_view out_name : absl::StrSplit(out, ", ")) {
      graph.AddEdge(v.name, out_name);
    }
  }
  absl::flat_hash_map<absl::string_view, int> pack;
  int64_t all_on = 0;
  int64_t start_on = 0;
  for (absl::string_view n : graph.nodes()) {
    all_on |= 1ll << pack.size();
    if (graph.GetData(n)->open) start_on |= 1ll << pack.size();
    pack[n] = pack.size();
  }
  if (pack.size() > 62) return Error("Can't fit");
  absl::flat_hash_map<State, int> state_to_flow = {{State{.cur = "AA", .open = start_on, .flow = 0}, 0}};
  for (int r = 0; r < 30; ++r) {
    VLOG(1) << r << ": " << state_to_flow.size();
    absl::flat_hash_map<State, int> new_state_to_flow;
    int best_flow = 0;
    for (const auto& [s, f] : state_to_flow) {
      best_flow = std::max(best_flow, f + s.flow * (30 - r));
    }
    for (const auto& [s, f] : state_to_flow) {
      if (f + full_flow * (30 - r) < best_flow) continue;
      int new_flow = f + s.flow;
      if (s.open == all_on) {
        new_state_to_flow[s] = std::max(new_state_to_flow[s], new_flow);
        continue;
      }
      auto bit_it = pack.find(s.cur);
      if (bit_it == pack.end()) return Error("Bad bit");
      int bit = bit_it->second;
      if ((s.open & (1ll << bit)) == 0) {
        State news = s;
        news.flow += graph.GetData(s.cur)->flow;
        news.open |= (1ll << bit);
        new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
      }
      for (absl::string_view out : *graph.Outgoing(s.cur)) {
        State news = s;
        news.cur = out;
        new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
      }
    }
    state_to_flow = new_state_to_flow;
  }
  int max = 0;
  for (const auto& [s, f] : state_to_flow) {
    max = std::max(f, max);
  }
  return IntReturn(max);
}

absl::StatusOr<std::string> Day_2022_16::Part2(
    absl::Span<absl::string_view> input) const {
  DirectedGraph<Valve> graph;
  int closed_count = 0;
  int full_flow = 0;
  for (absl::string_view line : input) {
    Valve v;
    absl::string_view out;
    if (!RE2::FullMatch(line, "Valve ([A-Z]+) has flow rate=(\\d+); tunnels? leads? to valves? ([A-Z ,]+)",
                        &v.name, &v.flow, &out)) {
      return Error("Bad line: ", line);
    }
    v.open = false;
    if (v.flow == 0) {
      v.open = true;
      ++closed_count;
    }
    full_flow += v.flow;
    graph.AddNode(v.name, v);
    for (absl::string_view out_name : absl::StrSplit(out, ", ")) {
      graph.AddEdge(v.name, out_name);
    }
  }
  absl::flat_hash_map<absl::string_view, int> pack;
  int64_t all_on = 0;
  int64_t start_on = 0;
  for (absl::string_view n : graph.nodes()) {
    all_on |= 1ll << pack.size();
    if (graph.GetData(n)->open) start_on |= 1ll << pack.size();
    pack[n] = pack.size();
  }
  if (pack.size() > 62) return Error("Can't fit");
  absl::flat_hash_map<State2, int> state_to_flow =
      {{State2{.me = "AA", .el = "AA", .open = start_on, .flow = 0}, 0}};
  for (int r = 0; r < 26; ++r) {
    VLOG(1) << r << ": " << state_to_flow.size();
    absl::flat_hash_map<State2, int> new_state_to_flow;
    int best_flow = 0;
    for (const auto& [s, f] : state_to_flow) {
      best_flow = std::max(best_flow, f + s.flow * (26 - r));
    }
    for (const auto& [s, f] : state_to_flow) {
      if (f + full_flow * (26 - r) < best_flow) continue;
      int new_flow = f + s.flow;
      if (s.open == all_on) {
        new_state_to_flow[s] = std::max(new_state_to_flow[s], new_flow);
        continue;
      }
      auto me_bit_it = pack.find(s.me);
      if (me_bit_it == pack.end()) return Error("Bad bit");
      int me_bit = me_bit_it->second;
      auto el_bit_it = pack.find(s.el);
      if (el_bit_it == pack.end()) return Error("Bad bit");
      int el_bit = el_bit_it->second;
      if (me_bit != el_bit && (s.open & (1ll << me_bit)) == 0 &&
          (s.open & (1ll << el_bit)) == 0) {
        // We both turn on.
        State2 news = s;
        news.flow += graph.GetData(s.me)->flow;
        news.open |= (1ll << me_bit);
        news.flow += graph.GetData(s.el)->flow;
        news.open |= (1ll << el_bit);
        new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
      }
      if ((s.open & (1ll << me_bit)) == 0) {
        // I turn on, elephant moves.
        for (absl::string_view out : *graph.Outgoing(s.el)) {
          State2 news = s;
          news.el = out;
          news.flow += graph.GetData(s.me)->flow;
          news.open |= (1ll << me_bit);
          new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
        }
      }
      if ((s.open & (1ll << el_bit)) == 0) {
        // Elevent turns on, I move.
        for (absl::string_view out : *graph.Outgoing(s.me)) {
          State2 news = s;
          news.me = out;
          news.flow += graph.GetData(s.el)->flow;
          news.open |= (1ll << el_bit);
          new_state_to_flow[news] = std::max(new_state_to_flow[news], new_flow);
        }
      }
      // We both move.
      for (absl::string_view me_out : *graph.Outgoing(s.me)) {
        for (absl::string_view el_out : *graph.Outgoing(s.el)) {
          State2 news = s;
          news.me = me_out;
          news.el = el_out;
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
  return IntReturn(max);
}

}  // namespace advent_of_code
