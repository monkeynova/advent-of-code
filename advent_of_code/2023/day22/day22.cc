// http://adventofcode.com/2023/day/22

#include "advent_of_code/2023/day22/day22.h"

#include "absl/algorithm/container.h"
#include "absl/log/log.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/tokenizer.h"

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

class SupportGraph {
 public:
  explicit SupportGraph(int size)
   : supports_(size + 1), supported_by_(size + 1) {}

  void Add(int supportee, int supported) {
    supports_[supportee].push_back(supported);
    supported_by_[supported ].push_back(supportee);
  }

  const std::vector<int>& Supports(int supported) {
    return supports_[supported];
  }

  const std::vector<int>& SupportedBy(int supportee) {
    return supported_by_[supportee];
  }

 private:
  static const std::vector<int> kEmpty;
  std::vector<std::vector<int>> supports_;
  std::vector<std::vector<int>> supported_by_;
};

const std::vector<int> SupportGraph::kEmpty;

SupportGraph Drop(std::vector<Cube>& list) {
  VLOG(1) << "Drop Start";
  absl::c_sort(list, [](Cube a, Cube b) { return a.min.z < b.min.z; });
  PointRectangle r = PointRectangle::Null();
  for (Cube c : list) {
    r.ExpandInclude({c.min.x, c.min.y});
    r.ExpandInclude({c.max.x, c.max.y});
  }
  CHECK(r.min == Cardinal::kOrigin);

  const int kFloorIndex = list.size();

  std::vector<std::vector<int>> heights(
      r.max.y + 1, std::vector<int>(r.max.x + 1, 0));
  std::vector<std::vector<int>> support(
      r.max.y + 1, std::vector<int>(r.max.x + 1, kFloorIndex));

  SupportGraph ret(list.size() + 1);
  for (int i = 0; i < list.size(); ++i) {
    PointRectangle xy =
        {{list[i].min.x, list[i].min.y}, {list[i].max.x, list[i].max.y}};
    int out_z = 0;
    for (Point p : xy) {
      out_z = std::max(out_z, heights[p.y][p.x]);
    }
    std::vector<bool> inserted(list.size() + 1, false);
    for (Point p : xy) {
      if (heights[p.y][p.x] == out_z) {
        int j = support[p.y][p.x];
        if (inserted[j]) continue;
        inserted[j] = true;
        ret.Add(j, i);
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
    bool all_multi_support = true;
    for (int s : support.Supports(i)) {
      if (support.SupportedBy(s).size() == 1) {
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
    std::deque<int> queue = {i};
    std::vector<bool> falling(list.size() + 1, false);
    falling[i] = true;
    int falling_count = 1;

    for (; !queue.empty(); queue.pop_front()) {
      for (int s : support.Supports(queue.front())) {
        bool supported = false;
        for (int os : support.SupportedBy(s)) {
          if (!falling[os]) {
            supported = true;
          }
        }
        if (!supported) {
          if (!falling[s]) {
            falling[s] = true;
            ++falling_count;
            queue.push_back(s);
          }
        }
      }
    }
    would_fall += falling_count - 1;
  }
  VLOG(1) << "Part2 done";

  return AdventReturn(would_fall);
}

}  // namespace advent_of_code
