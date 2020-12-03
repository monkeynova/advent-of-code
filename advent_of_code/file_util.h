#ifndef ADVENT_OF_CODE_FILE_UTIL_H
#define ADVENT_OF_CODE_FILE_UTIL_H

#include <vector>

#include "absl/status/status.h"

absl::Status GetContents(absl::string_view filename,
                         std::string* file_contents);

struct DirtyTestParseResult {
  int part;
  std::vector<absl::string_view> lines;
};

std::vector<DirtyTestParseResult> DirtyTestParse(absl::string_view contents);

#endif  // ADVENT_OF_CODE_FILE_UTIL_H