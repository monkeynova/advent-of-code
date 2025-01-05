#include "benchmark/benchmark.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/run_day.h"
#include "main_lib.h"
#include "vlog.h"

namespace {

void BM_Day(benchmark::State& state) {
  std::unique_ptr<advent_of_code::AdventDay> day =
      advent_of_code::CreateAdventDay(state.range(0), state.range(1));
  if (day == nullptr) {
    state.SkipWithError("No advent day");
    return;
  }
  int64_t bytes_processed = 0;
  for (auto _ : state) {
    absl::StatusOr<advent_of_code::DayRun> run = RunDay(day.get());
    if (!run.ok()) {
      state.SkipWithError(std::string(run.status().message()));
      return;
    }
    bytes_processed += run->bytes_processed;
  }
  state.SetBytesProcessed(bytes_processed);
}

static const int cur_year = []() {
  return absl::ToCivilYear(
    absl::Now() - absl::Seconds(30 * 86400), absl::UTCTimeZone()).year();
}();

BENCHMARK(BM_Day)->ArgsProduct(
    {benchmark::CreateDenseRange(2015, cur_year, /*step=*/1),
     benchmark::CreateDenseRange(1, 25, /*step=*/1)});

void BM_WholeYear(benchmark::State& state) {
  std::vector<std::unique_ptr<advent_of_code::AdventDay>> days;
  for (int day = 1; day < 25; ++day) {
    days.push_back(advent_of_code::CreateAdventDay(state.range(0), day));
    if (days.back() == nullptr) days.pop_back();
  }
  if (days.empty()) {
    state.SkipWithError("No advent day");
    return;
  }
  int64_t bytes_processed = 0;
  for (auto _ : state) {
    for (const auto& day : days) {
      absl::StatusOr<advent_of_code::DayRun> run = RunDay(day.get());
      if (!run.ok()) {
        state.SkipWithError(std::string(run.status().message()));
        return;
      }
      bytes_processed += run->bytes_processed;
    }
  }
  state.SetBytesProcessed(bytes_processed);
}

BENCHMARK(BM_WholeYear)->DenseRange(2015, cur_year);

}  // namespace