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
#include "advent_of_code/bfs.h"
#include "advent_of_code/directed_graph.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Valve {
  std::string_view name;
  int flow;
};

absl::StatusOr<DirectedGraph<Valve>> ParseGraph(
    absl::Span<std::string_view> input) {
  DirectedGraph<Valve> graph;
  for (std::string_view line : input) {
    Valve v;
    std::string_view out;
    if (!RE2::FullMatch(line,
                        "Valve ([A-Z]+) has flow rate=(\\d+); "
                        "tunnels? leads? to valves? ([A-Z ,]+)",
                        &v.name, &v.flow, &out)) {
      return Error("Bad line: ", line);
    }
    if (v.name.size() != 2) return Error("Bad valve: ", v.name);
    graph.AddNode(v.name, v);
    for (std::string_view out_name : absl::StrSplit(out, ", ")) {
      if (out_name.size() != 2) return Error("Bad valve: ", out_name);
      graph.AddEdge(v.name, out_name);
    }
  }
  return graph;
}

struct State {
  int16_t me;
  int16_t el;
  int open_set;

  absl::Status NextForMe(const DirectedGraph<Valve>& graph,
                         const absl::flat_hash_map<std::string_view, int>& pack,
                         absl::FunctionRef<absl::Status(State)> on_next) const {
    if (open_set == (1 << pack.size()) - 1) {
      return on_next(*this);
    }
    TryOpenMe(pack, on_next);
    std::string me_str = {static_cast<char>(me >> 8),
                          static_cast<char>(me & 0xFF)};
    if (graph.Outgoing(me_str) == nullptr) {
      return Error("No outgoing for me from: ", me_str);
    }
    for (std::string_view out : *graph.Outgoing(me_str)) {
      State s = *this;
      s.MoveMe(out);
      RETURN_IF_ERROR(on_next(s));
    }
    return absl::OkStatus();
  }

  absl::Status NextForEl(const DirectedGraph<Valve>& graph,
                         const absl::flat_hash_map<std::string_view, int>& pack,
                         absl::FunctionRef<absl::Status(State)> on_next) const {
    if (open_set == (1 << pack.size()) - 1) {
      return on_next(*this);
    }
    TryOpenEl(pack, on_next);
    std::string el_str = {static_cast<char>(el >> 8),
                          static_cast<char>(el & 0xFF)};
    if (graph.Outgoing(el_str) == nullptr) {
      return Error("No outgoing for el from: ", el_str);
    }
    for (std::string_view out : *graph.Outgoing(el_str)) {
      State s = *this;
      s.MoveEl(out);
      RETURN_IF_ERROR(on_next(s));
    }
    return absl::Status();
  }

  void TryOpenMe(const absl::flat_hash_map<std::string_view, int>& pack,
                 absl::FunctionRef<void(State)> on_next) const {
    std::string me_str = {static_cast<char>(me >> 8),
                          static_cast<char>(me & 0xFF)};
    auto bit_it = pack.find(me_str);
    if (bit_it == pack.end()) return;
    int64_t bit = 1ll << bit_it->second;
    if (open_set & bit) return;

    State new_state = *this;
    new_state.open_set |= bit;
    on_next(new_state);
    ;
  }

  void MoveMe(std::string_view dest) { me = (dest[0] << 8) | dest[1]; }

  void TryOpenEl(const absl::flat_hash_map<std::string_view, int>& pack,
                 absl::FunctionRef<void(State)> on_next) const {
    std::string el_str = {static_cast<char>(el >> 8),
                          static_cast<char>(el & 0xFF)};
    auto bit_it = pack.find(el_str);
    if (bit_it == pack.end()) return;
    int64_t bit = 1ll << bit_it->second;
    if (open_set & bit) return;

    State new_state = *this;
    new_state.open_set |= bit;
    on_next(new_state);
  }

  void MoveEl(std::string_view dest) { el = (dest[0] << 8) | dest[1]; }

  bool operator==(const State& o) const {
    return open_set == o.open_set &&
           ((me == o.me && el == o.el) || (me == o.el && el == o.me));
  }

  template <typename H>
  friend H AbslHashValue(H h, const State& s) {
    if (s.me < s.el) {
      return H::combine(std::move(h), s.open_set, s.me, s.el);
    }
    return H::combine(std::move(h), s.open_set, s.el, s.me);
  }
};

void InsertOrUpdateMax(absl::flat_hash_map<State, int>& map, State s,
                       int new_val) {
  auto [it, inserted] = map.emplace(s, new_val);
  if (!inserted) it->second = std::max(it->second, new_val);
}

absl::StatusOr<int> BestPath(const DirectedGraph<Valve>& graph, int minutes,
                             bool use_elephant) {
  absl::flat_hash_map<std::string_view, int> pack;
  int64_t all_on = 0;
  std::vector<const Valve*> ordered_valves;
  for (std::string_view n : graph.nodes()) {
    ordered_valves.push_back(graph.GetData(n));
  }
  auto by_flow = [](const Valve* a, const Valve* b) {
    return a->flow > b->flow;
  };
  absl::c_sort(ordered_valves, by_flow);
  for (const Valve* v : ordered_valves) {
    if (v->flow == 0) break;
    all_on |= 1ll << pack.size();
    pack[v->name] = pack.size();
  }
  if (pack.size() > 30) return Error("Can't fit");

  std::vector<int> flows(all_on + 1);
  for (int bv = 1; bv <= all_on; ++bv) {
    for (int bit = 0; (1 << bit) <= bv; ++bit) {
      if (bv & (1 << bit)) {
        flows[bv] += ordered_valves[bit]->flow;
      }
    }
  }

  int best_known = 0;
  if (use_elephant) {
    ASSIGN_OR_RETURN(best_known,
                     BestPath(graph, minutes, /*use_elephant=*/false));
  }

  VLOG(1) << "Max cardinality: " << graph.nodes().size() << "*"
          << (1 << pack.size()) << "*"
          << (use_elephant ? graph.nodes().size() : 1) << " = "
          << graph.nodes().size() * (1 << pack.size()) *
                 (use_elephant ? graph.nodes().size() : 1);

  State start;
  start.open_set = 0;
  start.MoveMe("AA");
  start.MoveEl("AA");
  absl::flat_hash_map<State, int> state_to_pressure = {{start, 0}};
  for (int r = 0; r < minutes; ++r) {
    VLOG(1) << r << ": " << state_to_pressure.size();
    absl::flat_hash_map<State, int> new_state_to_pressure;
    int best_pressure = best_known;
    for (const auto& [s, p] : state_to_pressure) {
      best_pressure =
          std::max(best_pressure, p + flows[s.open_set] * (minutes - r));
    }
    for (const auto& [s, p] : state_to_pressure) {
      int new_pressure = p + flows[s.open_set];
      {
        // The total pressure we can produce from here is bounded by a series
        // of move-1, open, move-1, open steps with open sorted by decreasing
        // pressure. Calculate what that is, and if there's some state that
        // produces more than that we can cut this branch short.
        int best_possible_pressure = new_pressure;
        int flow = flows[s.open_set];
        auto it = ordered_valves.begin();
        for (int j = r + 1; j < minutes; ++j) {
          // Only consider opening valves every other turn to allow for moving.
          if ((j - r) % 2 == 1) {
            for (/*nop*/; it != ordered_valves.end(); ++it) {
              if (s.open_set & (1 << (it - ordered_valves.begin()))) continue;
              flow += (*it)->flow;
              break;
            }
          }
          best_possible_pressure += flow;
        }
        if (best_possible_pressure < best_pressure) {
          continue;
        }
      }

      absl::Status st = s.NextForMe(graph, pack, [&](State s1) {
        if (use_elephant) {
          absl::Status st = s1.NextForEl(graph, pack, [&](State s2) {
            InsertOrUpdateMax(new_state_to_pressure, s2, new_pressure);
            return absl::OkStatus();
          });
          if (!st.ok()) return st;
        } else {
          InsertOrUpdateMax(new_state_to_pressure, s1, new_pressure);
        }
        return absl::OkStatus();
      });
      if (!st.ok()) return st;
    }
    state_to_pressure = std::move(new_state_to_pressure);
  }
  int max = 0;
  for (const auto& [s, p] : state_to_pressure) {
    max = std::max(p, max);
  }
  return max;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<Valve> graph, ParseGraph(input));

  return AdventReturn(BestPath(graph, 30, /*use_elephant=*/false));
}

absl::StatusOr<std::string> Day_2022_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(DirectedGraph<Valve> graph, ParseGraph(input));

  return AdventReturn(BestPath(graph, 26, /*use_elephant=*/true));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/16, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_16());
});

}  // namespace advent_of_code
