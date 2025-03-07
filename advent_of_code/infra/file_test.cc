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

void RunTestCase(AdventDay* advent_day, std::string_view test_case_with_options,
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
    for (std::string_view key_value : absl::StrSplit(flag_opt, ",")) {
      const auto& [key, value] = std::pair<std::string_view, std::string_view>(
          absl::StrSplit(key_value, absl::MaxSplits("=", 2)));
      absl::CommandLineFlag* flag = absl::FindCommandLineFlag(key);
      if (!flag) {
        test_result->AddTestOutput(
            absl::StrCat("ERROR: Could not find flag '", key, "'"));
        return;
      }
      std::string error;
      if (!flag->ParseFrom(value, &error)) {
        test_result->AddTestOutput(absl::StrCat("ERROR: Could not set flag '",
                                                key, "' to value: '", value,
                                                "': ", error));
        return;
      }
    }
  }

#ifdef _WIN32
  std::vector<std::string_view> lines = absl::StrSplit(test_case, "\r\n");
#else
  std::vector<std::string_view> lines = absl::StrSplit(test_case, '\n');
#endif
  absl::Span<std::string_view> lines_span = absl::MakeSpan(lines);
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
  advent_day->set_param(options.GetString(kParamOption));
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
  test_result->AddTestOutput(*output);
}

bool TestSingleDay(AdventDay* day) {
  absl::InstallFailureSignalHandler({});
  return file_based_test_driver::RunTestCasesFromFiles(
      day->test_file(), absl::bind_front(&RunTestCase, day));
}

}  // namespace advent_of_code
