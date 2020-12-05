#include "advent_of_code/file_benchmark.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

absl::StatusOr<std::vector<DirtyTestParseResult>> FileBenchmarkTests() {
  std::string file_contents;
  if (absl::Status st =
          GetContents(absl::GetFlag(FLAGS_test_file), &file_contents);
      !st.ok()) {
    return st;
  }
  return DirtyTestParse(file_contents);
}

void BM_Day(benchmark::State& state, AdventDay* day) {
  absl::StatusOr<std::vector<DirtyTestParseResult>> tests =
      FileBenchmarkTests();
  if (!tests.ok()) {
    return BM_Day_SetError(state, tests.status().message());
  }
  if (state.range(0) >= tests->size()) {
    return BM_Day_SetError(state, "Bad test");
  }
  const DirtyTestParseResult& test = (*tests)[state.range(0)];
  std::vector<absl::string_view> lines = absl::StrSplit(test.test, "\n");
  while (!lines.empty() && lines.back().empty()) lines.pop_back();
  switch (test.part) {
    case 1: {
      for (auto _ : state) {
        absl::StatusOr<std::vector<std::string>> st = day->Part1(lines);
        if (!st.ok()) {
          return BM_Day_SetError(state, st.status().message());
        }
      }
      break;
    }
    case 2: {
      for (auto _ : state) {
        absl::StatusOr<std::vector<std::string>> st = day->Part2(lines);
        if (!st.ok()) {
          return BM_Day_SetError(state, st.status().message());
        }
      }
      break;
    }
    default: {
      return BM_Day_SetError(state, "Bad part");
    }
  }
}
