// https://adventofcode.com/2018/day/4
//
// --- Day 4: Repose Record ---
// ----------------------------
//
// You've sneaked into another supply closet - this time, it's across
// from the prototype suit manufacturing lab. You need to sneak inside
// and fix the issues with the suit, but there's a guard stationed
// outside the lab, so this is as close as you can safely get.
//
// As you search the closet for anything that might help, you discover
// that you're not the first person to want to sneak in. Covering the
// walls, someone has spent an hour starting every midnight for the past
// few months secretly observing this guard post! They've been writing
// down the ID of the one guard on duty that night - the Elves seem to
// have decided that one guard was enough for the overnight shift - as
// well as when they fall asleep or wake up while at their post (your
// puzzle input).
//
// For example, consider the following records, which have already been
// organized into chronological order:
//
// [1518-11-01 00:00] Guard #10 begins shift
// [1518-11-01 00:05] falls asleep
// [1518-11-01 00:25] wakes up
// [1518-11-01 00:30] falls asleep
// [1518-11-01 00:55] wakes up
// [1518-11-01 23:58] Guard #99 begins shift
// [1518-11-02 00:40] falls asleep
// [1518-11-02 00:50] wakes up
// [1518-11-03 00:05] Guard #10 begins shift
// [1518-11-03 00:24] falls asleep
// [1518-11-03 00:29] wakes up
// [1518-11-04 00:02] Guard #99 begins shift
// [1518-11-04 00:36] falls asleep
// [1518-11-04 00:46] wakes up
// [1518-11-05 00:03] Guard #99 begins shift
// [1518-11-05 00:45] falls asleep
// [1518-11-05 00:55] wakes up
//
// Timestamps are written using year-month-day hour:minute format. The
// guard falling asleep or waking up is always the one whose shift most
// recently started. Because all asleep/awake times are during the
// midnight hour (00:00 - 00:59), only the minute portion (00 - 59) is
// relevant for those events.
//
// Visually, these records show that the guards are asleep at these
// times:
//
// Date   ID   Minute
// 000000000011111111112222222222333333333344444444445555555555
// 012345678901234567890123456789012345678901234567890123456789
// 11-01  #10  .....####################.....#########################.....
// 11-02  #99  ........................................##########..........
// 11-03  #10  ........................#####...............................
// 11-04  #99  ....................................##########..............
// 11-05  #99  .............................................##########.....
//
// The columns are Date, which shows the month-day portion of the
// relevant day; ID, which shows the guard on duty that day; and Minute,
// which shows the minutes during which the guard was asleep within the
// midnight hour. (The Minute column's header shows the minute's ten's
// digit in the first row and the one's digit in the second row.) Awake
// is shown as ., and asleep is shown as #.
//
// Note that guards count as asleep on the minute they fall asleep, and
// they count as awake on the minute they wake up. For example, because
// Guard #10 wakes up at 00:25 on 1518-11-01, minute 25 is marked as
// awake.
//
// If you can figure out the guard most likely to be asleep at a specific
// time, you might be able to trick that guard into working tonight so
// you can have the best chance of sneaking in. You have two strategies
// for choosing the best guard/minute combination.
//
// Strategy 1: Find the guard that has the most minutes asleep. What
// minute does that guard spend asleep the most?
//
// In the example above, Guard #10 spent the most minutes asleep, a total
// of 50 minutes (20+25+5), while Guard #99 only slept for a total of 30
// minutes (10+10+10). Guard #10 was asleep most during minute 24 (on two
// days, whereas any other minute the guard was asleep was only seen on
// one day).
//
// While this example listed the entries in chronological order, your
// entries are in the order you found them. You'll need to organize them
// before they can be analyzed.
//
// What is the ID of the guard you chose multiplied by the minute you
// chose? (In the above example, the answer would be 10 * 24 = 240.)
//
// --- Part Two ---
// ----------------
//
// Strategy 2: Of all guards, which guard is most frequently asleep on
// the same minute?
//
// In the example above, Guard #99 spent minute 45 asleep more than any
// other guard or minute - three times in total. (In all other cases, any
// guard spent any minute asleep at most twice.)
//
// What is the ID of the guard you chose multiplied by the minute you
// chose? (In the above example, the answer would be 99 * 45 = 4455.)

#include "advent_of_code/2018/day04/day04.h"

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
