#include "advent_of_code/2015/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

absl::flat_hash_set<std::string> RunStep(
    const absl::flat_hash_map<std::string_view, std::vector<std::string_view>>&
        map,
    std::string_view input) {
  absl::flat_hash_set<std::string> unique;
  for (const auto& pair : map) {
    std::string_view src = pair.first;
    const std::vector<std::string_view>& dest_list = pair.second;
    for (int i = 0; i < input.size() - src.size() + 1; ++i) {
      if (input.substr(i, src.size()) == src) {
        for (std::string_view dest : dest_list) {
          unique.insert(absl::StrCat(input.substr(0, i), dest,
                                     input.substr(i + src.size())));
        }
      }
    }
  }
  return unique;
}

int FindMinPath(const absl::flat_hash_map<std::string_view,
                                          std::vector<std::string_view>>& map,
                std::string_view src, std::string_view dest) {
  struct Path {
    std::string str;
    int dist;
  };
  std::deque<Path> frontier = {{.str = std::string(src), .dist = 0}};
  absl::flat_hash_set<std::string> hist;
  hist.insert(std::string(src));
  VLOG(2) << "FindMinPath: " << src << " => " << dest;
  for (int64_t i = 0; !frontier.empty(); ++i) {
    const Path& p = frontier.front();
    VLOG_IF(2, i % 7777 == 0) << "expanding: " << p.str << "; " << i << "; "
                              << p.dist << "; " << frontier.size();
    for (const auto& [src, dest_list] : map) {
      for (int j = 0; j < p.str.size() - src.size() + 1; ++j) {
        if (p.str.substr(j, src.size()) == src) {
          for (std::string_view test_dest : dest_list) {
            std::string next = absl::StrCat(p.str.substr(0, j), test_dest,
                                            p.str.substr(j + src.size()));
            if (next == dest) return p.dist + 1;
            if (next.size() > dest.size()) continue;
            if (hist.contains(next)) continue;
            hist.insert(next);
            frontier.push_back({.str = std::move(next), .dist = p.dist + 1});
          }
        }
      }
    }
    frontier.pop_front();
  }

  return -1;
}

std::string_view FirstElement(std::string_view str) {
  if (str.size() < 2) return str;
  if (str[1] >= 'a' && str[1] <= 'z') {
    return str.substr(0, 2);
  }
  return str.substr(0, 1);
}

}  // namespace

absl::StatusOr<std::string> Day_2015_19::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  std::string_view final = input.back();
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::flat_hash_map<std::string_view, std::vector<std::string_view>> map;
  for (std::string_view rule : input.subspan(0, input.size() - 2)) {
    std::string_view from;
    std::string_view to;
    if (!RE2::FullMatch(rule, "([A-Za-z]+) => ([A-Za-z]+)", &from, &to)) {
      return Error("Bad rule: ", rule);
    }
    map[from].push_back(to);
  }
  return AdventReturn(RunStep(map, final).size());
}

absl::StatusOr<std::string> Day_2015_19::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  std::string_view target = input.back();
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::flat_hash_map<std::string_view, std::vector<std::string_view>> map;
  std::map<std::string_view, std::string_view> reverse;
  for (std::string_view rule : input.subspan(0, input.size() - 2)) {
    std::string_view from;
    std::string_view to;
    if (!RE2::FullMatch(rule, "(e|[A-Z][a-z]?) => ([A-Za-z]+)", &from, &to)) {
      return Error("Bad rule: ", rule);
    }
    if (from.length() > to.length()) return Error("Can't handle reductions");
    map[from].push_back(to);
    if (reverse.find(to) != reverse.end()) return Error("Can't invert");
    reverse[to] = from;
  }

  bool checklist_match = true;
  for (const auto& [from, to_list] : map) {
    if (!checklist_match) break;
    for (std::string_view to : to_list) {
      // 'Ar' always terminates a rule.
      checklist_match &= !RE2::PartialMatch(to, "Ar.");
      VLOG_IF(1, !checklist_match) << from << " -> " << to;
      // 'Rn' always has a matching 'Ar'.
      checklist_match &=
          !RE2::PartialMatch(to, "Rn.*Rn") &&
          (!RE2::PartialMatch(to, "Rn") || RE2::PartialMatch(to, "Rn.*Ar"));
      // 'F' never goes to 'Rn..Ar'.
      checklist_match &= (from != "F") || !RE2::PartialMatch(to, "Rn.*Ar");
      VLOG_IF(1, !checklist_match) << from << " -> " << to;
      // Initial rule sets 2 elements.
      checklist_match &=
          (from != "e") || RE2::FullMatch(to, "[A-Z][a-z]?[A-Z][a-z]?");
      VLOG_IF(1, !checklist_match) << from << " -> " << to;
      // All rules that don't have Rn..Ar add one element.
      checklist_match &= RE2::PartialMatch(to, "Rn.*Ar") ||
                         RE2::FullMatch(to, "[A-Z][a-z]?[A-Z][a-z]?");
      VLOG_IF(1, !checklist_match) << from << " -> " << to;
      // All rules that add Rn..Ar (without Y) add 3.
      // All rules that add Rn..Y..Ar (with a single Y) add 5.
      // All rules that add Rn..Y..Y..Ar can't further match Rn..Ar since target
      // never has Rn..Y..Y..Ar.
      if (RE2::FullMatch(to, "[A-Z][a-z]?RnF(YFYF)?Ar")) {
        checklist_match &= !RE2::PartialMatch(target, "Rn[^R]*Y[^R]*Y[^R]*Ar");
        VLOG_IF(1, !checklist_match) << from << " -> " << to;
      } else {
        checklist_match &=
            !RE2::PartialMatch(to, "Rn.*Ar") ||
            RE2::FullMatch(to, "[A-Z][a-z]?Rn[A-Z][a-z]?(Y[A-Z][a-z]?)?Ar");
        VLOG_IF(1, !checklist_match) << from << " -> " << to;
      }
      if (!checklist_match) break;
    }
  }
  if (checklist_match) {
    // steps is # of elements - 1 (start) - 2 * count('Ar' == 'Rn') - 2 * count
    // 'Y'.
    int steps = -1;
    for (int off = 0; off < target.size();) {
      std::string_view next_elem = FirstElement(target.substr(off));
      off += next_elem.size();
      ++steps;
      if (next_elem == "Ar") --steps;
      if (next_elem == "Rn") --steps;
      if (next_elem == "Y") steps -= 2;
    }
    return AdventReturn(steps);
  }

  return AdventReturn(FindMinPath(map, "e", target));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2015, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2015_19()); });

}  // namespace advent_of_code
