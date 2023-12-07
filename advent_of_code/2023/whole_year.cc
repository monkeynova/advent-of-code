#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2023/day01/day01.h"
#include "advent_of_code/2023/day02/day02.h"
#include "advent_of_code/2023/day03/day03.h"
#include "advent_of_code/2023/day04/day04.h"
#include "advent_of_code/2023/day05/day05.h"
#include "advent_of_code/2023/day06/day06.h"
#include "advent_of_code/2023/day07/day07.h"
#include "advent_of_code/2023/day08/day08.h"
#include "advent_of_code/infra/file_util.h"
#include "advent_of_code/vlog.h"
#include "benchmark/benchmark.h"

namespace advent_of_code {

template <typename Day>
static void BM_WholeYear_ByDay(benchmark::State& state) {
  for (auto _ : state) {
    Day day;
    std::string filename(day.test_file());
    filename.erase(filename.rfind('/'));
    filename.append("/input.txt");
    absl::StatusOr<std::string> file = GetContents(filename);
    CHECK_OK(file);
    std::vector<std::string_view> input_own = absl::StrSplit(*file, '\n');
    while (!input_own.empty() && input_own.back().empty()) {
      input_own.pop_back();
    }
    absl::Span<std::string_view> input(input_own);
    absl::StatusOr<std::string> ret;
    if (ret = day.Part1(input); !ret.ok()) {
      state.SkipWithError(ret.status().message().data());
      return;
    }
    if (ret = day.Part2(input); !ret.ok()) {
      state.SkipWithError(ret.status().message().data());
      return;
    }
  }
}

BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_01);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_02);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_03);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_04);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_05);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_06);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_07);
BENCHMARK_TEMPLATE(BM_WholeYear_ByDay, Day_2023_08);

static void BM_WholeYear(benchmark::State& state) {
  std::vector<std::unique_ptr<AdventDay>> days;
  days.emplace_back(new Day_2023_01());
  days.emplace_back(new Day_2023_02());
  days.emplace_back(new Day_2023_03());
  days.emplace_back(new Day_2023_04());
  days.emplace_back(new Day_2023_05());
  days.emplace_back(new Day_2023_06());
  days.emplace_back(new Day_2023_07());

  for (auto _ : state) {
    for (const std::unique_ptr<AdventDay>& day : days) {
      std::string filename(day->test_file());
      filename.erase(filename.rfind('/'));
      filename.append("/input.txt");
      absl::StatusOr<std::string> file = GetContents(filename);
      CHECK_OK(file);
      std::vector<std::string_view> input_own = absl::StrSplit(*file, '\n');
      while (!input_own.empty() && input_own.back().empty()) {
        input_own.pop_back();
      }
      absl::Span<std::string_view> input(input_own);
      absl::StatusOr<std::string> ret;
      if (ret = day->Part1(input); !ret.ok()) {
        state.SkipWithError(ret.status().message().data());
        return;
      }
      if (ret = day->Part2(input); !ret.ok()) {
        state.SkipWithError(ret.status().message().data());
        return;
      }
    }
  }
}

BENCHMARK(BM_WholeYear);

}  // namespace advent_of_code