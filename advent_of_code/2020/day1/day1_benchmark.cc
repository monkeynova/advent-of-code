#include "advent_of_code/2020/day1/day1.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/file_util.h"
#include "benchmark/benchmark.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "main_lib.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

std::vector<DirtyTestParseResult> global_tests;

void BM_Day(benchmark::State& state) {
  Day1_2020 solver;
  for (auto _ : state) {
    for (const DirtyTestParseResult& test : global_tests) {
      switch (test.part) {
        case 1: (void)solver.Part1(global_tests[state.range(0)].lines); break;
        case 2: (void)solver.Part2(global_tests[state.range(0)].lines); break;
      }
    }
  }
}

BENCHMARK(BM_Day);

int main(int argc, char** argv) {
  std::vector<char*> non_flag_args = InitMain(argc, argv);
  if (non_flag_args.size() > 1) {
    LOG(ERROR) << "Unknown args: " << absl::StrJoin(non_flag_args, ", ");
    return 1;
  }
  std::string file_contents;
  CHECK(GetContents(absl::GetFlag(FLAGS_test_file), &file_contents).ok());
  global_tests = DirtyTestParse(file_contents);
  benchmark::RunSpecifiedBenchmarks();
  return 0;
}





