#include "advent_of_code/2019/day1/day1.h"

#include "absl/flags/flag.h"
#include "absl/functional/bind_front.h"
#include "absl/strings/str_split.h"
#include "file_based_test_driver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "main_lib.h"
#include "run_test_case_result.h"
#include "test_case_options.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

constexpr char kPartOption[] = "part";

void TestCase(const AdventDay* advent_day,
              absl::string_view test_case_with_options,
              file_based_test_driver::RunTestCaseResult* test_result) {
  file_based_test_driver::TestCaseOptions options;
  options.RegisterInt64(kPartOption, 0);

  std::string test_case = std::string(test_case_with_options);
  if (absl::Status st = options.ParseTestCaseOptions(&test_case); !st.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not parse options: ", st.message()));
    return;
  }

  std::vector<absl::string_view> test_lines = absl::StrSplit(test_case, "\n");
  absl::StatusOr<std::vector<std::string>> output;
  switch (options.GetInt64(kPartOption)) {
    case 1: {
      output = advent_day->Part1(test_lines);
      break;
    }
    case 2: {
      output = advent_day->Part2(test_lines);
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
  for (const auto& str : *output) {
    test_result->AddTestOutput(str);
  }
}

TEST(Day1, FileBasedTest) {
  InitializeAbslFlagsFromGtest();
  Day1_2019 solver;
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      absl::GetFlag(FLAGS_test_file), absl::bind_front(&TestCase, &solver)));
}
