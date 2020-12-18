#include "advent_of_code/file_util.h"

#include <sys/stat.h>

#include <fstream>
#include <iostream>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

static absl::Status NullFreeString(absl::string_view str,
                                   std::string* out_str) {
  if (str.find('\0') != absl::string_view::npos) {
    return absl::InvalidArgumentError(
        absl::StrCat("filename contains null characters: ", str));
  }
  *out_str = std::string(str);
  return absl::OkStatus();
}

absl::Status GetContents(absl::string_view filename,
                         std::string* file_contents) {
  // Because we are using a c api, check for in-string nulls.
  std::string filename_str;
  if (absl::Status status = NullFreeString(filename, &filename_str);
      !status.ok()) {
    return status;
  }

  struct stat status;
  if (stat(filename.data(), &status) != 0) {
    return absl::Status(absl::StatusCode::kNotFound,
                        absl::StrCat("Could not find", filename));
  } else if (S_ISREG(status.st_mode) == 0) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        absl::StrCat("File is not regular", filename));
  }

  std::ifstream stream(std::string(filename), std::ifstream::in);
  if (!stream) {
    // Could be a wider range of reasons.
    return absl::Status(absl::StatusCode::kNotFound,
                        absl::StrCat("Unable to open: ", filename));
  }
  stream.seekg(0, stream.end);
  int length = stream.tellg();
  stream.seekg(0, stream.beg);
  file_contents->clear();
  file_contents->resize(length);
  stream.read(&(*file_contents)[0], length);
  stream.close();
  return absl::OkStatus();
}

absl::Status HandleTestIncludes(std::string* test_case) {
  absl::string_view include_fname;
  RE2 include_pattern{"@include{([^}]*)}"};
  while (RE2::PartialMatch(*test_case, include_pattern, &include_fname)) {
    std::string contents;
    // TODO(@monkeynova): This is terrible. Both in terms of using the
    // ::internal namespace (though we're using the same fetching code as
    // the rest of FBTD this way) as well as the need for the ./ without
    // which the load fails.
    if (absl::Status st =
            GetContents(absl::StrCat("./", include_fname), &contents);
        !st.ok()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Unable to include file \"", include_fname, "\": ", st.message()));
      return st;
    }
    *test_case = absl::StrReplaceAll(
        *test_case,
        {{absl::StrCat("@include{", include_fname, "}"), contents}});
  }
  return absl::OkStatus();
}

}  // namespace advent_of_code
