#ifndef ADVENT_OF_CODE_INFRA_FILE_UTIL_H
#define ADVENT_OF_CODE_INFRA_FILE_UTIL_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace advent_of_code {

absl::Status GetContents(std::string_view filename,
                         std::string* file_contents);

absl::Status HandleTestIncludes(std::string* test_case);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INFRA_FILE_UTIL_H