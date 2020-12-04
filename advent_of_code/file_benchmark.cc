#include "advent_of_code/file_benchmark.h"

#include "absl/flags/flag.h"

ABSL_FLAG(std::string, test_file, "",
          "The file which contains the file based test driver tests");

absl::StatusOr<std::vector<DirtyTestParseResult>> FileBenchmarkTests() {
  std::string file_contents;
  if (absl::Status st =
          GetContents(absl::GetFlag(FLAGS_test_file), &file_contents);
      !st.ok()) {
    return st;
  }
  std::vector<DirtyTestParseResult> ret = DirtyTestParse(file_contents);
  return ret;
}
