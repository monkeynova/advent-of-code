#include "advent_of_code/2017/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day17_2017::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  int n;
  if (!absl::SimpleAtoi(input[0], &n)) return Error("Bad int:", input[0]);
  int pos = 0;
  std::vector<int> buf = {0};
  for (int i = 0; i < 2017; ++i) {
    VLOG(1) << absl::StrJoin(buf, ",");
    pos = (pos + n) % buf.size();
    buf.push_back(0);
    ++pos;
    for (int j = buf.size() - 1; j > pos; --j) {
      buf[j] = buf[j - 1];
    }
    buf[pos] = i + 1;
  }
  return IntReturn(buf[(pos + 1) % buf.size()]);
}

absl::StatusOr<std::vector<std::string>> Day17_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
