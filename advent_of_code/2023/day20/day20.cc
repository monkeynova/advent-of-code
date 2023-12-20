// http://adventofcode.com/2023/day/20

#include "advent_of_code/2023/day20/day20.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Modules {
 public:
  struct Control {
    enum { kFlipFlop, kConjunct, kBroadcast } type;
    bool flip_flop_state = false;
    absl::flat_hash_map<std::string_view, bool> conjuncts;
    std::vector<std::string_view> outputs;
  };

  static absl::StatusOr<Modules> Parse(absl::Span<std::string_view> input);

  void InitializeConjuncts();
  void SendPulses(int* low_pulse_count, int* high_pulse_count);
  std::optional<std::pair<int, int>> SendPulses(std::string_view find);

  std::vector<Modules> DisjointSubmodules(std::string_view* final_conj);

  std::vector<bool> State();

 private:
  Modules() = default;

  absl::flat_hash_map<std::string_view, Control> modules_;
  std::vector<std::string_view> flip_flops_;
};

absl::StatusOr<Modules> Modules::Parse(absl::Span<std::string_view> input) {
  Modules ret;
  for (std::string_view line : input) {
    auto [name, output_list] = PairSplit(line, " -> ");
    Control control;
    control.type = Control::kBroadcast;
    if (name[0] == '%') {
      control.type = Control::kFlipFlop;
      name = name.substr(1);
      control.flip_flop_state = 0;
    } else if (name[0] == '&') {
      control.type = Control::kConjunct;
      name = name.substr(1);
    }
    if (control.type == Control::kBroadcast && name != "broadcaster") {
      return Error("Bad name: ", name);
    }
    control.outputs = absl::StrSplit(output_list, ", ");
    if (!ret.modules_.emplace(name, control).second) {
      return Error("Duplicate name: ", name);
    }
  }
  return ret;
}

void Modules::InitializeConjuncts() {
  for (const auto& [name, con] : modules_) {
    for (std::string_view dest : con.outputs) {
      auto it2 = modules_.find(dest);
      if (it2 == modules_.end()) {
        continue;
      }
      if (it2->second.type == Control::kConjunct) {
        it2->second.conjuncts.emplace(name, false);
      }
    }
  }
}

void Modules::SendPulses(int* low_pulse_count, int* high_pulse_count) {
  struct Pulse {
    bool high;
    std::string_view dest;
    std::string_view src;
  };

  for (std::deque<Pulse> queue = {{false, "broadcaster", "button"}};
       !queue.empty(); queue.pop_front()) {
    Pulse cur = queue.front();
    VLOG(2) << cur.src << " -" << (cur.high ? "high" : "low") << "- -> " << cur.dest;
    if (cur.high) ++*high_pulse_count;
    else ++*low_pulse_count;

    auto it = modules_.find(cur.dest);
    if (it == modules_.end()) {
      continue;
    }
    Control& control = it->second;
    switch (control.type) {
      case Control::kBroadcast: {
        for (std::string_view d: control.outputs) {
          queue.push_back({cur.high, d, cur.dest});
        }
        break;
      }
      case Control::kFlipFlop: {
        if (!cur.high) {
          control.flip_flop_state = !control.flip_flop_state;
          for (std::string_view d: control.outputs) {
            queue.push_back({control.flip_flop_state, d, cur.dest});
          }
        }
        break;
      }
      case Control::kConjunct: {
        auto it2 = control.conjuncts.find(cur.src);
        CHECK(it2 != control.conjuncts.end()) << cur.src;
        it2->second = cur.high;
        bool out = !absl::c_all_of(
          control.conjuncts,
          [](std::pair<std::string_view, bool> p) { return p.second; });
        for (std::string_view d: control.outputs) {
          queue.push_back({out, d, cur.dest});
        }
        break;
      }
    }
  }
}

std::optional<std::pair<int, int>> Modules::SendPulses(std::string_view find) {
  struct Pulse {
    bool high;
    std::string_view dest;
    std::string_view src;
  };

  std::optional<std::pair<int, int>> ret;

  int pulse = 0;
  for (std::deque<Pulse> queue = {{false, "broadcaster", "button"}};
       !queue.empty(); queue.pop_front(), ++pulse) {
    Pulse cur = queue.front();
    VLOG(2) << cur.src << " -" << (cur.high ? "high" : "low") << "- -> " << cur.dest;

    if (cur.dest == find) {
      if (cur.high) {
        CHECK(!ret) << "Double high";
        ret = {pulse, pulse - 1};
      } else {
        if (ret && ret->second < ret->first) {
          ret->second = pulse;
        }
      }
    }

    auto it = modules_.find(cur.dest);
    if (it == modules_.end()) {
      continue;
    }
    Control& control = it->second;
    switch (control.type) {
      case Control::kBroadcast: {
        for (std::string_view d: control.outputs) {
          queue.push_back({cur.high, d, cur.dest});
        }
        break;
      }
      case Control::kFlipFlop: {
        if (!cur.high) {
          control.flip_flop_state = !control.flip_flop_state;
          for (std::string_view d: control.outputs) {
            queue.push_back({control.flip_flop_state, d, cur.dest});
          }
        }
        break;
      }
      case Control::kConjunct: {
        auto it2 = control.conjuncts.find(cur.src);
        CHECK(it2 != control.conjuncts.end()) << cur.src;
        it2->second = cur.high;
        bool out = !absl::c_all_of(
          control.conjuncts,
          [](std::pair<std::string_view, bool> p) { return p.second; });
        for (std::string_view d: control.outputs) {
          queue.push_back({out, d, cur.dest});
        }
        break;
      }
    }
  }
  return ret;
}

std::vector<bool> Modules::State() {
  std::vector<bool> state;
  for (std::string_view node : flip_flops_) {
    state.push_back(modules_[node].flip_flop_state);
  }
  return state;
}

std::vector<Modules> Modules::DisjointSubmodules(std::string_view* final_conj) {
  *final_conj = "";
  for (const auto& [name, con] : modules_) {
    for (std::string_view dest : con.outputs) {
      if (dest == "rx") {
        CHECK_EQ(con.outputs.size(), 1);
        *final_conj = name;
      }
    }
  }
  CHECK(!final_conj->empty());

  DirectedGraph<Control> graph;
  for (const auto& [name, con] : modules_) {
    if (name != "broadcaster" && name != *final_conj) {
      graph.AddNode(name, con);
      for (std::string_view out : con.outputs) {
        graph.AddEdge(name, out);
      }
    }
  }
  std::vector<std::vector<std::string_view>> forest = graph.Forest();
  VLOG(1) << forest.size();
  VLOG(1) << absl::StrJoin(
    modules_[*final_conj].conjuncts, ",",
    [](std::string* out, std::pair<std::string_view, bool> p) {
      absl::StrAppend(out, p.first);
    });
    
  std::vector<Modules> ret;
  for (int i = 0; i < forest.size(); ++i) {
    ret.push_back(Modules());
    absl::flat_hash_map<std::string_view, Control> sub_modules;
    ret.back().modules_["broadcaster"] = modules_["broadcaster"];
    for (std::string_view node : forest[i]) {
      ret.back().modules_[node] = modules_[node];
      if (ret.back().modules_[node].type == Control::kFlipFlop) {
        ret.back().flip_flops_.push_back(node);
      }
    }
  }
  return ret;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_20::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(Modules modules, Modules::Parse(input));
  int low_pulses = 0;
  int high_pulses = 0;
  modules.InitializeConjuncts();
  for (int i = 0; i < 1000; ++i) {
    modules.SendPulses(&low_pulses, &high_pulses);
  }
  return AdventReturn(low_pulses * high_pulses);
}

absl::StatusOr<std::string> Day_2023_20::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(Modules modules, Modules::Parse(input));

  modules.InitializeConjuncts();
  
  std::string_view final_conj;
  std::vector<Modules> sub_modules = modules.DisjointSubmodules(&final_conj);

  std::vector<std::pair<int64_t, int64_t>> chinese_remainder;
  std::optional<std::pair<int, int>> pulse_range;

  for (Modules& sub_module : sub_modules) {
    absl::flat_hash_map<int, std::pair<int, int>> on_range;
    absl::flat_hash_map<std::vector<bool>, int> hist;
    for (int i = 0; true; ++i) {
      std::optional<std::pair<int, int>> high_range = sub_module.SendPulses(final_conj);
      std::vector<bool> state = sub_module.State();
      if (high_range) {
        VLOG(1) << i << ": [" << high_range->first << "," << high_range->second << ")";
        on_range[i] = *high_range;
      }
      auto [it, inserted] = hist.emplace(state, i);
      if (!inserted) {
        if (on_range.size() != 1) {
          return absl::UnimplementedError("Can't handle part2 without singular range");
        }
        std::pair<int, int> new_pulse_range = on_range.begin()->second;
        if (!pulse_range) {
          pulse_range = new_pulse_range;
        } else {
          pulse_range->first = std::max(pulse_range->first, new_pulse_range.first);
          pulse_range->second = std::min(pulse_range->second, new_pulse_range.second);
          if (pulse_range->first > pulse_range->second) {
            return absl::UnimplementedError("Can't handle part2 without overlapping range");
          }
        }
        chinese_remainder.push_back({on_range.begin()->first, i});
        VLOG(1) << "Loop @" << i << " -> " << it->second;
        break;
      }
    }
  }
  std::optional<int64_t> found = ChineseRemainder(chinese_remainder);
  if (!found) return AdventReturn(found);
  return AdventReturn(*found + 1);

  return Error("Takes too long, not implemented, yadda");
}

}  // namespace advent_of_code
