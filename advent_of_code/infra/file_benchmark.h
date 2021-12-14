#ifndef ADVENT_OF_CODE_INFRA_FILE_BENCHMARK_H
#define ADVENT_OF_CODE_INFRA_FILE_BENCHMARK_H

#include "absl/status/statusor.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/file_util.h"
#include "benchmark/benchmark.h"

namespace advent_of_code {

absl::StatusOr<int> FileBenchmarkTestCount();
absl::StatusOr<int> FileBenchmarkTestCount(AdventDay* day);

inline void BM_Day_SetError(benchmark::State& state,
                            absl::string_view message) {
  std::string message_str = std::string(message);
  state.SkipWithError(message_str.c_str());
}

void BM_Day(benchmark::State& state, AdventDay* advent_day);

template <typename AdventType>
void BM_Day(benchmark::State& state) {
  AdventType day;
  BM_Day(state, &day);
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INFRA_FILE_BENCHMARK_H
