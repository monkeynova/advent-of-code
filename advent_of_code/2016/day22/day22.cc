#include "advent_of_code/2016/day22/day22.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Node {
  Point p;
  int used;
  int avail;
  int size;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day22_2016::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Node> nodes;
  for (absl::string_view str: input) {
    if (RE2::FullMatch(str, "root@ebhq-gridcenter# df -h")) continue;
    if (RE2::FullMatch(str, "Filesystem              Size  Used  Avail  Use%")) {
      continue;
    }
    Node n;
    if (!RE2::FullMatch(str, "/dev/grid/node-x(\\d+)-y(\\d+)\\s+(\\d+)T\\s+(\\d+)T\\s+(\\d+)T\\s+\\d+%",
                        &n.p.x, &n.p.y, &n.size, &n.used, &n.avail)) {
      return Error("Bad line: ", str);
    }
    if (n.size != n.used + n.avail) return Error("Bad used: ", str);
    nodes.push_back(n);
  }
  int viable = 0;
  for (int i = 0; i < nodes.size(); ++i) {
    for (int j = 0; j < nodes.size(); ++j) {
      if (i == j) continue;
      if (nodes[i].used > 0 && nodes[i].used < nodes[j].avail) ++viable;

    }
  }
  return IntReturn(viable);
}

absl::StatusOr<std::vector<std::string>> Day22_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
