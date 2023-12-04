#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

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
  if (in_[end] == '-' || isdigit(in_[end])) {
    ++end;
    while (end < in_.size() && isdigit(in_[end])) {
      ++end;
    }
  } else if (isalpha(in_[end])) {
    while (end < in_.size() && isalpha(in_[end])) {
      ++end;
    }
  }
  std::string_view ret = in_.substr(start, end - start);
  in_ = in_.substr(end);
  return ret;
}

}  // namespace advent_of_code
