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

struct Flight {
  absl::string_view name;
  int speed;
  int run_time;
  int rest_time;
};

absl::StatusOr<std::vector<std::string>> Day14_2015::Part1(
    absl::Span<absl::string_view> input) const {
  //Prancer can fly 25 km/s for 6 seconds, but then must rest for 143 seconds.
  std::vector<Flight> flights;
  for (absl::string_view str : input) {
    Flight next;
    if (!RE2::FullMatch(str, "(.*) can fly (\\d+) km/s for (\\d+) seconds, but then must rest for (\\d+) seconds\\.",
                        &next.name, &next.speed, &next.run_time, &next.rest_time)) {
      return Error("Bad input: ", str);
    }
    flights.push_back(next);
  }
  int time = 2503;
  absl::optional<int> max;
  for (Flight f : flights) {
    int cycles = time / (f.run_time + f.rest_time);
    int rem_time = time % (f.run_time + f.rest_time);
    if (rem_time > f.run_time) {
      ++cycles;
      rem_time = 0;
    }
    int dist = cycles * f.speed * f.run_time + rem_time * f.speed;
    max = opt_max(max, dist);
  }
  return IntReturn(max);
}

absl::StatusOr<std::vector<std::string>> Day14_2015::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}
