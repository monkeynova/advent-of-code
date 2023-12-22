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
    Tokenizer tok(line);
    ASSIGN_OR_RETURN(next.min.x, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(next.min.y, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(next.min.z, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs("~"));
    ASSIGN_OR_RETURN(next.max.x, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(next.max.y, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(next.max.z, tok.NextInt());
    if (!tok.Done()) return Error("Extra tokens");
 
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

SupportGraph Drop(std::vector<Cube>& list) {
  VLOG(1) << "Drop Start";
  absl::c_sort(list, [](Cube a, Cube b) { return a.min.z < b.min.z; });
  PointRectangle r = PointRectangle::Null();
  for (Cube c : list) {
    r.ExpandInclude({c.min.x, c.min.y});
    r.ExpandInclude({c.max.x, c.max.y});
  }
  CHECK(r.min == Cardinal::kOrigin);
  std::vector<std::vector<int>> heights(r.max.y + 1, std::vector<int>(r.max.x + 1, 0));
  std::vector<std::vector<int>> support(r.max.y + 1, std::vector<int>(r.max.x + 1, -1));

  SupportGraph ret;
  for (int i = 0; i < list.size(); ++i) {
    PointRectangle xy = {{list[i].min.x, list[i].min.y}, {list[i].max.x, list[i].max.y}};
    int out_z = 0;
    for (Point p : xy) {
      out_z = std::max(out_z, heights[p.y][p.x]);
    }
    for (Point p : xy) {
      if (heights[p.y][p.x] == out_z) {
        int j = support[p.y][p.x];
        ret.supports[j].insert(i);
        ret.supported_by[i].insert(j);
      }
    }
    list[i].max.z -= list[i].min.z - (out_z + 1);
    list[i].min.z = out_z + 1;
    for (Point p : xy) {
      heights[p.y][p.x] = list[i].max.z;
      support[p.y][p.x] = i;
    }
  }

  VLOG(1) << "Drop done";

  return ret;
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
        break;
      }
    }
    VLOG(2) << i << ": " << all_multi_support;
    if (all_multi_support) {
      ++disintigrable;
    }
  }

  VLOG(1) << "Part1 done";

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

  VLOG(1) << "Part2 done";

  return AdventReturn(would_fall);
}

}  // namespace advent_of_code
