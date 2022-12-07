// https://adventofcode.com/2016/day/20
//
// --- Day 20: Firewall Rules ---
// ------------------------------
//
// You'd like to set up a small hidden computer here so you can use it to
// get back into the network later. However, the corporate firewall only
// allows communication with certain external IP addresses.
//
// You've retrieved the list of blocked IPs from the firewall, but the
// list seems to be messy and poorly maintained, and it's not clear which
// IPs are allowed. Also, rather than being written in dot-decimal
// notation, they are written as plain 32-bit integers, which can have
// any value from 0 through 4294967295, inclusive.
//
// For example, suppose only the values 0 through 9 were valid, and that
// you retrieved the following blacklist:
//
// 5-8
// 0-2
// 4-7
//
// The blacklist specifies ranges of IPs (inclusive of both the start and
// end value) that are not allowed. Then, the only IPs that this firewall
// allows are 3 and 9, since those are the only numbers not in any range.
//
// Given the list of blocked IPs you retrieved from the firewall (your
// puzzle input), what is the lowest-valued IP that is not blocked?
//
// --- Part Two ---
// ----------------
//
// How many IPs are allowed by the blacklist?

#include "advent_of_code/2016/day20/day20.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
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
