// https://adventofcode.com/2015/day/19
//
// --- Day 19: Medicine for Rudolph ---
// ------------------------------------
//
// Rudolph the Red-Nosed Reindeer is sick! His nose isn't shining very
// brightly, and he needs medicine.
//
// Red-Nosed Reindeer biology isn't similar to regular reindeer biology;
// Rudolph is going to need custom-made medicine. Unfortunately,
// Red-Nosed Reindeer chemistry isn't similar to regular reindeer
// chemistry, either.
//
// The North Pole is equipped with a Red-Nosed Reindeer nuclear
// fusion/fission plant, capable of constructing any Red-Nosed Reindeer
// molecule you need. It works by starting with some input molecule and
// then doing a series of replacements, one per step, until it has the
// right molecule.
//
// However, the machine has to be calibrated before it can be used.
// Calibration involves determining the number of molecules that can be
// generated in one step from a given starting point.
//
// For example, imagine a simpler machine that supports only the
// following replacements:
//
// H => HO
// H => OH
// O => HH
//
// Given the replacements above and starting with HOH, the following
// molecules could be generated:
//
// * HOOH (via H => HO on the first H).
//
// * HOHO (via H => HO on the second H).
//
// * OHOH (via H => OH on the first H).
//
// * HOOH (via H => OH on the second H).
//
// * HHHH (via O => HH).
//
// So, in the example above, there are 4 distinct molecules (not five,
// because HOOH appears twice) after one replacement from HOH. Santa's
// favorite molecule, HOHOHO, can become 7 distinct molecules (over nine
// replacements: six from H, and three from O).
//
// The machine replaces without regard for the surrounding characters.
// For example, given the string H2O, the transition H => OO would result
// in OO2O.
//
// Your puzzle input describes all of the possible replacements and, at
// the bottom, the medicine molecule for which you need to calibrate the
// machine. How many distinct molecules can be created after all the
// different ways you can do one replacement on the medicine molecule?
//
// --- Part Two ---
// ----------------
//
// Now that the machine is calibrated, you're ready to begin molecule
// fabrication.
//
// Molecule fabrication always begins with just a single electron, e, and
// applying replacements one at a time, just like the ones during
// calibration.
//
// For example, suppose you have the following replacements:
//
// e => H
// e => O
// H => HO
// H => OH
// O => HH
//
// If you'd like to make HOH, you start with e, and then make the
// following replacements:
//
// * e => O to get O
//
// * O => HH to get HH
//
// * H => OH (on the second H) to get HOH
//
// So, you could make HOH after 3 steps. Santa's favorite molecule,
// HOHOHO, can be made in 6 steps.
//
// How long will it take to make the medicine? Given the available
// replacements and the medicine molecule in your puzzle input, what is
// the fewest number of steps to go from e to the medicine molecule?

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
    const absl::flat_hash_map<absl::string_view,
                              std::vector<absl::string_view>>& map,
    absl::string_view input) {
  absl::flat_hash_set<std::string> unique;
  for (const auto& pair : map) {
    absl::string_view src = pair.first;
    const std::vector<absl::string_view>& dest_list = pair.second;
    for (int i = 0; i < input.size() - src.size() + 1; ++i) {
      if (input.substr(i, src.size()) == src) {
        for (absl::string_view dest : dest_list) {
          unique.insert(absl::StrCat(input.substr(0, i), dest,
                                     input.substr(i + src.size())));
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

int FindMinPath(const absl::flat_hash_map<absl::string_view,
                                          std::vector<absl::string_view>>& map,
                absl::string_view src, absl::string_view dest) {
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
          for (absl::string_view test_dest : dest_list) {
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
      VLOG(3) << "Trying: " << dest << "@" << i << "; " << it->first << " => "
              << it->second;
      if (dest.substr(i, find.size()) == find) {
        std::string sub_dest = absl::StrCat(dest.substr(0, i), it->second,
                                            dest.substr(i + find.size()));
        absl::optional<int> sub_min =
            FindMinPathReverseImpl(map, map_by_len, memo, src, sub_dest);
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

absl::optional<int> FindMinPathReverse(
    const std::map<absl::string_view, absl::string_view>& map,
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

absl::string_view FirstElement(absl::string_view str) {
  if (str.size() < 2) return str;
  if (str[1] >= 'a' && str[1] <= 'z') {
    return str.substr(0, 2);
  }
  return str.substr(0, 1);
}

absl::optional<int> MatchFromFront(
    const std::map<absl::string_view, absl::string_view>& map,
    absl::string_view src, absl::string_view dest) {
  absl::string_view first_elem = FirstElement(dest);
  VLOG(1) << "Looking how to produce " << first_elem << " for " << dest;
  absl::optional<int> min;
  for (auto it = map.lower_bound(first_elem); it != map.end(); ++it) {
    absl::string_view rule_dest = it->first;
    absl::string_view rule_src = it->second;
    // No more rules that produce the first Element.
    if (rule_dest.substr(0, first_elem.size()) != first_elem) break;
    // This rule can't match since rules only make strings longer.
    if (dest.size() < rule_dest.size()) continue;

    VLOG(2) << "Found: " << it->first << " for " << first_elem;

    if (dest.substr(0, rule_dest.size()) == rule_dest) {
      std::string sub_dest =
          absl::StrCat(rule_src, dest.substr(rule_dest.size()));
      absl::optional<int> sub_min = MatchFromFront(map, src, sub_dest);
      if (sub_min) {
        if (!min || *min > *sub_min) {
          min = *sub_min;
        }
      }
    }
  }

  return absl::nullopt;
}

absl::flat_hash_set<absl::string_view> FindTerminalElements(
    const absl::flat_hash_map<absl::string_view,
                              std::vector<absl::string_view>>& map) {
  absl::flat_hash_set<absl::string_view> ret;
  for (const auto& [src, dest_list] : map) {
    for (absl::string_view dest : dest_list) {
      while (!dest.empty()) {
        absl::string_view first_elem = FirstElement(dest);
        if (!map.contains(first_elem)) {
          ret.insert(first_elem);
        }
        dest = dest.substr(first_elem.size());
      }
    }
  }
  return ret;
}

void PruneRulesFromTerminals(
    absl::string_view str,
    const absl::flat_hash_set<absl::string_view>& terminals,
    absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>>* map,
    std::map<absl::string_view, absl::string_view>* reverse) {
  absl::flat_hash_set<absl::string_view> unused_terminals = terminals;
  for (int off = 0; off < str.size();) {
    absl::string_view first_elem = FirstElement(str.substr(off));
    unused_terminals.erase(first_elem);
    off += first_elem.size();
  }
  if (unused_terminals.empty()) return;
  VLOG(1) << "Pruning terminals: " << absl::StrJoin(unused_terminals, ",");
  for (auto& [src, dest_list] : *map) {
    std::vector<std::string_view> new_dest_list;
    for (absl::string_view dest : dest_list) {
      bool prune_rule = false;
      for (int off = 0; off < dest.size();) {
        absl::string_view first_elem = FirstElement(dest.substr(off));
        if (unused_terminals.contains(first_elem)) {
          prune_rule = true;
          break;
        }
        off += first_elem.size();
      }
      VLOG(2) << "Should prune: " << src << " => " << dest << " = "
              << prune_rule;
      if (prune_rule) {
        VLOG(1) << "Pruning: " << src << " => " << dest;
        reverse->erase(dest);
      } else {
        new_dest_list.push_back(dest);
      }
    }
    dest_list = std::move(new_dest_list);
  }
  return;
}

std::vector<absl::string_view> SplitOnTerminalElements(
    absl::string_view str,
    const absl::flat_hash_set<absl::string_view>& terminals) {
  std::vector<absl::string_view> ret;
  int last_start = 0;
  for (int off = 0; off < str.size();) {
    absl::string_view first_elem = FirstElement(str.substr(off));
    off += first_elem.size();
    if (terminals.contains(first_elem) || off == str.size()) {
      ret.push_back(str.substr(last_start, off - last_start));
      last_start = off;
    }
  }
  return ret;
}

absl::StatusOr<int> MatchFromFrontRnAr(
    absl::string_view str,
    const absl::flat_hash_map<absl::string_view,
                              std::vector<absl::string_view>>& map) {
  absl::string_view first_elem = FirstElement(str);
  absl::string_view second_elem = FirstElement(str.substr(first_elem.size()));
  if (second_elem != "Rn") return Error("Not XRn");
  std::string replaced_string;
  int replace_steps = 0;
  for (int off = first_elem.size() + second_elem.size(); off < str.size();) {
    absl::string_view next_elem = FirstElement(str.substr(off));
    if (next_elem == "Ar") {
      int min = std::numeric_limits<int>::max();
      absl::string_view next_start;
      for (const auto& [start, _] : map) {
        int next_min =
            FindMinPath(map, start, str.substr(0, off + next_elem.size()));
        if (next_min > 0 && next_min < min) {
          min = next_min;
          next_start = start;
        }
      }
      VLOG(2) << "MatchFromFrontRnAr: " << str.substr(0, off + next_elem.size())
              << " to " << next_start << " takes " << min << " steps.";
      replace_steps += min;
      replaced_string =
          absl::StrCat(next_start, str.substr(off + next_elem.size()));
      str = replaced_string;
      off = next_start.size();
      absl::string_view check_rn = FirstElement(str.substr(off));
      if (check_rn != "Rn") return Error("Not XRn");
    } else {
      off += next_elem.size();
    }
  }

  return Error("Nothing to return");
}

}  // namespace

absl::StatusOr<std::string> Day_2015_19::Part1(
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

absl::StatusOr<std::string> Day_2015_19::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() < 3) return Error("Bad input");
  absl::string_view final = input.back();
  if (!input[input.size() - 2].empty()) return Error("Bad input");
  absl::flat_hash_map<absl::string_view, std::vector<absl::string_view>> map;
  std::map<absl::string_view, absl::string_view> reverse;
  for (absl::string_view rule : input.subspan(0, input.size() - 2)) {
    absl::string_view from;
    absl::string_view to;
    if (!RE2::FullMatch(rule, "(e|[A-Z][a-z]?) => ([A-Za-z]+)", &from, &to)) {
      return Error("Bad rule: ", rule);
    }
    if (from.length() > to.length()) return Error("Can't handle reductions");
    map[from].push_back(to);
    if (reverse.find(to) != reverse.end()) return Error("Can't invert");
    reverse[to] = from;
  }

  if (final.size() < 10) {
    // TODO(@monkeynova): This filter might be better served in a reverse
    // model where we recognize the hack will work.
    return IntReturn(FindMinPath(map, "e", final));
  }

  // 'Ar' always terminates a rule.
  // 'Rn' always has a matching 'Ar'.
  // Initial rule sets 2 elemnts.
  // All rules that don't have Rn..Ar add one element.
  // All rules that add Rn..Ar (without Y) add 3.
  // All ruels that add Rn..Y..Ar (with a single Y) add 5.
  // steps is # of elements - 1 (start) - 2 * count('Ar' == 'Rn') - 2 * count
  // 'Y'.
  int steps = -1;
  for (int off = 0; off < final.size();) {
    absl::string_view next_elem = FirstElement(final.substr(off));
    off += next_elem.size();
    ++steps;
    if (next_elem == "Ar") --steps;
    if (next_elem == "Rn") --steps;
    if (next_elem == "Y") steps -= 2;
  }
  return IntReturn(steps);

  absl::flat_hash_set<absl::string_view> terminals = FindTerminalElements(map);
  VLOG(1) << "Terminal Elements = " << absl::StrJoin(terminals, ",");
  std::vector<absl::string_view> terminal_split =
      SplitOnTerminalElements(final, {"Ar"});  // terminals);
  VLOG(1) << "Terminal Split = " << absl::StrJoin(terminal_split, ", ");
  PruneRulesFromTerminals(final, terminals, &map, &reverse);

  return IntReturn(MatchFromFrontRnAr(final, map));

  // return IntReturn(MatchFromFront(reverse, "e", final));

  return IntReturn(FindMinPath(map, "e", final));
}

}  // namespace advent_of_code
