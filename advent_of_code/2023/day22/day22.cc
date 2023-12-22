// http://adventofcode.com/2023/day/22

#include "advent_of_code/2023/day22/day22.h"

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

absl::StatusOr<std::vector<Cube>> Parse(absl::Span<std::string_view> input) {
  std::vector<Cube> list;
  for (std::string_view line : input) {
    Cube next;
    if (!RE2::FullMatch(line, "(\\d+,\\d+,\\d+)~(\\d+,\\d+,\\d+)", next.min.Capture(), next.max.Capture())) {
      return Error("Bad line");
    }
    int same_count = 0;
    if (next.min.x == next.max.x) ++same_count;
    if (next.min.y == next.max.y) ++same_count;
    if (next.min.z == next.max.z) ++same_count;
    if (same_count < 2) return Error("Not a line: ", line);
    if (next.min.x > next.max.x) return Error("Need swap");
    if (next.min.y > next.max.y) return Error("Need swap");
    if (next.min.z > next.max.z) return Error("Need swap");
    list.push_back(next);
  }
  return list;
}

struct SupportGraph {
  absl::flat_hash_map<int, absl::flat_hash_set<int>> supports;
  absl::flat_hash_map<int, absl::flat_hash_set<int>> supported_by;
};

SupportGraph BuildSupportGraph(const std::vector<Cube>& list) {
  SupportGraph ret;
  for (int i = 0; i < list.size(); ++i) {
    if (list[i].min.z == 1) {
      ret.supports[-1].insert(i);
      ret.supported_by[i].insert(-1);
    }
    Cube drop = list[i];
    --drop.min.z;
    --drop.max.z;
    for (int j = 0; j < list.size(); ++j) {
      if (i == j) continue;
      if (drop.Overlaps(list[j])) {
        ret.supports[j].insert(i);
        ret.supported_by[i].insert(j);
      }
    }
  }
  return ret;
}

SupportGraph Drop(std::vector<Cube>& list) {
if (true) {

  int max_support_z = 1;
  absl::c_sort(list, [](Cube a, Cube b) { return a.min.z < b.min.z; });
  for (int i = 0; i < list.size(); ++i) {
    if (list[i].min.z > max_support_z) {
      list[i].max.z -= list[i].min.z - max_support_z;
      list[i].min.z = max_support_z;
    }
    Cube drop = list[i];
    while (true) {
      --drop.min.z;
      --drop.max.z;
      bool supported = false;
      if (drop.min.z == 0) {
        break;
      }
      for (int j = 0; j < i; ++j) {
        if (drop.Overlaps(list[j])) {
          supported = true;
          break;
        }
      }
      if (supported) break;
      list[i] = drop;
    }
    max_support_z = std::max(max_support_z, list[i].max.z + 1);
  }
 
} else {

  std::vector<bool> supported(list.size(), false);
  while (!absl::c_all_of(supported, [](bool b) { return b; })) {
    for (bool support_changed = true; support_changed;) {
      support_changed = false;
      for (int i = 0; i < list.size(); ++i) {
        if (supported[i]) continue;
        if (list[i].min.z == 1) {
          supported[i] = true;
          support_changed = true;
        }
        Cube drop = list[i];
        --drop.min.z;
        --drop.max.z;
        for (int j = 0; j < list.size(); ++j) {
          if (i == j) continue;
          if (!supported[j]) continue;
          if (drop.Overlaps(list[j])) {
            supported[i] = true;
            support_changed = true;
          }
        }
      }
    }
    for (int i = 0; i < list.size(); ++i) {
      if (supported[i]) continue;
      --list[i].min.z;
      --list[i].max.z;
    }
  }

}

  return BuildSupportGraph(list);
}

}  // namespace

absl::StatusOr<std::string> Day_2023_22::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<Cube> list, Parse(input));
  SupportGraph support = Drop(list);

  int disintigrable = 0;
  for (int i = 0; i < list.size(); ++i) {
    auto it1 = support.supports.find(i);
    if (it1 == support.supports.end()) {
      VLOG(2) << i << ": No supports";
      VLOG(2) << list[i].min << "-" << list[i].max;
      ++disintigrable;
      continue;
    }
    bool all_multi_support = true;
    for (int s : it1->second) {
      auto it2 = support.supported_by.find(s);
      if (it2 == support.supported_by.end()) return Error("Integrity check");
      if (it2->second.size() == 0) return Error("Integrity check");
      if (it2->second.size() == 1) {
        if (!it2->second.contains(i)) return Error("Integrity check");
        all_multi_support = false;
      }
    }
    VLOG(2) << i << ": " << all_multi_support;
    if (all_multi_support) {
      ++disintigrable;
    }
  }

  return AdventReturn(disintigrable);
}

absl::StatusOr<std::string> Day_2023_22::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<Cube> list, Parse(input));
  SupportGraph support = Drop(list);

  int would_fall = 0;
  for (int i = 0; i < list.size(); ++i) {
    absl::flat_hash_set<int> this_supported;
    for (std::deque<int> queue = {-1}; !queue.empty(); queue.pop_front()) {
      this_supported.insert(queue.front());
      auto it = support.supports.find(queue.front());
      if (it == support.supports.end()) continue;
      for (int o : it->second) {
        if (o == i) continue;
        if (this_supported.insert(o).second) {
          queue.push_back(o);
        }
      }
    }
    // this_supported includes 'ground' for extra - 1 to remove i.
    would_fall += list.size() - this_supported.size();
  }

  return AdventReturn(would_fall);
}

}  // namespace advent_of_code
