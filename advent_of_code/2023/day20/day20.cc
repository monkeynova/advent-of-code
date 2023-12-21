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
    std::vector<std::pair<int, bool>> conjuncts;
    std::vector<int> outputs;
  };

  static absl::StatusOr<Modules> Parse(absl::Span<std::string_view> input);

  void InitializeConjuncts();

  void SendPulses(absl::FunctionRef<void(std::string_view, bool, int)> on_pulse);

  std::vector<Modules> DisjointSubmodules(std::string_view final,
                                          std::string_view* final_conj);

  std::vector<bool> State();

 private:
  Modules() = default;

  int broadcast_id_ = -1;
  std::vector<Control> modules_;
  std::vector<std::string_view> names_;
  std::vector<int> flip_flops_;
};

absl::StatusOr<Modules> Modules::Parse(absl::Span<std::string_view> input) {
  absl::flat_hash_map<std::string_view, int> name_to_id;
  Modules ret;
  auto alloc_name = [&](std::string_view name) {
    auto [name_it, inserted] = name_to_id.emplace(name, ret.names_.size());
    if (inserted) {
      ret.names_.push_back(name);
      ret.modules_.push_back({});
    }
    return name_it->second;
  };

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
    int name_id = alloc_name(name);
    if (control.type == Control::kBroadcast) {
      if (name != "broadcaster") {
        return Error("Bad name: ", name);
      }
      ret.broadcast_id_ = name_id;
    }
    for (std::string_view out : absl::StrSplit(output_list, ", ")) {
      control.outputs.push_back(alloc_name(out));
    }
    ret.modules_[name_id] = std::move(control);
  }
  return ret;
}

void Modules::InitializeConjuncts() {
  for (int src = 0; src < modules_.size(); ++src) {
    for (int dest : modules_[src].outputs) {
      if (modules_[dest].type == Control::kConjunct) {
        modules_[dest].conjuncts.emplace_back(src, false);
      }
    }
  }
}

void Modules::SendPulses(absl::FunctionRef<void(std::string_view, bool, int)> on_pulse) {
  struct Pulse {
    bool high;
    int dest;
    int src;
  };

  int pulse_num = 0;
  for (std::deque<Pulse> queue = {{false, broadcast_id_, -1}};
       !queue.empty(); queue.pop_front(), ++pulse_num) {
    Pulse cur = queue.front();
    VLOG(2) << cur.src << " -" << (cur.high ? "high" : "low") << "- -> " << cur.dest;
    on_pulse(names_[cur.dest], cur.high, pulse_num);

    Control& control = modules_[cur.dest];
    switch (control.type) {
      case Control::kBroadcast: {
        for (int d : control.outputs) {
          queue.push_back({cur.high, d, cur.dest});
        }
        break;
      }
      case Control::kFlipFlop: {
        if (!cur.high) {
          control.flip_flop_state = !control.flip_flop_state;
          for (int d : control.outputs) {
            queue.push_back({control.flip_flop_state, d, cur.dest});
          }
        }
        break;
      }
      case Control::kConjunct: {
        bool out = false;
        for (auto& [id, val] : control.conjuncts) {
          if (id == cur.src) val = cur.high;
          out |= !val;
        }
        for (int d : control.outputs) {
          queue.push_back({out, d, cur.dest});
        }
        break;
      }
    }
  }
}

std::vector<bool> Modules::State() {
  std::vector<bool> state;
  for (int node : flip_flops_) {
    state.push_back(modules_[node].flip_flop_state);
  }
  return state;
}

std::vector<Modules> Modules::DisjointSubmodules(
    std::string_view final, std::string_view* final_conj) {
  *final_conj = "";
  for (int i = 0; i < modules_.size(); ++i) {
    for (int dest : modules_[i].outputs) {
      if (names_[dest] == final) {
        if (modules_[i].outputs.size() != 1) {
          return {*this};
        }
        *final_conj = names_[i];
      }
    }
  }
  if (final_conj->empty()) {
    return {*this};
  }

  std::vector<Modules> ret;
  for (int piece : modules_[broadcast_id_].outputs) {
    ret.push_back(Modules());
    Modules& build = ret.back();
    build.modules_.resize(modules_.size());
    build.names_ = names_;
    build.broadcast_id_ = broadcast_id_;
    build.modules_[broadcast_id_] = modules_[broadcast_id_];
    std::vector<bool> used(modules_.size(), false);
    for (std::deque<int> queue = {piece}; !queue.empty(); queue.pop_front()) {
      int id = queue.front();
      build.modules_[id] = modules_[id];
      if (build.modules_[id].type == Control::kFlipFlop) {
        build.flip_flops_.push_back(id);
      }
      for (int o : modules_[id].outputs) {
        if (!used[o]) {
          queue.push_back(o);
          used[o] = true;
        }
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
    modules.SendPulses([&](std::string_view dest, bool high, int pulse_num) {
      if (high) ++high_pulses;
      else ++low_pulses;
    });
  }
  return AdventReturn(low_pulses * high_pulses);
}

absl::StatusOr<std::string> Day_2023_20::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(Modules modules, Modules::Parse(input));

  modules.InitializeConjuncts();
  
  std::string_view final_conj;
  std::vector<Modules> sub_modules =
      modules.DisjointSubmodules("rx", &final_conj);

  // TODO(@monkeynova): Each of sub_modules is a counter with a comparison. If
  // we could recognized the structure and order the bits, we could extract the
  // values directly.

  std::vector<std::pair<int64_t, int64_t>> chinese_remainder;
  std::optional<std::pair<int, int>> pulse_range;

  for (Modules& sub_module : sub_modules) {
    std::optional<std::pair<int, int>> on_range;
    int on_range_at = -1;
    std::vector<bool> start_state = sub_module.State();
    for (int i = 0; true; ++i) {
      std::optional<std::pair<int, int>> high_range;
      sub_module.SendPulses([&](std::string_view dest, bool high, int pulse) {
        if (dest != final_conj) return;
        if (high) {
          CHECK(!high_range) << "Double high";
          high_range = {pulse, pulse - 1};
        } else {
          if (high_range && high_range->second < high_range->first) {
            high_range->second = pulse;
          }
        }
      });
      std::vector<bool> state = sub_module.State();
      if (high_range) {
        VLOG(1) << i << ": [" << high_range->first << "," << high_range->second
                << ")";
        if (on_range) {
          return absl::UnimplementedError(
              "Can't handle part2 without singular range");
        }
        on_range = *high_range;
        on_range_at = i;
      }
      if (state == start_state) {
        VLOG(1) << "Loop @" << i + 1 << " -> 0";
        if (!pulse_range) {
          pulse_range = *on_range;
        } else {
          pulse_range->first = std::max(pulse_range->first, on_range->first);
          pulse_range->second = std::min(pulse_range->second, on_range->second);
          if (pulse_range->first > pulse_range->second) {
            return absl::UnimplementedError(
                "Can't handle part2 without overlapping range");
          }
        }
        chinese_remainder.push_back({on_range_at, i + 1});
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
