#include "advent_of_code/file_test.h"

#include "absl/flags/flag.h"
#include "absl/strings/str_split.h"
#include "base/file_util.h"
#include "re2/re2.h"
#include "run_test_case_result.h"
#include "test_case_options.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

constexpr char kPartOption[] = "part";

std::string TestCaseFileName() { return absl::GetFlag(FLAGS_test_file); }

void RunTestCase(const AdventDay* advent_day,
                 absl::string_view test_case_with_options,
                 file_based_test_driver::RunTestCaseResult* test_result) {
  file_based_test_driver::TestCaseOptions options;
  options.RegisterInt64(kPartOption, 0);

  std::string test_case = std::string(test_case_with_options);
  absl::string_view include_fname;
  RE2 include_pattern{"@include{([^}]*)}"};
  while (RE2::PartialMatch(test_case, include_pattern, &include_fname)) {
    std::string contents;
    // TODO(@monkeynova): This is terrible. Both in terms of using the
    // ::internal namespace (though we're using the same fetching code as
    // the rest of FBTD this way) as well as the need for the ./ without
    // which the load fails.
    if (absl::Status st = file_based_test_driver::internal::GetContents(
            absl::StrCat("./", include_fname), &contents);
        !st.ok()) {
      test_result->AddTestOutput(
          absl::StrCat("ERROR: Unable to include file \"", include_fname,
                       "\": ", st.message()));
      return;
    }
    RE2::Replace(&test_case, absl::StrCat("@include{", include_fname, "}"),
                 contents);
  }

  if (absl::Status st = options.ParseTestCaseOptions(&test_case); !st.ok()) {
    test_result->AddTestOutput(
        absl::StrCat("ERROR: Could not parse options: ", st.message()));
    return;
  }

  std::vector<absl::string_view> test_lines = absl::StrSplit(test_case, "\n");
  while (!test_lines.empty() && test_lines.back().empty()) {
    test_lines.pop_back();
  }
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
