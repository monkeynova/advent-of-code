#include "advent_of_code/infra/file_benchmark.h"

#include "absl/flags/flag.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/infra/file_flags.h"
#include "advent_of_code/infra/file_test_options.h"
#include "file_based_test_driver/test_case_options.h"

ABSL_FLAG(std::string, bechmark_file_include_filter, "",
          "If non-empty, specifies a string such that benchmarks are filtered "
          "to only run if they (a) have an include and (b) that include "
          "contains the filter as a substring.");

namespace advent_of_code {

struct DirtyTestParseResult {
  int part;
  std::string test;
  file_based_test_driver::TestCaseOptions options;
  std::vector<std::string> includes;
};

absl::Status FinishTest(DirtyTestParseResult* result) {
  SetupTestCaseOptions(&result->options);
  RETURN_IF_ERROR(result->options.ParseTestCaseOptions(&result->test));
  RETURN_IF_ERROR(HandleTestIncludes(&result->test, &result->includes));
  return absl::OkStatus();
}

absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>>
DirtyTestParse(std::string_view contents) {
  std::vector<std::unique_ptr<DirtyTestParseResult>> ret;
  auto next = absl::make_unique<DirtyTestParseResult>();
  bool in_answer = false;
  bool in_test = false;
  for (std::string_view line : absl::StrSplit(contents, "\n")) {
    if (line == "--") {
      in_answer = true;
    } else if (line == "==") {
      in_answer = false;
      in_test = false;
      RETURN_IF_ERROR(FinishTest(next.get()));
      ret.push_back(std::move(next));
      next = absl::make_unique<DirtyTestParseResult>();
      continue;
    }
    if (in_answer) continue;
    if (!in_test && line.empty()) continue;
    if (!in_test && line[0] == '#') continue;
    in_test = true;
    if (in_test && !line.empty() && line[0] == '\\') {
      line = line.substr(1);
    }
    absl::StrAppend(&next->test, line, "\n");
  }
  RETURN_IF_ERROR(FinishTest(next.get()));
  ret.push_back(std::move(next));
  return ret;
}

absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>>
FileBenchmarkTests(std::string_view test_file) {
  ASSIGN_OR_RETURN(std::string file_contents, GetContents(test_file));
  return DirtyTestParse(file_contents);
}

absl::StatusOr<int> FileBenchmarkTestCount(AdventDay* day) {
  ASSIGN_OR_RETURN(std::vector<std::unique_ptr<DirtyTestParseResult>> tests,
                   FileBenchmarkTests(day->test_file()));
  return tests.size();
}

int FileBenchmarkMaxIdx(AdventDay* day) {
  return FileBenchmarkTests(day->test_file())->size() - 1;
}

void BM_Day(benchmark::State& state, AdventDay* day) {
  absl::StatusOr<std::vector<std::unique_ptr<DirtyTestParseResult>>> tests =
      FileBenchmarkTests(day->test_file());
  if (!tests.ok()) {
    return BM_Day_SetError(state, tests.status().ToString());
  }
  if (state.range(0) >= tests->size()) {
    return BM_Day_SetError(state, "Bad test");
  }
  const DirtyTestParseResult* test = (*tests)[state.range(0)].get();
  std::vector<std::string_view> lines = absl::StrSplit(test->test, "\n");
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
  std::string skip;
  if (lines_span.empty()) {
    skip = "(empty test)";
  }
  int part = test->options.IsExplicitlySet(kPartOption)
                 ? test->options.GetInt64(kPartOption)
                 : 0;
  if (skip.empty()) {
    int64_t part_filter = absl::GetFlag(FLAGS_part_filter);
    if (part_filter && part_filter != part) {
      skip = absl::StrCat("(part=", part, ")");
    }
  }
  if (skip.empty()) {
    static std::string include_filter = 
        absl::GetFlag(FLAGS_bechmark_file_include_filter);
    if (!include_filter.empty()) {
      auto matches = [&](const std::string& test) {
        return test.find(include_filter) != std::string::npos;
      };
      if (!absl::c_any_of(test->includes, matches)) {
        skip = absl::StrCat("(no include contains '", include_filter, "'");
      }
    }
  }
  if (!skip.empty()) {
    state.SkipWithMessage(
        absl::StrCat("Part: ", part, "; *** SKIPPED ", skip, " ****"));
    return;
  }

  day->set_param(test->options.GetString(kParamOption));

  state.SetLabel(absl::StrCat("Part: ", part));
  switch (part) {
    case 1: {
      for (auto _ : state) {
        absl::StatusOr<std::string> st = day->Part1(lines_span);
        if (!st.ok()) {
          return BM_Day_SetError(state, st.status().ToString());
        }
      }
      break;
    }
    case 2: {
      for (auto _ : state) {
        absl::StatusOr<std::string> st = day->Part2(lines_span);
        if (!st.ok()) {
          return BM_Day_SetError(state, st.status().ToString());
        }
      }
      break;
    }
    default: {
      return BM_Day_SetError(state, "Bad part");
    }
  }
}

}  // namespace advent_of_code
