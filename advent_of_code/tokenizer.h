#ifndef ADVENT_OF_CODE_TOKENIZER_H
#define ADVENT_OF_CODE_TOKENIZER_H

#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "vlog.h"

namespace advent_of_code {

class Tokenizer {
 public:
  explicit Tokenizer(std::string_view input) : in_(input) {}

  // Returns true if the original input has been completely consumed.
  bool Done() const { return in_.empty(); }

  absl::Status AssertDone() const {
    if (!in_.empty()) return absl::InvalidArgumentError("Not done");
    return absl::OkStatus();
  }

  // Returns (and consumes from input) the next token.
  std::string_view Next();

  // Consumes the next token and returns an error if it != `expect`.
  absl::Status NextIs(std::string_view expect);

  absl::Status NextAre(std::initializer_list<std::string_view> c) {
    for (const auto& s : c) RETURN_IF_ERROR(NextIs(s));
    return absl::OkStatus();
  }

  // Consumes the next token and converts it to an int. Returns an error if
  // parsing the token as an int fails.
  absl::StatusOr<int64_t> NextInt();

 private:
  std::string_view in_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_TOKENIZER_H
