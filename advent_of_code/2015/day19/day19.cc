#include "advent_of_code/2015/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day19_2015::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  absl::string_view final = input.back();
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>> map;
  for (absl::string_view rule : input.subspan(0, input.size() - 2)) {
    absl::string_view from;
    absl::string_view to;
    if (!RE2::FullMatch(rule, "([A-Za-z]+) => ([A-Za-z]+)", &from, &to)) {
      return Error("Bad rule: ", rule);
    }
    map[from].push_back(to);
  }
  absl::flat_hash_set<std::string> unique;
  for (const auto& pair : map) {
    absl::string_view src = pair.first;
    const std::vector<absl::string_view>& dest_list = pair.second;
    for (int i = 0; i < final.size() - src.size() + 1; ++i) {
      if (final.substr(i, src.size()) == src) {
        for (absl::string_view dest : dest_list) {
          unique.insert(absl::StrCat(final.substr(0, i), dest, final.substr(i + src.size())));
        }
      }
    }
  }
  return IntReturn(unique.size());
}

absl::StatusOr<std::vector<std::string>> Day19_2015::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}
