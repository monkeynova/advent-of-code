#ifndef ADVENT_OF_CODE_INFRA_FILE_UTIL_H
#define ADVENT_OF_CODE_INFRA_FILE_UTIL_H

#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace advent_of_code {

absl::StatusOr<std::string> GetContents(std::string_view filename);

absl::Status HandleTestIncludes(
    std::string* test_case, std::vector<std::string>* includes_out = nullptr);

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INFRA_FILE_UTIL_H