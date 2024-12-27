// http://adventofcode.com/2024/day/9

#include "advent_of_code/2024/day09/day09.h"

#include "absl/log/log.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2024_09::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("1 line?");
  std::string blocks(input[0]);
  if (blocks.size() % 2 == 0) {
    blocks.resize(blocks.size() - 1);
  }
  for (char &c : blocks) c -= '0';
  int64_t check_sum = 0;
  int64_t out_pos = 0;
  for (int i = 0; i < blocks.size(); i += 2) {
    int64_t id = i / 2;
    for (int j = 0; j < blocks[i]; ++j) {
      check_sum += id * out_pos;
      ++out_pos;
    }
    if (i + 1 == blocks.size()) break;
    while (blocks[i + 1] > 0) {
      int take = std::min(blocks[i + 1], blocks[blocks.size() - 1]);
      int64_t take_id = (blocks.size() - 1) / 2;
      for (int j = 0; j < take; ++j) {
        check_sum += take_id * out_pos;
        ++out_pos;
      }
      blocks[i + 1] -= take;
      blocks[blocks.size() - 1] -= take;
      if (blocks[blocks.size() - 1] == 0) {
        blocks.resize(blocks.size() - 2);
      }
    }
  }

  return AdventReturn(check_sum);
}

absl::StatusOr<std::string> Day_2024_09::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("1 line?");
  std::string_view blocks = input[0];
  struct Block {
    int64_t id;
    int64_t start;
    int64_t size;

    int64_t CheckSum() const {
      int64_t check_sum = 0;
      for (int i = 0; i < size; ++i) {
        check_sum += id * (start + i);
      }
      return check_sum;
    }
  };
  std::vector<Block> alloc;
  std::deque<Block> free;

  int offset = 0;
  for (int i = 0; i < blocks.size(); i += 2) {
    {
      int size = blocks[i] - '0';
      alloc.push_back({.id = i / 2, .start = offset, .size = size});
      offset += size;
    }
    if (i + 1 == blocks.size()) break;
    {
      int size = blocks[i + 1] - '0';
      free.push_back({.id = 0, .start = offset, .size = size});
      offset += size;
    }
  }

  int64_t check_sum = 0;
  for (/*nop*/; !alloc.empty(); alloc.pop_back()) {
    while (!free.empty() && free.back().start > alloc.back().start) {
      free.pop_back();
    }
    bool found = false;
    for (Block& b : free) {
      if (b.size < alloc.back().size) continue;
      alloc.back().start = b.start;
      check_sum += alloc.back().CheckSum();
      found = true;

      b.size -= alloc.back().size;
      b.start += alloc.back().size;
      break;
    }
    while (!free.empty() && free.front().size == 0) {
      free.pop_front();
    }
    if (!found) {
      check_sum += alloc.back().CheckSum();
    }
  }

  return AdventReturn(check_sum);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/9,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_09()); });

}  // namespace advent_of_code

