#include "advent_of_code/file_benchmark.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/file_test_options.h"
#include "file_based_test_driver/test_case_options.h"
#include "glog/logging.h"
#include "re2/re2.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

ABSL_FLAG(bool, run_long_tests, false,
          "Unless true, tests marked [long=$reason] will be ignored");

struct DirtyTestParseResult {
  int part;
  std::string test;
  file_based_test_driver::TestCaseOptions options;
};

absl::Status FinishTest(DirtyTestParseResult* result) {
  SetupTestCaseOptions(&result->options);
  if (absl::Status st = result->options.ParseTestCaseOptions(&result->test);
      !st.ok())
    return st;
  if (absl::Status st = HandleTestIncludes(&result->test); !st.ok()) return st;
  return absl::OkStatus();
}

absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>>
DirtyTestParse(absl::string_view contents) {
  std::vector<std::unique_ptr<DirtyTestParseResult>> ret;
  auto next = absl::make_unique<DirtyTestParseResult>();
  bool in_answer = false;
  bool in_test = false;
  for (absl::string_view line : absl::StrSplit(contents, "\n")) {
    if (line == "--") {
      in_answer = true;
    } else if (line == "==") {
      in_answer = false;
      in_test = false;
      if (absl::Status st = FinishTest(next.get()); !st.ok()) return st;
      ret.push_back(std::move(next));
      next = absl::make_unique<DirtyTestParseResult>();
      continue;
    }
    if (in_answer) continue;
    if (!in_test && line.empty()) continue;
    if (!in_test && line[0] == '#') continue;
    in_test = true;
    absl::StrAppend(&next->test, line, "\n");
  }
  if (absl::Status st = FinishTest(next.get()); !st.ok()) return st;
  ret.push_back(std::move(next));
  return ret;
}

absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>>
FileBenchmarkTests() {
  std::string file_contents;
  if (absl::Status st =
          GetContents(absl::GetFlag(FLAGS_test_file), &file_contents);
      !st.ok()) {
    return st;
  }
  return DirtyTestParse(file_contents);
}

absl::StatusOr<int> FileBenchmarkTestCount() {
  absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>> tests =
      FileBenchmarkTests();
  if (!tests.ok()) return tests.status();
  return tests->size();
}

void BM_Day(benchmark::State& state, AdventDay* day) {
  absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>> tests =
      FileBenchmarkTests();
  if (!tests.ok()) {
    return BM_Day_SetError(state, tests.status().message());
  }
  if (state.range(0) >= tests->size()) {
    return BM_Day_SetError(state, "Bad test");
  }
  const DirtyTestParseResult* test = (*tests)[state.range(0)].get();
  std::vector<absl::string_view> lines = absl::StrSplit(test->test, "\n");
  while (!lines.empty() && lines.back().empty()) lines.pop_back();

  int part = test->options.GetInt64(kPartOption);
  if (test->options.GetString(kLongOption) != "" &&
      !absl::GetFlag(FLAGS_run_long_tests)) {
    state.SetLabel(absl::StrCat("Part: ", part, "; *** SKIPPED (long) ****"));
    for (auto _ : state) {
    }
    return;
  }
  state.SetLabel(absl::StrCat("Part: ", part));
  switch (part) {
    case 1: {
      for (auto _ : state) {
        absl::StatusOr<std::vector<std::string>> st = day->Part1(absl::MakeSpan(lines));
        if (!st.ok()) {
          return BM_Day_SetError(state, st.status().message());
        }
      }
      break;
    }
    case 2: {
      for (auto _ : state) {
        absl::StatusOr<std::vector<std::string>> st = day->Part2(absl::MakeSpan(lines));
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
