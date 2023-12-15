// http://adventofcode.com/2023/day/15

#include "advent_of_code/2023/day15/day15.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

int Hash(std::string_view str) {
  int hash = 0;
  for (char c : str) {
    hash = ((hash + c) * 17) % 256;
  }
  return hash;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_15::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int total = 0;
  for (std::string_view piece : absl::StrSplit(input[0], ",")) {
    total += Hash(piece);
  }
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_15::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  struct Entry {
    std::string_view label;
    int value;
  };
  std::array<std::vector<Entry>, 256> hash_table;
  for (std::string_view piece : absl::StrSplit(input[0], ",")) {
    if (piece.back() == '-') {
      std::string_view to_remove = piece.substr(0, piece.size() - 1);
      int hash = Hash(to_remove);
      for (int i = 0; i < hash_table[hash].size(); ++i) {
        if (hash_table[hash][i].label == to_remove) {
          hash_table[hash].erase(hash_table[hash].begin() + i);
          break;
        }
      }
    } else {
      auto [to_set, val_str] = PairSplit(piece, "=");
      int val;
      if (!absl::SimpleAtoi(val_str, &val)) return Error("Bad int");
      int hash = Hash(to_set);
      bool found = false;
      for (int i = 0; i < hash_table[hash].size(); ++i) {
        if (hash_table[hash][i].label == to_set) {
          hash_table[hash][i].value = val;
          found = true;
          break;
        }
      }
      if (!found) {
        hash_table[hash].push_back({.label = to_set, .value = val});
      }
    }
  }
  int total = 0;
  for (int box_num = 0; box_num < 256; ++box_num) {
    for (int i = 0; i < hash_table[box_num].size(); ++i) {
      total += (box_num + 1) * (i + 1) * hash_table[box_num][i].value;
    }
  }
  return AdventReturn(total);
}

}  // namespace advent_of_code
