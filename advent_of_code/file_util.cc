#include "advent_of_code/file_util.h"

#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

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

std::vector<DirtyTestParseResult> DirtyTestParse(absl::string_view contents) {
  std::vector<DirtyTestParseResult> ret;
  for (absl::string_view test : absl::StrSplit(contents, "\n==\n")) {
      std::vector<absl::string_view> lines = absl::StrSplit(test, "\n");
      int start = 0;
      int part = 0;
      // Skip comments.
      while (start < lines.size() && (lines.empty() || lines[start][0] == '#')) ++start;
      // Skip options.
      while (start < lines.size() && (lines.empty() || lines[start][0] == '[')) {
          LOG(WARNING) << lines[start];
          LOG(WARNING) << part;
          RE2::PartialMatch(lines[start], "[part=(\\d+)]", &part);
          LOG(WARNING) << part;
          ++start;
      }
      if (start < lines.size()) {
        ret.push_back(DirtyTestParseResult{
            .part = part,
            .lines = std::vector<absl::string_view>(lines.begin() + start, lines.end())});
      }
  }
  return ret;
}

