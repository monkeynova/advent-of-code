#ifndef ADVENT_OF_CODE_2017_KNOT_HASH_H
#define ADVENT_OF_CODE_2017_KNOT_HASH_H

#include <vector>

#include "absl/strings/string_view.h"

namespace advent_of_code {

void KnotHashRunLoop(
    const std::vector<int64_t>& lengths, std::vector<int>* loop,
    int round_count = 1);

std::string KnotHash(absl::string_view input);

}  // namespace advent_of_code;

#endif  // ADVENT_OF_CODE_2017_KNOT_HASH_H