#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/file_util.h"
#include "advent_of_code/vlog.h"
#include "benchmark/benchmark.h"
#include "main_lib.h"

namespace {

struct Input {
  std::string file;
  std::vector<std::string_view> lines;
};

absl::StatusOr<Input> ReadInput(advent_of_code::AdventDay* day) {
  if (day == nullptr) return absl::InvalidArgumentError("null day");
  Input ret;
  std::string filename(day->test_file());
  filename.erase(filename.rfind('/'));
  filename.append("/input.txt");
  ASSIGN_OR_RETURN(ret.file, advent_of_code::GetContents(filename));
  ret.lines = absl::StrSplit(ret.file, '\n');
  while (!ret.lines.empty() && ret.lines.back().empty()) {
    ret.lines.pop_back();
  }
  return ret;  
}

struct DayRun {
  absl::Duration time;
  std::string title;
  std::string part1;
  std::string part2;
};

absl::StatusOr<DayRun> RunDay(advent_of_code::AdventDay* day) {
  if (day == nullptr) {
    return DayRun{.time = absl::Seconds(0), .title = "???", .part1 = "", .part2 = ""};
  }
  DayRun ret;
  absl::Time start = absl::Now();
  ret.title = std::string(day->test_file());
  absl::StatusOr<Input> input = ReadInput(day);
  RETURN_IF_ERROR(input.status());
  ASSIGN_OR_RETURN(ret.part1, day->Part1(absl::MakeSpan(input->lines)));
  ASSIGN_OR_RETURN(ret.part2, day->Part2(absl::MakeSpan(input->lines)));
  ret.time = absl::Now() - start;
  return ret;
}

void BM_Day(benchmark::State& state) {
  std::unique_ptr<advent_of_code::AdventDay> day =
      advent_of_code::CreateAdventDay(state.range(0), state.range(1));
  if (day == nullptr) {
    state.SkipWithError("No advent day");
    return;
  }
  for (auto _ : state) {
    absl::StatusOr<DayRun> run = RunDay(day.get());
    if (!run.ok()) {
      state.SkipWithError(std::string(run.status().message()));
      return;
    }
  }
}

BENCHMARK(BM_Day)->ArgsProduct({
  benchmark::CreateDenseRange(2015, 2023, /*step=*/1),
  benchmark::CreateDenseRange(1, 25, /*step=*/1)
});

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
  for (auto _ : state) {
    for (const auto& day: days) {
      absl::StatusOr<DayRun> run = RunDay(day.get());
      if (!run.ok()) {
        state.SkipWithError(std::string(run.status().message()));
        return;
      }
    }
  }
}

BENCHMARK(BM_WholeYear)->DenseRange(2015, 2023);

}