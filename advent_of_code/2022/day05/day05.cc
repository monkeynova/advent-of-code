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

absl::StatusOr<std::vector<std::vector<char>>> ParseStacks(
    absl::Span<absl::string_view> lines) {
  std::vector<std::vector<char>> stacks;
  for (absl::string_view line : lines) {
    if (line.size() % 4 != 3) return Error("Bad line");
    for (int i = 0; 4 * i < line.size(); ++i) {
      while (stacks.size() <= i) stacks.push_back({});
      absl::string_view crate = line.substr(4 * i, 3);
      if (crate == "   ") {
        if (!stacks[i].empty()) return Error("Hole in stack");
      } else if (crate[0] == '[' && crate[2] == ']') {
        stacks[i].push_back(crate[1]);
      } else if (crate[0] == ' ' && crate[2] == ' ') {
        if (crate[1] != i + '1') return Error("Bad crate (index): ", crate);
      } else {
        return Error("Bad crate: ", crate);
      }
    }
  }

  for (std::vector<char>& s : stacks) {
    absl::c_reverse(s);
  }
  return stacks;
}

}  // namespace

absl::StatusOr<std::string> Day_2022_05::Part1(
    absl::Span<absl::string_view> input) const {
  std::optional<int> split_at;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      split_at = i;
      break;
    }
  }
  if (!split_at) return Error("No empty line in input");

  absl::StatusOr<std::vector<std::vector<char>>> stacks =
      ParseStacks(input.subspan(0, *split_at));
  if (!stacks.ok()) return stacks.status();

  for (absl::string_view line : input.subspan(*split_at + 1)) {
    int count, from_idx, to_idx;
    if (!RE2::FullMatch(line, "move (\\d+) from (\\d+) to (\\d+)",
                        &count, &from_idx, &to_idx)) {
      return Error("Bad move (RE)");
    }
    if (stacks->size() < from_idx) return Error("Bad move (from) ", from_idx);
    std::vector<char>& from = (*stacks)[from_idx - 1];
    if (stacks->size() < to_idx) return Error("Bad move (to) ", to_idx);
    std::vector<char>& to = (*stacks)[to_idx - 1];
    if (from.size() < count) return Error("Bad move: empty");
    for (int i = 0; i < count; ++i) {
      to.push_back(from[from.size() - i - 1]);
    }
    from.resize(from.size() - count);
  }
  std::string ret;
  for (std::vector<char>& s : *stacks) {
    ret.append(1, s.back());
  }
  return ret;
}

absl::StatusOr<std::string> Day_2022_05::Part2(
    absl::Span<absl::string_view> input) const {
  std::optional<int> split_at;
  for (int i = 0; i < input.size(); ++i) {
    if (input[i].empty()) {
      split_at = i;
      break;
    }
  }
  if (!split_at) return Error("No empty line in input");

  absl::StatusOr<std::vector<std::vector<char>>> stacks =
      ParseStacks(input.subspan(0, *split_at));
  if (!stacks.ok()) return stacks.status();

  for (absl::string_view line : input.subspan(*split_at + 1)) {
    int count, from_idx, to_idx;
    if (!RE2::FullMatch(line, "move (\\d+) from (\\d+) to (\\d+)",
                        &count, &from_idx, &to_idx)) {
      return Error("Bad move (RE)");
    }
    if (stacks->size() < from_idx) return Error("Bad move (from) ", from_idx);
    std::vector<char>& from = (*stacks)[from_idx - 1];
    if (stacks->size() < to_idx) return Error("Bad move (to) ", to_idx);
    std::vector<char>& to = (*stacks)[to_idx - 1];
    if (from.size() < count) return Error("Bad move: empty");
    for (int i = 0; i < count; ++i) {
      to.push_back(from[from.size() - count + i]);
    }
    from.resize(from.size() - count);
  }
  std::string ret;
  for (std::vector<char>& s : *stacks) {
    ret.append(1, s.back());
  }
  return ret;
}

}  // namespace advent_of_code
