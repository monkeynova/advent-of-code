#include "advent_of_code/file_test_options.h"

void SetupTestCaseOptions(file_based_test_driver::TestCaseOptions* options) {
  options->RegisterInt64(kPartOption, 0);
  options->RegisterBool(kIgnoreOption, false);
  options->RegisterString(kLongOption, "");
}
