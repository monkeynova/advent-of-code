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

ABSL_FLAG(bool, advent_2022_16_deep, false, "...");

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
    if (!RE2::FullMatch(line,
                        "Valve ([A-Z]+) has flow rate=(\\d+); "
                        "tunnels? leads? to valves? ([A-Z ,]+)",
                        &v.name, &v.flow, &out)) {
      return Error("Bad line: ", line);
    }
    if (v.name.size() != 2) return Error("Bad valve: ", v.name);
    full_flow += v.flow;
    graph.AddNode(v.name, v);
    for (absl::string_view out_name : absl::StrSplit(out, ", ")) {
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

  void NextForMe(const DirectedGraph<Valve>& graph,
                 const absl::flat_hash_map<absl::string_view, int>& pack,
                 absl::FunctionRef<void(State)> on_next) const {
    if (open_set == (1 << pack.size()) - 1) {
      on_next(*this);
      return;
    }
    TryOpenMe(pack, on_next);
    std::string me_str = {static_cast<char>(me >> 8),
                          static_cast<char>(me & 0xFF)};
    CHECK(graph.Outgoing(me_str) != nullptr) << me_str;
    for (absl::string_view out : *graph.Outgoing(me_str)) {
      State s = *this;
      s.MoveMe(out);
      on_next(s);
    }
  }

  void NextForEl(const DirectedGraph<Valve>& graph,
                 const absl::flat_hash_map<absl::string_view, int>& pack,
                 absl::FunctionRef<void(State)> on_next) const {
    if (open_set == (1 << pack.size()) - 1) {
      on_next(*this);
      return;
    }
    TryOpenEl(pack, on_next);
    std::string el_str = {static_cast<char>(el >> 8),
                          static_cast<char>(el & 0xFF)};
    CHECK(graph.Outgoing(el_str) != nullptr) << el_str;
    for (absl::string_view out : *graph.Outgoing(el_str)) {
      State s = *this;
      s.MoveEl(out);
      on_next(s);
    }
  }

  void TryOpenMe(const absl::flat_hash_map<absl::string_view, int>& pack,
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

  void MoveMe(absl::string_view dest) { me = (dest[0] << 8) | dest[1]; }

  void TryOpenEl(const absl::flat_hash_map<absl::string_view, int>& pack,
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

  void MoveEl(absl::string_view dest) { el = (dest[0] << 8) | dest[1]; }

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
  absl::flat_hash_map<absl::string_view, int> pack;
  int64_t all_on = 0;
  int full_flow = 0;
  std::vector<const Valve*> ordered_valves;
  for (absl::string_view n : graph.nodes()) {
    ordered_valves.push_back(graph.GetData(n));
  }
  auto by_flow = [](const Valve* a, const Valve* b) {
    return a->flow > b->flow;
  };
  absl::c_sort(ordered_valves, by_flow);
  for (const Valve* v : ordered_valves) {
    if (v->flow == 0) break;
    all_on |= 1ll << pack.size();
    full_flow += v->flow;
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
    absl::StatusOr<int> just_me =
        BestPath(graph, minutes, /*use_elephant=*/false);
    if (!just_me.ok()) return just_me.status();
    best_known = *just_me;
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

      s.NextForMe(graph, pack, [&](State s1) {
        if (use_elephant) {
          s1.NextForEl(graph, pack, [&](State s2) {
            InsertOrUpdateMax(new_state_to_pressure, s2, new_pressure);
          });
        } else {
          InsertOrUpdateMax(new_state_to_pressure, s1, new_pressure);
        }
      });
    }
    state_to_pressure = std::move(new_state_to_pressure);
  }
  int max = 0;
  for (const auto& [s, p] : state_to_pressure) {
    max = std::max(p, max);
  }
  return max;
}

using MinPathMap =
    absl::flat_hash_map<absl::string_view,
                        std::vector<std::pair<absl::string_view, int>>>;

class CalcMinPath : public BFSInterface<CalcMinPath, absl::string_view> {
 public:
  CalcMinPath(const DirectedGraph<Valve>& graph, absl::string_view src,
              absl::string_view dest)
      : graph_(graph), cur_(src), dest_(dest) {}

  absl::string_view identifier() const override { return cur_; }
  bool IsFinal() const override { return cur_ == dest_; }
  void AddNextSteps(State* state) const override {
    for (absl::string_view next_cur : *graph_.Outgoing(cur_)) {
      CalcMinPath next = *this;
      next.cur_ = next_cur;
      state->AddNextStep(next);
    }
  }

 private:
  const DirectedGraph<Valve>& graph_;
  absl::string_view cur_;
  absl::string_view dest_;
};

MinPathMap MinValvePaths(const DirectedGraph<Valve>& graph) {
  std::vector<absl::string_view> valves;
  for (absl::string_view loc : graph.nodes()) {
    if (graph.GetData(loc)->flow != 0) {
      valves.push_back(loc);
    }
  }
  MinPathMap ret;
  for (absl::string_view dest : valves) {
    absl::string_view src = "AA";
    std::optional<int> dist = CalcMinPath(graph, src, dest).FindMinSteps();
    CHECK(dist);
    ret[src].push_back({dest, *dist});
  }
  for (absl::string_view src : valves) {
    for (absl::string_view dest : valves) {
      std::optional<int> dist = CalcMinPath(graph, src, dest).FindMinSteps();
      CHECK(dist);
      ret[src].push_back({dest, *dist});
    }
  }
  return ret;
}

int FindBestOrderRecursive(
    int depth, const absl::flat_hash_map<absl::string_view, int>& valves,
    const MinPathMap& min_path_map, int minutes, int pressure, int flow,
    absl::string_view cur, absl::flat_hash_set<absl::string_view>& open) {
  std::string prefix(depth * 2, ' ');
  VLOG(2) << prefix << "FindBestOrderRecursive(" << minutes << "," << pressure
          << "," << flow << "," << cur << ")";

  auto next_map_it = min_path_map.find(cur);
  CHECK(next_map_it != min_path_map.end()) << cur;

  int best = pressure + minutes * flow;
  for (const auto& [dest, dist] : next_map_it->second) {
    int time_to_open = dist + 1;
    if (open.contains(dest)) continue;
    if (time_to_open > minutes) continue;
    auto valves_it = valves.find(dest);
    CHECK(valves_it != valves.end()) << dest;
    open.insert(dest);
    int sub = FindBestOrderRecursive(
        depth + 1, valves, min_path_map, minutes - time_to_open,
        pressure + time_to_open * flow, flow + valves_it->second, dest, open);
    open.erase(dest);
    best = std::max(best, sub);
  }
  return best;
}

int FindBestOrder(const absl::flat_hash_map<absl::string_view, int>& valves,
                  const MinPathMap& min_path_map, int minutes) {
  absl::flat_hash_set<absl::string_view> open;
  return FindBestOrderRecursive(
      /*depth=*/0, valves, min_path_map, minutes, /*pressure=*/0, /*flow=*/0,
      /*cur=*/"AA", open);
}

struct TravelState {
  absl::string_view dest;
  int time;
};

int FindBestOrderElephantRecursive(
    int depth, const absl::flat_hash_map<absl::string_view, int>& valves,
    const MinPathMap& min_path_map, int minutes, int pressure, int flow,
    TravelState me, TravelState el,
    absl::flat_hash_set<absl::string_view>& open) {
  std::string prefix(depth * 2, ' ');
  VLOG(2) << prefix << "FindBestOrderRecursive(" << minutes << "," << pressure
          << "," << flow << ","
          << "{" << me.dest << "@" << me.time << "},"
          << "{" << el.dest << "@" << el.time << "}"
          << ")";

  std::vector<absl::string_view> to_close;
  if (me.time != 0 && el.time != 0) {
    int time_to_next = std::min(me.time, el.time);
    me.time -= time_to_next;
    el.time -= time_to_next;
    minutes -= time_to_next;
    pressure += time_to_next * flow;

    if (me.time == 0) {
      auto valves_it = valves.find(me.dest);
      CHECK(valves_it != valves.end()) << me.dest;
      flow += valves_it->second;
      CHECK(!open.contains(me.dest)) << me.dest;
      open.insert(me.dest);
      to_close.push_back(me.dest);
    }
    if (el.time == 0) {
      auto valves_it = valves.find(el.dest);
      CHECK(valves_it != valves.end()) << el.dest;
      flow += valves_it->second;
      CHECK(!open.contains(el.dest)) << el.dest;
      open.insert(el.dest);
      to_close.push_back(el.dest);
    }
    std::string prefix(depth * 2, ' ');
    VLOG(2) << prefix << "FindBestOrderRecursive(" << minutes << "," << pressure
            << "," << flow << ","
            << "{" << me.dest << "@" << me.time << "},"
            << "{" << el.dest << "@" << el.time << "}"
            << ")";
  }

  int best = pressure + minutes * flow;
  CHECK(me.time == 0 || el.time == 0);
  if (me.time == 0) {
    auto next_map_it = min_path_map.find(me.dest);
    CHECK(next_map_it != min_path_map.end()) << me.dest;
    for (const auto& [me_dest, dist] : next_map_it->second) {
      int time_to_open = dist + 1;
      if (me_dest == el.dest) continue;
      if (open.contains(me_dest)) continue;
      if (time_to_open > minutes) continue;

      TravelState new_me = {.dest = me_dest, .time = time_to_open};

      int sub = FindBestOrderElephantRecursive(depth + 1, valves, min_path_map,
                                               minutes, pressure, flow, new_me,
                                               el, open);

      best = std::max(best, sub);
      if (depth < 2) {
        VLOG(1) << prefix << "best: " << best;
      }
    }
  } else if (el.time == 0) {
    auto next_map_it = min_path_map.find(el.dest);
    CHECK(next_map_it != min_path_map.end()) << el.dest;
    for (const auto& [el_dest, dist] : next_map_it->second) {
      int time_to_open = dist + 1;
      if (el_dest == me.dest) continue;
      if (open.contains(el_dest)) continue;
      if (time_to_open > minutes) continue;

      TravelState new_el = {.dest = el_dest, .time = time_to_open};

      int sub = FindBestOrderElephantRecursive(depth + 1, valves, min_path_map,
                                               minutes, pressure, flow, me,
                                               new_el, open);

      best = std::max(best, sub);
      if (depth < 2) {
        VLOG(1) << prefix << "best: " << best;
      }
    }
  }

  for (absl::string_view valve : to_close) {
    CHECK(open.contains(valve)) << valve;
    open.erase(valve);
  }

  return best;
}

int FindBestOrderElephant(
    const absl::flat_hash_map<absl::string_view, int>& valves,
    const MinPathMap& min_path_map, int minutes) {
  absl::flat_hash_set<absl::string_view> open;
  return FindBestOrderElephantRecursive(
      /*depth=*/0, valves, min_path_map, minutes, /*pressure=*/0, /*flow=*/0,
      /*me=*/{.dest = "AA", .time = 0}, /*el=*/{.dest = "AA", .time = 0}, open);
}

}  // namespace

absl::StatusOr<std::string> Day_2022_16::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<Valve>> graph = ParseGraph(input);
  if (!graph.ok()) return graph.status();

  if (absl::GetFlag(FLAGS_advent_2022_16_deep)) {
    absl::flat_hash_map<absl::string_view, int> valves;
    for (absl::string_view loc : graph->nodes()) {
      if (graph->GetData(loc)->flow != 0) {
        valves.insert({loc, graph->GetData(loc)->flow});
      }
    }

    MinPathMap min_paths = MinValvePaths(*graph);
    return IntReturn(FindBestOrder(valves, min_paths, 30));
  }

  return IntReturn(BestPath(*graph, 30, /*use_elephant=*/false));
}

absl::StatusOr<std::string> Day_2022_16::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<DirectedGraph<Valve>> graph = ParseGraph(input);
  if (!graph.ok()) return graph.status();

  if (absl::GetFlag(FLAGS_advent_2022_16_deep)) {
    absl::flat_hash_map<absl::string_view, int> valves;
    valves.insert({"AA", 0});
    for (absl::string_view loc : graph->nodes()) {
      if (graph->GetData(loc)->flow != 0) {
        valves.insert({loc, graph->GetData(loc)->flow});
      }
    }

    MinPathMap min_paths = MinValvePaths(*graph);
    return IntReturn(FindBestOrderElephant(valves, min_paths, 26));
  }

  return IntReturn(BestPath(*graph, 26, /*use_elephant=*/true));
}

}  // namespace advent_of_code
