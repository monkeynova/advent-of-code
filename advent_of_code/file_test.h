#ifndef ADVENT_OF_CODE_FILE_TEST_H
#define ADVENT_OF_CODE_FILE_TEST_H

#include "absl/functional/bind_front.h"
#include "advent_of_code/advent_day.h"
#include "benchmark/benchmark.h"
#include "file_based_test_driver/file_based_test_driver.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "main_lib.h"

namespace advent_of_code {

std::string TestCaseFileName();
std::vector<int> TestCaseLines();

void RunTestCase(const AdventDay* advent_day,
                 absl::string_view test_case_with_options,
                 file_based_test_driver::RunTestCaseResult* test_result);

template <typename Day>
class FileTest : public ::testing::Test {
 public:
  bool RunTest() {
    InitializeAbslFlagsFromGtest();
    google::InstallFailureSignalHandler();
    Day solver;
    return file_based_test_driver::RunTestCasesFromFiles(
        TestCaseFileName(), absl::bind_front(&RunTestCase, &solver));
  }
};

#endif  //  ADVENT_OF_CODE_FILE_TEST_H
}  // namespace advent_of_code
