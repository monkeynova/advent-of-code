#include "advent_of_code/tokenizer.h"

#include "absl/strings/str_cat.h"

namespace advent_of_code {

absl::Status Tokenizer::NextIs(std::string_view expect) {
  std::string_view next = Next();
  if (next == expect) return absl::OkStatus();
  return absl::InvalidArgumentError(
      absl::StrCat("Unexpected token: '", next, "' != '", expect,
                   "'; remaining is '", in_, "'"));
}

absl::StatusOr<int64_t> Tokenizer::NextInt() {
  std::string_view next = Next();
  int64_t ret;
  if (absl::SimpleAtoi(next, &ret)) return ret;
  return absl::InvalidArgumentError(
      absl::StrCat("Not an int: '", next, "'; remaining is '", in_, "'"));
}

std::string_view Tokenizer::Next() {
  int start = 0;
  while (start < in_.size() && isspace(in_[start])) {
    ++start;
  }
  if (start == in_.size()) {
    in_ = "";
    return "";
  }
  int end = start + 1;
  if (in_[start] == '-' || isdigit(in_[start])) {
    while (end < in_.size() && isdigit(in_[end])) {
      ++end;
    }
  } else if (isalpha(in_[start])) {
    while (end < in_.size()) {
      if (!isalpha(in_[end]) && !isdigit(in_[end])) break;
      ++end;
    }
  }
  std::string_view ret = in_.substr(start, end - start);
  in_ = in_.substr(end);
  return ret;
}

}  // namespace advent_of_code
