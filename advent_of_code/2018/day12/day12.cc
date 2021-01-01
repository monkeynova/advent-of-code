#include "advent_of_code/2018/day12/day12.h"

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

absl::StatusOr<std::vector<std::string>> Day12_2018::Part1(
    absl::Span<absl::string_view> input) const {
  absl::string_view initial_state;
  absl::flat_hash_map<absl::string_view, absl::string_view> rules;
  for (absl::string_view row : input) {
    absl::string_view from;
    absl::string_view to;
    if (row.empty()) continue;
    if (RE2::FullMatch(row, "initial state: ([\\.\\#]*)", &initial_state)) {
      // OK.
    } else if (RE2::FullMatch(row, "([\\.\\#]{5}) => ([\\.\\#])", &from, &to)) {
      if (rules.contains(from)) return Error("Dupe rule: ", row);
      rules[from] = to;
    } else {
      return Error("Bad row: ", row);
    }
  }
  if (initial_state.empty()) return Error("No initial state");
  std::string cur = std::string(initial_state);
  absl::string_view empty = ".....";
  int offset = 0;
  for (int i = 0; i < 20; ++i) {
    VLOG(1) << "@[" << offset << "]: " << cur;
    absl::string_view cur_view = cur;
    std::string next;
    int cur_size = cur.size();  // Not an unsigned int!
    for (int j = -4; j < cur_size; ++j) {
      int start_need = -std::min(j, 0);
      int end_need = std::max(j + 5 - cur_size, 0);
      std::string test = absl::StrCat(
        empty.substr(0, start_need),
        cur_view.substr(std::max(j, 0), 5 - start_need - end_need),
        empty.substr(0, end_need));
      if (test.size() != 5) return Error("Bad read: ", j);
      if (!rules.contains(test)) {
        //return Error("No rule for: ", test);
        next.append(std::string(empty.substr(0, 1)));
      } else {
        next.append(std::string(rules[test]));
      }
      VLOG(2) << j << ": " << test << " => " << next.back();
    }
    offset -= 2;
    absl::string_view next_view = next;
    while (next_view[0] == '.') {
      ++offset;
      next_view = next_view.substr(1);
    }
    while (next_view.back() == '.') next_view = next_view.substr(0, next_view.size() - 1);
    cur = std::string(next_view);
  }
  VLOG(1) << "@[" << offset << "]: " << cur;
  int count = 0;
  for (int i = 0; i < cur.size(); ++i) {
    if (cur[i] == '#') {
      VLOG(2) << i + offset;
      count += i + offset;
    }
  }
  return IntReturn(count);
}

absl::StatusOr<std::vector<std::string>> Day12_2018::Part2(
    absl::Span<absl::string_view> input) const {
  absl::string_view initial_state;
  absl::flat_hash_map<absl::string_view, absl::string_view> rules;
  for (absl::string_view row : input) {
    absl::string_view from;
    absl::string_view to;
    if (row.empty()) continue;
    if (RE2::FullMatch(row, "initial state: ([\\.\\#]*)", &initial_state)) {
      // OK.
    } else if (RE2::FullMatch(row, "([\\.\\#]{5}) => ([\\.\\#])", &from, &to)) {
      if (rules.contains(from)) return Error("Dupe rule: ", row);
      rules[from] = to;
    } else {
      return Error("Bad row: ", row);
    }
  }
  if (initial_state.empty()) return Error("No initial state");
  std::string cur = std::string(initial_state);
  absl::string_view empty = ".....";
  int64_t offset = 0;
  constexpr int64_t kGenerations = 50'000'000'000;
  for (int64_t i = 0; i < kGenerations; ++i) {
    VLOG(1) << "@[" << offset << "]: " << cur;
    absl::string_view cur_view = cur;
    std::string next;
    int cur_size = cur.size();  // Not an unsigned int!
    for (int j = -4; j < cur_size; ++j) {
      int start_need = -std::min(j, 0);
      int end_need = std::max(j + 5 - cur_size, 0);
      std::string test = absl::StrCat(
        empty.substr(0, start_need),
        cur_view.substr(std::max(j, 0), 5 - start_need - end_need),
        empty.substr(0, end_need));
      if (test.size() != 5) return Error("Bad read: ", j);
      if (!rules.contains(test)) {
        //return Error("No rule for: ", test);
        next.append(std::string(empty.substr(0, 1)));
      } else {
        next.append(std::string(rules[test]));
      }
      VLOG(2) << j << ": " << test << " => " << next.back();
    }
    int64_t last_offset = offset;
    offset -= 2;
    absl::string_view next_view = next;
    while (next_view[0] == '.') {
      ++offset;
      next_view = next_view.substr(1);
    }
    while (next_view.back() == '.') next_view = next_view.substr(0, next_view.size() - 1);
    if (cur == next_view) {
      LOG(INFO) << "Found translation @" << i;
      int64_t offset_delta = offset - last_offset;
      offset += offset_delta * (kGenerations - i - 1);
      LOG(INFO) << "Upping offset by " << offset_delta * (kGenerations - i - 1);
      break;
    }
    cur = std::string(next_view);
  }
  VLOG(1) << "@[" << offset << "]: " << cur;
  int64_t count = 0;
  for (int64_t i = 0; i < cur.size(); ++i) {
    if (cur[i] == '#') {
      VLOG(2) << i + offset;
      count += i + offset;
    }
  }
  return IntReturn(count);
}

}  // namespace advent_of_code
