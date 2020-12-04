#ifndef ADVENT_OF_CODE_FILE_BENCHMARK_H
#define ADVENT_OF_CODE_FILE_BENCHMARK_H

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "advent_of_code/file_util.h"
#include "benchmark/benchmark.h"

absl::StatusOr<std::vector<DirtyTestParseResult>> FileBenchmarkTests();

template <typename AdventType>
void BM_Day(benchmark::State& state) {
  AdventType day;
  absl::StatusOr<std::vector<DirtyTestParseResult>> tests =
      FileBenchmarkTests();
  if (!tests.ok()) {
    std::string load_error = std::string(tests.status().message());
    state.SkipWithError(load_error.c_str());
    return;
  }
  if (state.range(0) >= tests->size()) {
    state.SkipWithError("Bad test");
    return;
  }
  const DirtyTestParseResult& test = (*tests)[state.range(0)];
  switch (test.part) {
    case 1: {
      for (auto _ : state) {
        (void)day.Part1(test.lines);
      }
      break;
    }
    case 2: {
      for (auto _ : state) {
        (void)day.Part2(test.lines);
      }
      break;
    }
    default: {
      state.SkipWithError("Bad part");
      return;
    }
  }
}

#endif  // ADVENT_OF_CODE_FILE_BENCHMARK_H
