// http://adventofcode.com/2023/day/15

#include "advent_of_code/2023/day15/day15.h"

#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

int Hash(std::string_view str) {
  int hash = 0;
  for (char c : str) {
    hash = ((hash + c) * 17) % 256;
  }
  return hash;
}

class HashTable {
 public:
  HashTable() = default;

  int Power() const {
    int total = 0;
    for (int box_num = 0; box_num < 256; ++box_num) {
      for (int i = 0; i < table_[box_num].size(); ++i) {
        total += (box_num + 1) * (i + 1) * table_[box_num][i].val;
      }
    }
    return total;
  }

  void Delete(std::string_view label) {
    int hash = Hash(label);
    for (int i = 0; i < table_[hash].size(); ++i) {
      if (table_[hash][i].label == label) {
        table_[hash].erase(table_[hash].begin() + i);
        break;
      }
    }
  }

  void Set(std::string_view label, int val) {
    int hash = Hash(label);
    bool found = false;
    for (int i = 0; i < table_[hash].size(); ++i) {
      if (table_[hash][i].label == label) {
        table_[hash][i].val = val;
        found = true;
        break;
      }
    }
    if (!found) {
      table_[hash].push_back({.label = label, .val = val});
    }
  }

 private:
  struct Entry {
    std::string_view label;
    int val;
  };

  std::array<std::vector<Entry>, 256> table_;
};

}  // namespace

absl::StatusOr<std::string> Day_2023_15::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  int total = 0;
  int hash = 0;
  for (char c : input[0]) {
    if (c == ',') {
      total += hash;
      hash = 0;
    } else {
      hash += c;
      hash *= 17;
      hash %= 256;
    }
  }
  total += hash;
  return AdventReturn(total);
}

absl::StatusOr<std::string> Day_2023_15::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  HashTable hash_table;
  Tokenizer tok(input[0]);
  while (!tok.Done()) {
    std::string_view label = tok.Next();
    std::string_view op = tok.Next();
    if (op == "=") {
      ASSIGN_OR_RETURN(int val, tok.NextInt());
      hash_table.Set(label, val);
    } else if (op == "-") {
      hash_table.Delete(label);
    } else {
      return Error("Bad op: ", op);
    }
    if (!tok.Done()) {
      RETURN_IF_ERROR(tok.NextIs(","));
    }
  }
  return AdventReturn(hash_table.Power());
}

}  // namespace advent_of_code
