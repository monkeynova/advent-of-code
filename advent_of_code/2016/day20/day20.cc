// http://adventofcode.com/2016/day/20

#include "advent_of_code/2016/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Block {
  int64_t start;
  int64_t end;
  bool operator<(const Block& o) const {
    if (start != o.start) return start < o.start;
    return end < o.end;
  }
};

}  // namespace

absl::StatusOr<std::string> Day_2016_20::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Block> blocks;
  for (absl::string_view s : input) {
    Block b;
    if (!RE2::FullMatch(s, "(\\d+)-(\\d+)", &b.start, &b.end)) {
      return Error("Bad range: ", s);
    }
    blocks.push_back(b);
  }
  std::sort(blocks.begin(), blocks.end());
  if (blocks[0].start != 0) return IntReturn(0);
  int invalid_through = blocks[0].end;
  for (Block b : blocks) {
    if (b.start <= invalid_through + 1) {
      if (b.end > invalid_through) invalid_through = b.end;
    } else {
      return IntReturn(invalid_through + 1);
    }
  }

  return Error("Not found");
}

absl::StatusOr<std::string> Day_2016_20::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Block> blocks;
  for (absl::string_view s : input) {
    Block b;
    if (!RE2::FullMatch(s, "(\\d+)-(\\d+)", &b.start, &b.end)) {
      return Error("Bad range: ", s);
    }
    blocks.push_back(b);
  }
  std::sort(blocks.begin(), blocks.end());
  if (blocks[0].start != 0) return IntReturn(0);
  int64_t invalid_through = blocks[0].end;
  int64_t allowed = 0;
  for (Block b : blocks) {
    if (b.start <= invalid_through + 1) {
      if (b.end > invalid_through) invalid_through = b.end;
    } else {
      allowed += b.start - invalid_through - 1;
      invalid_through = b.end;
    }
  }
  if (invalid_through < std::numeric_limits<uint32_t>::max()) {
    allowed += std::numeric_limits<uint32_t>::max() - invalid_through - 1;
  }

  return IntReturn(allowed);
}

}  // namespace advent_of_code
