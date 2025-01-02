#include "benchmark/benchmark.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/file_util.h"
#include "main_lib.h"
#include "vlog.h"

namespace {

struct Input {
  Input() = default;

  // Neigher copyable nor movable. `lines` refers into `file`.
  Input(const Input&) = delete;
  Input(Input&&) = delete;

  std::string file;
  std::vector<std::string_view> lines;
};

absl::Status ReadInput(advent_of_code::AdventDay* day, Input* ret) {
  if (day == nullptr) return absl::InvalidArgumentError("null day");
  std::string filename(day->test_file());
  filename.erase(filename.rfind('/'));
  filename.append("/input.txt");
  ASSIGN_OR_RETURN(ret->file, advent_of_code::GetContents(filename));
#ifdef _WIN32
  ret->lines = absl::StrSplit(ret->file, "\r\n");
#else
  ret->lines = absl::StrSplit(ret->file, '\n');
#endif
  while (!ret->lines.empty() && ret->lines.back().empty()) {
    ret->lines.pop_back();
  }
  return absl::OkStatus();
}

struct DayRun {
  absl::Duration time;
  std::string title;
  std::string part1;
  std::string part2;
  int bytes_processed;
};

absl::StatusOr<DayRun> RunDay(advent_of_code::AdventDay* day) {
  if (day == nullptr) {
    return absl::NotFoundError("Null day");
  }
  DayRun ret;
  absl::Time start = absl::Now();
  ret.title = std::string(day->title());
  Input input;
  RETURN_IF_ERROR(ReadInput(day, &input));
  ret.bytes_processed = input.file.size();
  absl::StatusOr<std::string> part1 = day->Part1(absl::MakeSpan(input.lines));
  if (part1.ok())
    ret.part1 = *std::move(part1);
  else
    ret.part1 = absl::StrCat("Error: ", part1.status().message());
  absl::StatusOr<std::string> part2 = day->Part2(absl::MakeSpan(input.lines));
  if (part2.ok())
    ret.part2 = *std::move(part2);
  else
    ret.part2 = absl::StrCat("Error: ", part2.status().message());
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
  int64_t bytes_processed = 0;
  for (auto _ : state) {
    absl::StatusOr<DayRun> run = RunDay(day.get());
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
      absl::StatusOr<DayRun> run = RunDay(day.get());
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