#include "advent_of_code/2015/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/opt_cmp.h"
#include "glog/logging.h"
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

absl::StatusOr<std::vector<std::string>> Day_2015_14::Part1(
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

absl::StatusOr<std::vector<std::string>> Day_2015_14::Part2(
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
