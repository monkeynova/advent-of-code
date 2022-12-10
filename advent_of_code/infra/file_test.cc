#include "advent_of_code/infra/file_test.h"

#include "absl/debugging/failure_signal_handler.h"
#include "absl/flags/commandlineflag.h"
#include "absl/flags/flag.h"
#include "absl/flags/reflection.h"
#include "absl/functional/bind_front.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/infra/file_flags.h"
#include "advent_of_code/infra/file_test_options.h"
#include "advent_of_code/infra/file_util.h"
#include "benchmark/benchmark.h"
#include "file_based_test_driver/base/file_util.h"
#include "file_based_test_driver/file_based_test_driver.h"
#include "file_based_test_driver/run_test_case_result.h"
#include "file_based_test_driver/test_case_options.h"
#include "re2/re2.h"

namespace advent_of_code {

constexpr absl::Duration kMinAllowedTestTime = absl::Seconds(15);

void RunTestCase(const AdventDay* advent_day,
                 absl::string_view test_case_with_options,
                 file_based_test_driver::RunTestCaseResult* test_result) {
  std::unique_ptr<absl::FlagSaver> flag_saver;

  file_based_test_driver::TestCaseOptions options;
  SetupTestCaseOptions(&options);

  std::string test_case = std::string(test_case_with_options);
  if (absl::Status st = HandleTestIncludes(&test_case); !st.ok()) {
    test_result->AddTestOutput(absl::StrCat("ERROR: ", st.ToString()));
    return;
  }

  if (absl::Status st = options.ParseTestCaseOptions(&test_case); !st.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not parse options: ", st.ToString()));
    return;
  }

  if (options.GetBool(kIgnoreOption)) {
    test_result->set_ignore_test_output(true);
    return;
  }

  if (std::string flag_opt = options.GetString(kFlagOption);
      !flag_opt.empty()) {
    flag_saver = absl::make_unique<absl::FlagSaver>();
    for (absl::string_view key_value : absl::StrSplit(flag_opt, ",")) {
      const auto& [key, value] =
          std::pair<absl::string_view, absl::string_view>(
              absl::StrSplit(key_value, absl::MaxSplits("=", 2)));
      absl::CommandLineFlag* flag = absl::FindCommandLineFlag(key);
      if (!flag) {
        test_result->AddTestOutput(absl::StrCat(
          "ERROR: Could not find flag '", key, "'"));
        return;
      }
      std::string error;
      if (!flag->ParseFrom(value, &error)) {
        test_result->AddTestOutput(absl::StrCat(
          "ERROR: Could not set flag '", key, "' to value: '", value, "': ",
          error));
        return;
      }
    }
  }

  absl::StatusOr<absl::Duration> test_allowed_duration = kMinAllowedTestTime;
  if (std::string long_opt = options.GetString(kLongOption);
      !long_opt.empty()) {
    test_allowed_duration = ParseLongTestDuration(long_opt);
    if (!test_allowed_duration.ok()) {
      test_result->AddTestOutput(
          absl::StrCat("ERROR: Could not parse 'long' option: ",
                       test_allowed_duration.status().ToString()));
      return;
    }
    if (*test_allowed_duration < kMinAllowedTestTime) {
      test_allowed_duration = kMinAllowedTestTime;
    }
    if (absl::GetFlag(FLAGS_run_long_tests) < *test_allowed_duration) {
      if (absl::GetFlag(FLAGS_fail_if_long_skip)) {
        test_result->AddTestOutput(absl::StrCat("ERROR: Skipped long test"));
      } else {
        test_result->set_ignore_test_output(true);
      }
      return;
    }
  }

  std::vector<absl::string_view> lines = absl::StrSplit(test_case, "\n");
  absl::Span<absl::string_view> lines_span = absl::MakeSpan(lines);
  // Pull off HACK: prefix lines from the front...
  while (!lines_span.empty() && absl::StartsWith(lines_span[0], "HACK:")) {
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
  absl::Time start = absl::Now();
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
        absl::StrCat("ERROR: ", output.status().ToString()));
    return;
  }
  absl::Time end = absl::Now();
  if (absl::GetFlag(FLAGS_fail_if_long_skip) &&
      end - start > *test_allowed_duration) {
    test_result->AddTestOutput(absl::StrCat(
        "ERROR: Test took took long: took=", (end - start) / absl::Seconds(1),
        "s; allowed=", *test_allowed_duration / absl::Seconds(1), "s"));
    return;
  }
  test_result->AddTestOutput(*output);
}

bool TestSingleDay(AdventDay* day) {
  absl::InstallFailureSignalHandler({});
  return file_based_test_driver::RunTestCasesFromFiles(
      day->test_file(), absl::bind_front(&RunTestCase, day));
}

}  // namespace advent_of_code
