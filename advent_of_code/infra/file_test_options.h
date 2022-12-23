#ifndef ADVENT_OF_CODE_INFRA_FILE_TEST_OPTIONS_H
#define ADVENT_OF_CODE_INFRA_FILE_TEST_OPTIONS_H

#include "absl/status/statusor.h"
#include "file_based_test_driver/test_case_options.h"

namespace advent_of_code {

constexpr char kPartOption[] = "part";
constexpr char kIgnoreOption[] = "ignore";
constexpr char kLongOption[] = "long";
constexpr char kFlagOption[] = "flag";
constexpr char kParamOption[] = "param";

void SetupTestCaseOptions(file_based_test_driver::TestCaseOptions* options);

absl::StatusOr<absl::Duration> ParseLongTestDuration(
    absl::string_view long_option_value);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INFRA_FILE_TEST_OPTIONS_H