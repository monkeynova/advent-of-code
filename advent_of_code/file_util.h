#ifndef ADVENT_OF_CODE_FILE_UTIL_H
#define ADVENT_OF_CODE_FILE_UTIL_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "file_based_test_driver/test_case_options.h"

absl::Status GetContents(absl::string_view filename,
                         std::string* file_contents);

absl::Status HandleTestIncludes(std::string* test_case);

constexpr char kPartOption[] = "part";
constexpr char kIgnoreOption[] = "ignore";
constexpr char kLongOption[] = "long";

void SetupTestCaseOptions(file_based_test_driver::TestCaseOptions* options);

#endif  // ADVENT_OF_CODE_FILE_UTIL_H