// http://adventofcode.com/2022/day/05

#include "advent_of_code/2022/day05/day05.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_05::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<std::vector<char>> stacks;
  bool done_parse = false;
  for (absl::string_view line : input) {
    if (line == "HACK:") continue;
    if (line.empty()) {
      for (std::vector<char>& s : stacks) {
        absl::c_reverse(s);
      }
      done_parse = true;
      continue;
    }
    if (done_parse) { 
      int count, from, to;
      if (!RE2::FullMatch(line, "move (\\d+) from (\\d+) to (\\d+)", &count, &from, &to)) {
        return Error("Bad move (RE)");
      }
      if (stacks.size() < from) return Error("Bad move (from) ", from);
      if (stacks.size() < to) return Error("Bad move (to) ", to);
      for (int i = 0; i < count; ++i) {
        if (stacks[from-1].empty()) return Error("Bad stack");
        char c = stacks[from - 1].back();
        stacks[from-1].pop_back();
        stacks[to-1].push_back(c);
      }
    } else {
      int i = 0;
      for (int idx = 0; idx < line.size(); idx += 4) {
        absl::string_view crate = line.substr(idx, 3);
        if (crate == "   ") {
          // nop
        } else if (crate[0] == '[' && crate[2] == ']') {
          while (stacks.size() <= i) stacks.push_back({});
          stacks[i].push_back(crate[1]);
        } else if (crate[0] == ' ' && crate[2] == ' ') {
          if (crate[1] != i + '1') return Error("Bad crate (index): ", crate);
        } else {
          return Error("Bad crate: ", crate);
        }
        ++i;
      }
    }
  }
  std::string ret;
  for (std::vector<char>& s : stacks) {
    ret.append(1, s.back());
  }
  return ret;
}

absl::StatusOr<std::string> Day_2022_05::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<std::vector<char>> stacks;
  bool done_parse = false;
  for (absl::string_view line : input) {
    if (line == "HACK:") continue;
    if (line.empty()) {
      for (std::vector<char>& s : stacks) {
        absl::c_reverse(s);
      }
      done_parse = true;
      continue;
    }
    if (done_parse) { 
      int count, from, to;
      if (!RE2::FullMatch(line, "move (\\d+) from (\\d+) to (\\d+)", &count, &from, &to)) {
        return Error("Bad move (RE)");
      }
      if (stacks.size() < from) return Error("Bad move (from) ", from);
      if (stacks.size() < to) return Error("Bad move (to) ", to);
      std::vector<char> to_add;
      for (int i = 0; i < count; ++i) {
        if (stacks[from-1].empty()) return Error("Bad stack");
        char c = stacks[from - 1].back();
        stacks[from-1].pop_back();
        to_add.push_back(c);
      }
      while (!to_add.empty()) {
        char c = to_add.back();
        to_add.pop_back();
        stacks[to-1].push_back(c);
      }
    } else {
      int i = 0;
      for (int idx = 0; idx < line.size(); idx += 4) {
        absl::string_view crate = line.substr(idx, 3);
        if (crate == "   ") {
          // nop
        } else if (crate[0] == '[' && crate[2] == ']') {
          while (stacks.size() <= i) stacks.push_back({});
          stacks[i].push_back(crate[1]);
        } else if (crate[0] == ' ' && crate[2] == ' ') {
          if (crate[1] != i + '1') return Error("Bad crate (index): ", crate);
        } else {
          return Error("Bad crate: ", crate);
        }
        ++i;
      }
    }
  }
  std::string ret;
  for (std::vector<char>& s : stacks) {
    ret.append(1, s.back());
  }
  return ret;
}

}  // namespace advent_of_code
