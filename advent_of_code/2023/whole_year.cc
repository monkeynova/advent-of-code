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
#include "advent_of_code/2023/day09/day09.h"
#include "advent_of_code/2023/day10/day10.h"
#include "advent_of_code/2023/day11/day11.h"
#include "advent_of_code/2023/day12/day12.h"
#include "advent_of_code/2023/day13/day13.h"
#include "advent_of_code/2023/day14/day14.h"
#include "advent_of_code/2023/day15/day15.h"
#include "advent_of_code/2023/day16/day16.h"
#include "advent_of_code/2023/day17/day17.h"
#include "advent_of_code/2023/day18/day18.h"
#include "advent_of_code/2023/day19/day19.h"
#include "advent_of_code/2023/day20/day20.h"
#include "advent_of_code/2023/day21/day21.h"
#include "advent_of_code/2023/day22/day22.h"
#include "advent_of_code/2023/day23/day23.h"
#include "advent_of_code/2023/day24/day24.h"
#include "advent_of_code/infra/file_util.h"
#include "advent_of_code/vlog.h"
#include "benchmark/benchmark.h"

namespace advent_of_code {

absl::Span<const std::unique_ptr<AdventDay>> AllDays() {
  static std::vector<std::unique_ptr<AdventDay>> days = []() {
    std::vector<std::unique_ptr<AdventDay>> days;
    days.emplace_back(new Day_2023_01());
    days.emplace_back(new Day_2023_02());
    days.emplace_back(new Day_2023_03());
    days.emplace_back(new Day_2023_04());
    days.emplace_back(new Day_2023_05());
    days.emplace_back(new Day_2023_06());
    days.emplace_back(new Day_2023_07());
    days.emplace_back(new Day_2023_08());
    days.emplace_back(new Day_2023_09());
    days.emplace_back(new Day_2023_10());
    days.emplace_back(new Day_2023_11());
    days.back()->set_param("1000000");
    days.emplace_back(new Day_2023_12());
    days.emplace_back(new Day_2023_13());
    days.emplace_back(new Day_2023_14());
    days.emplace_back(new Day_2023_15());
    days.emplace_back(new Day_2023_16());
    days.emplace_back(new Day_2023_17());
    days.emplace_back(new Day_2023_18());
    days.emplace_back(new Day_2023_19());
    days.emplace_back(new Day_2023_20());
    days.emplace_back(new Day_2023_21());
    days.back()->set_param("64,26501365");
    days.emplace_back(new Day_2023_22());
    days.emplace_back(new Day_2023_23());
    days.emplace_back(new Day_2023_24());
    days.back()->set_param("200000000000000,400000000000000");
    return days;
  }();
  return days;
}

struct Input {
  std::string file;
  std::vector<std::string_view> lines;
};

absl::StatusOr<Input> ReadInput(AdventDay* day) {
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

static void BM_WholeYear_2023(benchmark::State& state) {
  int bytes_processed = 0;
  for (auto _ : state) {
    AdventDay* day = AllDays()[state.range(0) - 1].get();
    absl::StatusOr<Input> input = ReadInput(day);
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
  state.SetBytesProcessed(bytes_processed);
}

BENCHMARK(BM_WholeYear_2023)->DenseRange(1, AllDays().size());

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