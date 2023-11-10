#ifndef ADVENT_OF_CODE_2017_KNOT_HASH_H
#define ADVENT_OF_CODE_2017_KNOT_HASH_H

#include <vector>

#include "absl/strings/string_view.h"

namespace advent_of_code {

class KnotHash {
 public:
  std::string DigestHex(std::string_view input);

  void RunLoop(std::string_view lengths, std::vector<unsigned char>* loop,
               int round_count = 1);
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2017_KNOT_HASH_H