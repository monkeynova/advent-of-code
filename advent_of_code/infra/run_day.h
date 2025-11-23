#ifndef ADVENT_OF_CODE_INFRA_RUN_DAY_H
#define ADVENT_OF_CODE_INFRA_RUN_DAY_H

#include <cstdint>
#include <string>

#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "advent_of_code/advent_day.h"

namespace advent_of_code {

struct DayRun {
  absl::Duration time;
  std::string title;
  std::string part1;
  bool part1_solved;
  std::string part2;
  bool part2_solved;
  int64_t bytes_processed;
};

absl::StatusOr<DayRun> RunDay(advent_of_code::AdventDay* day);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INFRA_RUN_DAY_H
