// https://adventofcode.com/2015/day/14
//
// --- Day 14: Reindeer Olympics ---
// ---------------------------------
//
// This year is the Reindeer Olympics! Reindeer can fly at high speeds,
// but must rest occasionally to recover their energy. Santa would like
// to know which of his reindeer is fastest, and so he has them race.
//
// Reindeer can only either be flying (always at their top speed) or
// resting (not moving at all), and always spend whole seconds in either
// state.
//
// For example, suppose you have the following Reindeer:
//
// * Comet can fly 14 km/s for 10 seconds, but then must rest for 127
// seconds.
//
// * Dancer can fly 16 km/s for 11 seconds, but then must rest for 162
// seconds.
//
// After one second, Comet has gone 14 km, while Dancer has gone 16 km.
// After ten seconds, Comet has gone 140 km, while Dancer has gone 160
// km. On the eleventh second, Comet begins resting (staying at 140 km),
// and Dancer continues on for a total distance of 176 km. On the 12th
// second, both reindeer are resting. They continue to rest until the
// 138th second, when Comet flies for another ten seconds. On the 174th
// second, Dancer flies for another 11 seconds.
//
// In this example, after the 1000th second, both reindeer are resting,
// and Comet is in the lead at 1120 km (poor Dancer has only gotten 1056
// km by that point). So, in this situation, Comet would win (if the race
// ended at 1000 seconds).
//
// Given the descriptions of each reindeer (in your puzzle input), after
// exactly 2503 seconds, what distance has the winning reindeer traveled?
//
// --- Part Two ---
// ----------------
//
// Seeing how reindeer move in bursts, Santa decides he's not pleased
// with the old scoring system.
//
// Instead, at the end of each second, he awards one point to the
// reindeer currently in the lead. (If there are multiple reindeer tied
// for the lead, they each get one point.) He keeps the traditional 2503
// second time limit, of course, as doing otherwise would be entirely
// ridiculous.
//
// Given the example reindeer from above, after the first second, Dancer
// is in the lead and gets one point. He stays in the lead until several
// seconds into Comet's second burst: after the 140th second, Comet pulls
// into the lead and gets his first point. Of course, since Dancer had
// been in the lead for the 139 seconds before that, he has accumulated
// 139 points by the 140th second.
//
// After the 1000th second, Dancer has accumulated 689 points, while poor
// Comet, our old champion, only has 312. So, with the new scoring
// system, Dancer would win (if the race ended at 1000 seconds).
//
// Again given the descriptions of each reindeer (in your puzzle input),
// after exactly 2503 seconds, how many points does the winning reindeer
// have?

#include "advent_of_code/2015/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/opt_cmp.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Flight {
  absl::string_view name;
  int speed;
  int run_time;
  int rest_time;
};

struct RaceResult {
  std::vector<absl::string_view> winners;
  int dist = std::numeric_limits<int>::min();
};

RaceResult RunRace(const std::vector<Flight>& flights, int time) {
  RaceResult res;
  for (Flight f : flights) {
    int cycles = time / (f.run_time + f.rest_time);
    int rem_time = time % (f.run_time + f.rest_time);
    if (rem_time > f.run_time) {
      ++cycles;
      rem_time = 0;
    }
    int dist = cycles * f.speed * f.run_time + rem_time * f.speed;
    if (dist > res.dist) {
      res.dist = dist;
      res.winners = {f.name};
    } else if (dist == res.dist) {
      res.winners.push_back(f.name);
    }
  }
  return res;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_14::Part1(
    absl::Span<absl::string_view> input) const {
  static LazyRE2 parse = {
      "(.*) can fly (\\d+) km/s for (\\d+) seconds, but then "
      "must rest for (\\d+) seconds\\."};
  // Prancer can fly 25 km/s for 6 seconds, but then must rest for 143 seconds.
  std::vector<Flight> flights;
  for (absl::string_view str : input) {
    Flight next;
    if (!RE2::FullMatch(str, *parse, &next.name, &next.speed, &next.run_time,
                        &next.rest_time)) {
      return Error("Bad input: ", str);
    }
    flights.push_back(next);
  }
  return IntReturn(RunRace(flights, 2503).dist);
}

absl::StatusOr<std::string> Day_2015_14::Part2(
    absl::Span<absl::string_view> input) const {
  static LazyRE2 parse = {
      "(.*) can fly (\\d+) km/s for (\\d+) seconds, but then "
      "must rest for (\\d+) seconds\\."};
  std::vector<Flight> flights;
  for (absl::string_view str : input) {
    Flight next;
    if (!RE2::FullMatch(str, *parse, &next.name, &next.speed, &next.run_time,
                        &next.rest_time)) {
      return Error("Bad input: ", str);
    }
    flights.push_back(next);
  }
  absl::flat_hash_map<absl::string_view, int> points;
  for (int i = 1; i <= 2503; ++i) {
    RaceResult rr = RunRace(flights, i);
    for (absl::string_view name : rr.winners) {
      ++points[name];
    }
  }
  absl::optional<int> max;
  for (const auto& pair : points) {
    max = opt_max(max, pair.second);
  }
  return IntReturn(max);
}

}  // namespace advent_of_code
