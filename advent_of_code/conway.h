#ifndef ADVENT_OF_CODE_CONWAY_H
#define ADVENT_OF_CODE_CONWAY_H

#include "absl/status/status.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

class Conway {
 public:
  Conway(CharBoard b, std::string lookup, char fill = '.')
   : b_(std::move(b)), lookup_(std::move(lookup)), fill_(fill) {}

  const CharBoard& board() const { return b_; }
  char fill() const { return fill_; }

  absl::Status Advance();
  absl::Status AdvanceN(int count);

 private:
  CharBoard b_;
  std::string lookup_;
  char fill_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CONWAY_H