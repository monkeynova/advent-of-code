#include "advent_of_code/2019/int_code.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_split.h"
#include "file_based_test_driver/base/file_util.h"
#include "file_based_test_driver/file_based_test_driver.h"
#include "file_based_test_driver/run_test_case_result.h"
#include "file_based_test_driver/test_case_options.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "main_lib.h"
#include "re2/re2.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

ABSL_FLAG(absl::Duration, run_long_tests, absl::Seconds(0),
          "Flag does nothing but exists to allow success for bazel test ... "
          "--test_arg=--run_long_tests=...");

namespace advent_of_code {

constexpr char kInputOption[] = "input";

void RunTestCase(absl::string_view test_case_with_options,
                 file_based_test_driver::RunTestCaseResult* test_result) {
  file_based_test_driver::TestCaseOptions options;
  options.RegisterString(kInputOption, "");

  std::string test_case = std::string(test_case_with_options);
  if (absl::Status st = options.ParseTestCaseOptions(&test_case); !st.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not parse options: ", st.message()));
    return;
  }

  std::vector<absl::string_view> test_lines = absl::StrSplit(test_case, "\n");
  while (test_lines.back().empty()) {
    test_lines.pop_back();
  }
  absl::StatusOr<IntCode> codes = IntCode::Parse(absl::MakeSpan(test_lines));
  if (!codes.ok()) {
    test_result->AddTestOutput(absl::StrCat("ERROR: Cannot parse Intcode: ",
                                            codes.status().message()));
    return;
  }
  std::vector<absl::string_view> input_str =
      absl::StrSplit(options.GetString(kInputOption), ",");
  std::vector<int64_t> input;
  input.reserve(input_str.size());
  for (absl::string_view str : input_str) {
    input.push_back(0);
    if (!absl::SimpleAtoi(str, &input.back())) {
      test_result->AddTestOutput(
          absl::StrCat("ERROR: Cannot parse input as int: ", str));
      return;
    }
  }
  std::vector<int64_t> output;
  if (absl::Status st = codes->Run(input, &output); !st.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not run code: ", st.message()));
    return;
  }
  for (int outvar : output) {
    test_result->AddTestOutput(absl::StrCat(outvar));
  }
}

TEST(IntCodeTest, FileBasedTest) {
  InitializeAbslFlagsFromGtest();
  google::InstallFailureSignalHandler();
  EXPECT_TRUE(file_based_test_driver::RunTestCasesFromFiles(
      absl::GetFlag(FLAGS_test_file), &RunTestCase));
}

}  // namespace advent_of_code
