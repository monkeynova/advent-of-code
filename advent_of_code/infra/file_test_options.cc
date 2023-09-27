#include "advent_of_code/infra/file_test_options.h"

#include "absl/strings/str_cat.h"
#include "re2/re2.h"

namespace advent_of_code {

void SetupTestCaseOptions(file_based_test_driver::TestCaseOptions* options) {
  options->RegisterInt64(kPartOption, 0);
  options->RegisterBool(kIgnoreOption, false);
  options->RegisterString(kLongOption, "");
  options->RegisterString(kFlagOption, "");
  options->RegisterString(kParamOption, "");
}

absl::StatusOr<absl::Duration> ParseLongTestDuration(
    absl::string_view long_option_value) {
  RE2 valid_re{"(\\d+[sm]) (\\((\\d+[sm]) opt\\) )?\\d+\\.\\d+\\.\\d+"};
  absl::string_view default_duration;
  absl::string_view opt_duration;
  absl::string_view ignore;
  if (!RE2::FullMatch(long_option_value, valid_re, &default_duration, &ignore,
                      &opt_duration)) {
    return absl::InvalidArgumentError(
        absl::StrCat("Bad 'long' option: ", long_option_value, "; must match ",
                     valid_re.pattern()));
  }

  std::string error;
  absl::Duration ret;
#ifdef NDEBUG
  if (!AbslParseFlag(opt_duration, &ret, &error)) {
    return absl::InvalidArgumentError(error);
  }
#else
  if (!AbslParseFlag(default_duration, &ret, &error)) {
    return absl::InvalidArgumentError(error);
  }
#endif

  return ret;
}

}  // namespace advent_of_code
