#include "advent_of_code/infra/file_test.h"

#include "absl/flags/flag.h"
#include "absl/functional/bind_front.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/infra/file_test_options.h"
#include "advent_of_code/infra/file_util.h"
#include "benchmark/benchmark.h"
#include "file_based_test_driver/base/file_util.h"
#include "file_based_test_driver/file_based_test_driver.h"
#include "file_based_test_driver/run_test_case_result.h"
#include "file_based_test_driver/test_case_options.h"
#include "main_lib.h"
#include "re2/re2.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

ABSL_FLAG(absl::Duration, run_long_tests, absl::Seconds(0),
          "Unless true, tests marked [long=$reason] will be ignored");

ABSL_FLAG(int64_t, part_filter, 0, "If non-zero, only runs the specified part");

namespace advent_of_code {

std::string TestCaseFileName() { return absl::GetFlag(FLAGS_test_file); }

static void CollectLines(
    std::vector<int>* lines, absl::string_view test_case_with_options,
    file_based_test_driver::RunTestCaseResult* test_result) {
  lines->push_back(test_result->line());
}

std::vector<int> TestCaseLines() {
  std::vector<int> lines;
  (void)file_based_test_driver::RunTestCasesFromFiles(
      TestCaseFileName(), absl::bind_front(&CollectLines, &lines));
  return lines;
}

void RunTestCase(const AdventDay* advent_day,
                 absl::string_view test_case_with_options,
                 file_based_test_driver::RunTestCaseResult* test_result) {
  file_based_test_driver::TestCaseOptions options;
  SetupTestCaseOptions(&options);

  std::string test_case = std::string(test_case_with_options);
  if (absl::Status st = HandleTestIncludes(&test_case); !st.ok()) {
    test_result->AddTestOutput(absl::StrCat("ERROR: ", st.message()));
    return;
  }

  if (absl::Status st = options.ParseTestCaseOptions(&test_case); !st.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not parse options: ", st.message()));
    return;
  }

  if (options.GetBool(kIgnoreOption)) {
    test_result->set_ignore_test_output(true);
    return;
  }

  if (std::string long_opt = options.GetString(kLongOption);
      !long_opt.empty()) {
    absl::StatusOr<absl::Duration> long_duration =
        ParseLongTestDuration(long_opt);
    if (!long_duration.ok()) {
      test_result->AddTestOutput(
          absl::StrCat("ERROR: Could not parse 'long' option: ",
                       long_duration.status().message()));
      return;
    }
    if (absl::GetFlag(FLAGS_run_long_tests) < *long_duration) {
      test_result->set_ignore_test_output(true);
      return;
    }
  }

  std::vector<absl::string_view> lines = absl::StrSplit(test_case, "\n");
  absl::Span<absl::string_view> lines_span = absl::MakeSpan(lines);
  // Pull off HACK: prefix lines from the front...
  while (!lines_span.empty() && absl::StartsWith(lines_span[0], "HACK: ")) {
    lines_span = lines_span.subspan(1);
  }
  // ... and empty lines from the end.
  while (!lines_span.empty() && lines_span.back().empty()) {
    lines_span = lines_span.subspan(0, lines_span.size() - 1);
  }
  absl::StatusOr<std::string> output;
  int64_t part_filter = absl::GetFlag(FLAGS_part_filter);
  if (part_filter && part_filter != options.GetInt64(kPartOption)) {
    test_result->set_ignore_test_output(true);
    return;
  }
  switch (options.GetInt64(kPartOption)) {
    case 1: {
      output = advent_day->Part1(lines_span);
      break;
    }
    case 2: {
      output = advent_day->Part2(lines_span);
      break;
    }
    default: {
      test_result->AddTestOutput(absl::StrCat(
          "ERROR: Bad part (must be 1 or 2): ", options.GetInt64(kPartOption)));
      return;
    }
  }
  if (!output.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not run test: ", output.status().message()));
    return;
  }
  test_result->AddTestOutput(*output);
}

bool TestSingleDay(AdventDay* solver) {
  InitializeAbslFlagsFromGtest();
  google::InstallFailureSignalHandler();
  return file_based_test_driver::RunTestCasesFromFiles(
      TestCaseFileName(), absl::bind_front(&RunTestCase, solver));
}

}  // namespace advent_of_code
