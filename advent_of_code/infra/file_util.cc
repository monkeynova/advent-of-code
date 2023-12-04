#include "advent_of_code/infra/file_util.h"

#include <sys/stat.h>

#include <fstream>
#include <iostream>

#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/vlog.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> GetContents(std::string_view filename) {
  struct stat status;
  if (stat(filename.data(), &status) != 0) {
    return absl::NotFoundError(absl::StrCat("Could not find", filename));
  } else if (S_ISREG(status.st_mode) == 0) {
    return absl::FailedPreconditionError(
      absl::StrCat("File is not regular", filename));
  }

  std::ifstream stream(filename, std::ifstream::in);
  if (!stream) {
    // Could be a wider range of reasons.
    return absl::NotFoundError(absl::StrCat("Unable to open: ", filename));
  }
  stream.seekg(0, stream.end);
  int length = stream.tellg();
  stream.seekg(0, stream.beg);
  std::string file_contents;
  file_contents.resize(length);
  stream.read(&file_contents[0], length);
  stream.close();
  return file_contents;
}

absl::Status HandleTestIncludes(std::string* test_case,
                                std::vector<std::string>* includes_out) {
  std::string_view include_fname;
  RE2 include_pattern{"@include{([^}]*)}"};
  while (RE2::PartialMatch(*test_case, include_pattern, &include_fname)) {
    if (includes_out != nullptr) {
      includes_out->emplace_back(include_fname);
    }
    // absl::StrCat(include_fname) ensures that the string_view has a '\0'
    // after it.
    ASSIGN_OR_RETURN(std::string contents,
                     GetContents(absl::StrCat(include_fname)));
    *test_case = absl::StrReplaceAll(
        *test_case,
        {{absl::StrCat("@include{", include_fname, "}"), contents}});
  }
  return absl::OkStatus();
}

}  // namespace advent_of_code
