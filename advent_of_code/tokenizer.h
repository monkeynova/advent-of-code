#ifndef ADVENT_OF_CODE_TOKENIZER_H
#define ADVENT_OF_CODE_TOKENIZER_H

#include <string_view>

namespace advent_of_code {

class Tokenizer {
 public:
  explicit Tokenizer(std::string_view input) : in_(input) {}

  bool Done() { return in_.empty(); }

  std::string_view Next();

 private:
  std::string_view in_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_TOKENIZER_H
