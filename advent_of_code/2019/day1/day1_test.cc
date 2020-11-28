#include "advent_of_code/2019/day1/day1.h"

#include "absl/functional/bind_front.h"
#include "absl/strings/str_split.h"
#include "file_based_test_driver.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "run_test_case_result.h"
#include "test_case_options.h"

using RunCase = std::function<absl::StatusOr<std::vector<std::string>>(const std::vector<absl::string_view>&)>;

void TestCase(absl::string_view test_case, RunCase run_case,
              file_based_test_driver::RunTestCaseResult* test_result) {
  std::vector<absl::string_view> test_lines = absl::StrSplit(test_case, "\n");
  absl::StatusOr<std::vector<std::string>> output = run_case(test_lines);
  if (!output.ok()) {
      test_result->AddTestOutput(
          absl::StrCat("ERROR: Could not run test: ", output.status().message()));
      return;
  }
  for (const auto& str : *output) {
    test_result->AddTestOutput(str);
  }
}

void TestPart1(absl::string_view test_case,
              file_based_test_driver::RunTestCaseResult* test_result) {
  Day1_2019 solver;
  TestCase(test_case, absl::bind_front(&Day1_2019::Part1, &solver), test_result);
}

void TestPart2(absl::string_view test_case,
              file_based_test_driver::RunTestCaseResult* test_result) {
  Day1_2019 solver;
  TestCase(test_case, absl::bind_front(&Day1_2019::Part2, &solver), test_result);
}

TEST(Part1, FileBasedTest) {
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      "advent_of_code/2019/day1/part1.test", TestPart1));
}

TEST(Part2, FileBasedTest) {
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      "advent_of_code/2019/day1/part2.test", TestPart2));
}