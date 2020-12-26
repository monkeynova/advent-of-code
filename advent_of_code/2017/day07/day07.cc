#include "advent_of_code/2017/day07/day07.h"

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

absl::StatusOr<std::vector<std::string>> Day07_2017::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<absl::string_view> parents;
  absl::flat_hash_set<absl::string_view> children;
  for (absl::string_view str : input) {
    std::vector<absl::string_view> p_and_c_list = absl::StrSplit(str, " -> ");
    if (p_and_c_list.size() > 2) return Error("Bad input: ", str);
    if (p_and_c_list.size() == 1) continue;
    for (absl::string_view child : absl::StrSplit(p_and_c_list[1], ", ")) {
      children.insert(child);
    }
    absl::string_view parent;
    if (!RE2::FullMatch(p_and_c_list[0], "(.*) \\(\\d+\\)", &parent)) {
      return Error("Bad parent: ", p_and_c_list[0]);
    }
    parents.insert(parent);
  }
  absl::string_view root;
  for (absl::string_view p : parents) {
    if (!children.contains(p)) {
      if (root != "") return Error("Dupe roots");
      root = p;
    }
  }
  return std::vector<std::string>{std::string(root)};
}

absl::StatusOr<std::vector<std::string>> Day07_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
