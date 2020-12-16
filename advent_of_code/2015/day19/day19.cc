#include "advent_of_code/2015/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::flat_hash_set<std::string> RunStep(
  const absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>& map,
  absl::string_view input) {
  absl::flat_hash_set<std::string> unique;
  for (const auto& pair : map) {
    absl::string_view src = pair.first;
    const std::vector<absl::string_view>& dest_list = pair.second;
    for (int i = 0; i < input.size() - src.size() + 1; ++i) {
      if (input.substr(i, src.size()) == src) {
        for (absl::string_view dest : dest_list) {
          unique.insert(absl::StrCat(input.substr(0, i), dest, input.substr(i + src.size())));
        }
      }
    }
  }
  return unique;
}

#if 0
ParseTree BuildParseTree(
    const std::map<absl::string_view, absl::string_view>& map) {
  for (const auto& [dest, src] : map) {
    //...
  }
}
#endif

int FindMinPath(const absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>& map,
                absl::string_view src, absl::string_view dest) {
  struct Path {
    std::string str;
    int dist;
  };
  std::deque<Path> frontier = {{.str = std::string(src), .dist = 0}};
  absl::flat_hash_set<std::string> hist;
  hist.insert(std::string(src));
  for (int64_t i = 0; !frontier.empty(); ++i) {
    const Path& p = frontier.front();
    VLOG_IF(1, i % 7777 == 0) << "expanding: " << p.str << "; " << i << "; " << p.dist << "; " << frontier.size();
    for (const std::string& expansion : RunStep(map, p.str)) {
      if (expansion == dest) return p.dist + 1;
      if (expansion.size() > dest.size()) continue;
      if (hist.contains(expansion)) continue;
      hist.insert(expansion);
      frontier.push_back({.str = expansion, .dist = p.dist + 1});
    }
    frontier.pop_front();
  }

  return -1;
}

absl::optional<int> FindMinPathReverseImpl(
    const std::map<absl::string_view, absl::string_view>& map,
    const std::vector<absl::string_view>& map_by_len,
    absl::flat_hash_map<std::string, absl::optional<int>>* memo,
    absl::string_view src, absl::string_view dest) {
  if (auto it = memo->find(dest); it != memo->end()) {
    return it->second;
  }
  VLOG(2) << "FindMinPathReverse: " << src << "; " << dest;
  if (src == dest) return 0;
  absl::optional<int> min;
  // If we find a cycle, bomb out as impossible.
  memo->emplace(dest, min);
  int found_len;
  for (absl::string_view find : map_by_len) {
    if (find.size() > dest.size()) continue;
    auto it = map.find(find);
    CHECK(it != map.end());
    CHECK(it->first == find);
    if (min && found_len > find.size()) break;
    for (int i = 0; i <= dest.size() - find.size(); ++i) {
      VLOG(3) << "Trying: " << dest << "@" << i << "; " << it->first << " => " << it->second;
      if (dest.substr(i, find.size()) == find) {
        std::string sub_dest = absl::StrCat(dest.substr(0, i), it->second, dest.substr(i + find.size()));
        absl::optional<int> sub_min = FindMinPathReverseImpl(map, map_by_len, memo, src, sub_dest);
        if (sub_min) {
          if (!min || *min > *sub_min + 1) {
            min = *sub_min + 1;
            found_len = find.size();
          }
        }
      }
    }
  }
  memo->emplace(dest, min);
  return min;
}

absl::optional<int> FindMinPathReverse(const std::map<absl::string_view, absl::string_view>& map,
                       absl::string_view src, absl::string_view dest) {
  std::vector<absl::string_view> map_by_len;
  for (const auto& pair : map) map_by_len.push_back(pair.first);
  std::sort(map_by_len.begin(), map_by_len.end(),
  [](absl::string_view a, absl::string_view b) {
    if (a.size() != b.size()) return a.size() > b.size();
    return a < b;
  });
  absl::flat_hash_map<std::string, absl::optional<int>> memo;
  return FindMinPathReverseImpl(map, map_by_len, &memo, src, dest);
}

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
  return IntReturn(RunStep(map, final).size());
}

absl::StatusOr<std::vector<std::string>> Day19_2015::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  absl::string_view final = input.back();
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>> map;
  std::map<absl::string_view, absl::string_view> reverse;
  for (absl::string_view rule : input.subspan(0, input.size() - 2)) {
    absl::string_view from;
    absl::string_view to;
    if (!RE2::FullMatch(rule, "([A-Za-z]+) => ([A-Za-z]+)", &from, &to)) {
      return Error("Bad rule: ", rule);
    }
    if (from.length() > to.length()) return Error("Can't handle reductions");
    map[from].push_back(to);
    if (reverse.find(to) != reverse.end()) return Error("Can't invert");
    reverse[to] = from;
  }

  return IntReturn(FindMinPathReverse(reverse, "e", final));
}
