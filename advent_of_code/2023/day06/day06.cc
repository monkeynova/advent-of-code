// http://adventofcode.com/2023/day/6

#include "advent_of_code/2023/day06/day06.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

int64_t ValidRangeSize(int64_t time, int64_t distance) {
  // (time - t) * t > distance
  // t * time - t^2 > distance
  // 0 > t^2 - t * time + distance
  int64_t d = time * time - 4 * distance;
  if (d < 0) return 0;
  // floor + 1 and ceil - 1 handle the strict inequality with 0.
  // That is we would use ceil/floor respectivey if we wanted >= 0.
  int64_t min = floor(0.5 * (time - sqrt(d))) + 1;
  int64_t max = ceil(0.5 * (time + sqrt(d))) - 1;
  VLOG(1) << min << "-" << max;
  return max - min + 1;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_06::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  Tokenizer time_tok(input[0]);
  RETURN_IF_ERROR(time_tok.NextIs("Time"));
  RETURN_IF_ERROR(time_tok.NextIs(":"));
  Tokenizer dist_tok(input[1]);
  RETURN_IF_ERROR(dist_tok.NextIs("Distance"));
  RETURN_IF_ERROR(dist_tok.NextIs(":"));

  int64_t product = 1;
  while (!time_tok.Done()) {
    ASSIGN_OR_RETURN(int time, time_tok.NextInt());
    if (dist_tok.Done()) return Error("Bad lengths");
    ASSIGN_OR_RETURN(int distance, dist_tok.NextInt());
    product *= ValidRangeSize(time, distance);
  }
  if (!dist_tok.Done()) return Error("Bad lengths");
  return AdventReturn(product);
}

absl::StatusOr<std::string> Day_2023_06::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 2) return Error("Bad input");
  int64_t time = 0;
  std::string_view time_str = absl::StripPrefix(input[0], "Time:");
  for (char c : time_str) {
    if (isdigit(c)) {
      time = time * 10 + c - '0';
    } else if (!isspace(c)) {
      return Error("Bad time");
    }
  }
  int64_t distance = 0;
  std::string_view distance_str = absl::StripPrefix(input[1], "Distance:");
  for (char c : distance_str) {
    if (isdigit(c)) {
      distance = distance * 10 + c - '0';
    } else if (!isspace(c)) {
      return Error("Bad distance");
    }
  }
  return AdventReturn(ValidRangeSize(time, distance));
}

}  // namespace advent_of_code
