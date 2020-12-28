#include "advent_of_code/2017/day16/day16.h"

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

absl::StatusOr<std::vector<std::string>> Day16_2017::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::string line = "abcdefghijklmnop";
  if (line.size() != 16) return Error("WTF");
  std::vector<absl::string_view> ins = absl::StrSplit(input[0], ",");
  for (absl::string_view i : ins) {
    int n1, n2;
    char c1, c2;
    VLOG(1) << line;
    VLOG(1) << i;
    if (RE2::FullMatch(i, "s(\\d+)", &n1)) {
      absl::string_view line_view = line;
      int split = line_view.size() - n1;
      line = absl::StrCat(line_view.substr(split), line_view.substr(0, split));
    } else if (RE2::FullMatch(i, "x(\\d+)/(\\d+)", &n1, &n2)) {
      std::swap(line[n1], line[n2]);
    } else if (RE2::FullMatch(i, "p([a-p])/([a-p])", &c1, &c2)) {
      for (int i = 0; i < line.length(); ++i) {
        if (c1 == line[i]) n1 = i;
        if (c2 == line[i]) n2 = i;
      }
      std::swap(line[n1], line[n2]);
    }
  }
  VLOG(1) << line;
  return std::vector<std::string>{line};
}

absl::StatusOr<std::vector<std::string>> Day16_2017::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  return Error("Not implemented");
}

}  // namespace advent_of_code
