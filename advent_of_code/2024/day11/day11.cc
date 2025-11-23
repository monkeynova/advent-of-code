// http://adventofcode.com/2024/day/11

#include "advent_of_code/2024/day11/day11.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"

namespace advent_of_code {

namespace {

int64_t HalfPow10(int64_t stone) {
  if (stone < 10) return 0;
  if (stone < 100) return 10;
  if (stone < 1'000) return 0;
  if (stone < 10'000) return 100;
  if (stone < 100'000) return 0;
  if (stone < 1'000'000) return 1'000;
  if (stone < 10'000'000) return 0;
  if (stone < 100'000'000) return 10'000;
  if (stone < 1'000'000'000) return 0;
  if (stone < 10'000'000'000) return 100'000;
  if (stone < 100'000'000'000) return 0;
  if (stone < 1'000'000'000'000) return 1'000'000;
  if (stone < 10'000'000'000'000) return 0;
  if (stone < 100'000'000'000'000) return 10'000'000;
  if (stone < 1'000'000'000'000'000) return 0;
  if (stone < 10'000'000'000'000'000) return 100'000'000;
  if (stone < 100'000'000'000'000'000) return 0;
  if (stone < 1'000'000'000'000'000'000) return 1'000'000'000;
  LOG(FATAL) << "Stone too big";
}

absl::flat_hash_map<int64_t, int64_t> RunStep(
    absl::flat_hash_map<int64_t, int64_t> stones) {
  absl::flat_hash_map<int64_t, int64_t> new_stones;
  for (const auto& [stone, count] : stones) {
    if (stone == 0) {
      new_stones[1] += count;
    } else if (int64_t pow10_half = HalfPow10(stone)) {
      new_stones[stone / pow10_half] += count;
      new_stones[stone % pow10_half] += count;
    } else {
      new_stones[stone * 2024] += count;
    }
  }
  return new_stones;
}

int64_t CountStonesAfter(const std::vector<int64_t>& stones_vec,
                         int64_t steps) {
  absl::flat_hash_map<int64_t, int64_t> stones;
  for (int64_t s : stones_vec) ++stones[s];

  for (int i = 0; i < steps; ++i) {
    stones = RunStep(std::move(stones));
  }
  return absl::c_accumulate(
      stones, int64_t{0},
      [](int64_t a, const std::pair<int64_t, int64_t>& s_and_c) {
        return a + s_and_c.second;
      });
}

}  // namespace

absl::StatusOr<std::string> Day_2024_11::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("1 line?");
  ASSIGN_OR_RETURN(std::vector<int64_t> in_stones,
                   ParseAsInts(absl::StrSplit(input[0], " ")));

  return AdventReturn(CountStonesAfter(in_stones, 25));
}

absl::StatusOr<std::string> Day_2024_11::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("1 line?");
  ASSIGN_OR_RETURN(std::vector<int64_t> in_stones,
                   ParseAsInts(absl::StrSplit(input[0], " ")));

  return AdventReturn(CountStonesAfter(in_stones, 75));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/11,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_11()); });

}  // namespace advent_of_code
