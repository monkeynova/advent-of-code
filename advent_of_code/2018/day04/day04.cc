#include "advent_of_code/2018/day04/day04.h"

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

absl::StatusOr<std::string> Day_2018_04::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::string_view> sorted(input.begin(), input.end());
  std::sort(sorted.begin(), sorted.end());
  int cur_guard = -1;
  struct SleepWindow {
    int start;
    int end;
  };
  absl::flat_hash_map<int, std::vector<SleepWindow>> schedule;
  SleepWindow cur_sleep_window = {-1, -1};
  for (absl::string_view row : sorted) {
    int min;
    VLOG(1) << row;
    if (RE2::PartialMatch(row, "Guard \\#(\\d+) begins shift", &cur_guard)) {
      // Handled.
    } else if (RE2::PartialMatch(row, "00:(\\d+)\\] falls asleep", &min)) {
      if (cur_guard == -1) return Error("No guard: ", row);
      if (cur_sleep_window.start != -1) return Error("Sleep start: ", row);
      cur_sleep_window.start = min;
    } else if (RE2::PartialMatch(row, "00:(\\d+)\\] wakes up", &min)) {
      if (cur_guard == -1) return Error("No guard: ", row);
      if (cur_sleep_window.start == -1)
        return Error("Sleep end (no start): ", row);
      if (cur_sleep_window.end != -1) return Error("Sleep end: ", row);
      cur_sleep_window.end = min;
      schedule[cur_guard].push_back(cur_sleep_window);
      cur_sleep_window = {-1, -1};
    } else {
      return Error("Bad line: ", row);
    }
  }
  int max_sleep = -1;
  int max_guard_id = -1;
  for (const auto& [guard_id, sleeps] : schedule) {
    int total_sleep = 0;
    for (SleepWindow w : sleeps) {
      VLOG(1) << guard_id << ": " << w.start << ", " << w.end;
      total_sleep += w.end - w.start;
    }
    if (total_sleep > max_sleep) {
      max_sleep = total_sleep;
      max_guard_id = guard_id;
    }
  }
  VLOG(1) << "Using guard: " << max_guard_id;
  absl::flat_hash_map<int, int> sleep_min;
  for (SleepWindow w : schedule[max_guard_id]) {
    for (int i = w.start; i < w.end; ++i) {
      ++sleep_min[i];
    }
  }
  int max_sleep_minute = -1;
  int max_sleep_count = -1;
  int dupes = 0;
  for (const auto& [minute, count] : sleep_min) {
    if (max_sleep_count < count) {
      dupes = 1;
      max_sleep_count = count;
      max_sleep_minute = minute;
    } else if (max_sleep_count == count) {
      ++dupes;
    }
  }
  if (dupes != 1) return Error("Bad best minute calculation: ", dupes);
  return IntReturn(max_guard_id * max_sleep_minute);
}

absl::StatusOr<std::string> Day_2018_04::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<absl::string_view> sorted(input.begin(), input.end());
  std::sort(sorted.begin(), sorted.end());
  int cur_guard = -1;
  struct SleepWindow {
    int start;
    int end;
  };
  absl::flat_hash_map<int, std::vector<SleepWindow>> schedule;
  SleepWindow cur_sleep_window = {-1, -1};
  for (absl::string_view row : sorted) {
    int min;
    VLOG(1) << row;
    if (RE2::PartialMatch(row, "Guard \\#(\\d+) begins shift", &cur_guard)) {
      // Handled.
    } else if (RE2::PartialMatch(row, "00:(\\d+)\\] falls asleep", &min)) {
      if (cur_guard == -1) return Error("No guard: ", row);
      if (cur_sleep_window.start != -1) return Error("Sleep start: ", row);
      cur_sleep_window.start = min;
    } else if (RE2::PartialMatch(row, "00:(\\d+)\\] wakes up", &min)) {
      if (cur_guard == -1) return Error("No guard: ", row);
      if (cur_sleep_window.start == -1)
        return Error("Sleep end (no start): ", row);
      if (cur_sleep_window.end != -1) return Error("Sleep end: ", row);
      cur_sleep_window.end = min;
      schedule[cur_guard].push_back(cur_sleep_window);
      cur_sleep_window = {-1, -1};
    } else {
      return Error("Bad line: ", row);
    }
  }
  int max_sleep_count = -1;
  int max_sleep_minute = -1;
  int max_guard_id = -1;
  int guard_dupes = 0;
  absl::flat_hash_set<int> bad_guards;
  for (const auto& [guard_id, sleeps] : schedule) {
    absl::flat_hash_map<int, int> sleep_min;
    for (SleepWindow w : sleeps) {
      for (int i = w.start; i < w.end; ++i) {
        ++sleep_min[i];
      }
    }
    int this_max_sleep_minute = -1;
    int this_max_sleep_count = -1;
    int dupes = 0;
    for (const auto& [minute, count] : sleep_min) {
      if (this_max_sleep_count < count) {
        dupes = 1;
        this_max_sleep_count = count;
        this_max_sleep_minute = minute;
      } else if (this_max_sleep_count == count) {
        ++dupes;
      }
    }
    if (dupes != 1) {
      bad_guards.insert(guard_id);
    }
    if (this_max_sleep_count > max_sleep_count) {
      guard_dupes = 1;
      max_sleep_count = this_max_sleep_count;
      max_sleep_minute = this_max_sleep_minute;
      max_guard_id = guard_id;
    } else if (this_max_sleep_count == max_sleep_count) {
      ++guard_dupes;
    }
  }
  LOG(INFO) << "Guard: " << max_guard_id;
  LOG(INFO) << "Minute: " << max_sleep_minute;
  if (bad_guards.contains(max_guard_id)) {
    return Error("Bad best minute calculation for selected guard: ",
                 max_guard_id);
  }
  if (guard_dupes != 1) return Error("Bad guard calculation: ", guard_dupes);
  return IntReturn(max_guard_id * max_sleep_minute);
}

}  // namespace advent_of_code
