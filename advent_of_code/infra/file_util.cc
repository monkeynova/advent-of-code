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
  int length = status.st_size;

  FILE* f = fopen(filename.data(), "r");
  if (f == nullptr) {
    // Could be a wider range of reasons.
    return absl::NotFoundError(
        absl::StrFormat("Error opening %s: %s", filename, strerror(errno)));
  }
  std::string file_contents(length, 0);
  if (fread(&file_contents[0], 1, length, f) != length) {
    return absl::UnavailableError(
        absl::StrFormat("Error reading %d bytes from %s: %s", length, filename,
                        strerror(errno)));
  }
  if (fclose(f) != 0) {
    return absl::UnavailableError(
        absl::StrFormat("Error closing %s: %s", filename, strerror(errno)));
  }
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
