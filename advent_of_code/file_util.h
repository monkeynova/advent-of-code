#ifndef ADVENT_OF_CODE_FILE_UTIL_H
#define ADVENT_OF_CODE_FILE_UTIL_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

absl::Status GetContents(absl::string_view filename,
                         std::string* file_contents);

absl::Status HandleTestIncludes(std::string* test_case);

#endif  // ADVENT_OF_CODE_FILE_UTIL_H