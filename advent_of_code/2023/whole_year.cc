#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/advent_day.h"
#include "advent_of_code/infra/file_util.h"
#include "advent_of_code/vlog.h"
#include "benchmark/benchmark.h"

namespace advent_of_code {

namespace {

constexpr int kValidDays = 25;

absl::Span<const std::unique_ptr<AdventDay>> AllDays() {
  static std::vector<std::unique_ptr<AdventDay>> days;
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    days = []() {
      LOG(INFO) << "Creating AllDays";
      std::vector<std::unique_ptr<AdventDay>> days;
      for (int day = 1; day <= kValidDays; ++day) {
        days.emplace_back(CreateAdventDay(2023, day));
      }
      return days;
    }();
  }
  return days;
}

struct Input {
  std::string file;
  std::vector<std::string_view> lines;
};

absl::StatusOr<Input> ReadInput(AdventDay* day) {
  if (day == nullptr) return absl::InvalidArgumentError("null day");
  Input ret;
  std::string filename(day->test_file());
  filename.erase(filename.rfind('/'));
  filename.append("/input.txt");
  ASSIGN_OR_RETURN(ret.file, GetContents(filename));
  ret.lines = absl::StrSplit(ret.file, '\n');
  while (!ret.lines.empty() && ret.lines.back().empty()) {
    ret.lines.pop_back();
  }
  return ret;  
}

}  // namespace

static void BM_WholeYear_2023(benchmark::State& state) {
  int bytes_processed = 0;
  for (auto _ : state) {
    AdventDay* day = AllDays()[state.range(0) - 1].get();
    absl::StatusOr<Input> input = ReadInput(day);
    CHECK(input.ok()) << input.status().message();
    bytes_processed += input->file.size();
    absl::Span<std::string_view> in_span(input->lines);
    absl::StatusOr<std::string> ret;
    if (ret = day->Part1(in_span); !ret.ok()) {
      state.SkipWithError(ret.status().message().data());
      return;
    }
    if (ret = day->Part2(in_span); !ret.ok()) {
      state.SkipWithError(ret.status().message().data());
      return;
    }
  }
  state.SetBytesProcessed(bytes_processed);
}

BENCHMARK(BM_WholeYear_2023)->DenseRange(1, kValidDays);

static void BM_WholeYear_ParseOnly(benchmark::State& state) {
  int bytes_processed = 0;
  for (auto _ : state) {
    for (const std::unique_ptr<AdventDay>& day : AllDays()) {
      absl::StatusOr<Input> input = ReadInput(day.get());
      CHECK(input.ok());
      bytes_processed += input->file.size();
      absl::Span<std::string_view> in_span(input->lines);
      benchmark::DoNotOptimize(in_span);
    }
  }
  state.SetBytesProcessed(bytes_processed);
}

BENCHMARK(BM_WholeYear_ParseOnly);

static void BM_WholeYear(benchmark::State& state) {
  int bytes_processed = 0;
  for (auto _ : state) {
    for (const std::unique_ptr<AdventDay>& day : AllDays()) {
      absl::StatusOr<Input> input = ReadInput(day.get());
      CHECK(input.ok());
      bytes_processed += input->file.size();
      absl::Span<std::string_view> in_span(input->lines);
      absl::StatusOr<std::string> ret;
      if (ret = day->Part1(in_span); !ret.ok()) {
        state.SkipWithError(ret.status().message().data());
        return;
      }
      if (ret = day->Part2(in_span); !ret.ok()) {
        state.SkipWithError(ret.status().message().data());
        return;
      }
    }
  }
  state.SetBytesProcessed(bytes_processed);
}

BENCHMARK(BM_WholeYear);

}  // namespace advent_of_code