#include "advent_of_code/2020/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {}  // namespace

absl::StatusOr<std::vector<std::string>> Day14_2020::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int64_t, int64_t> mem;
  int64_t mask = 0;
  int64_t mask_val = 0;
  for (absl::string_view str : input) {
    absl::string_view mask_str;
    int64_t addr;
    int64_t val;
    if (RE2::FullMatch(str, "mask = ([10X]+)", &mask_str)) {
      if (mask_str.size() != 36)
        return Error("Bad mask:", mask_str, "; ", mask_str.size());
      mask = 0;
      mask_val = 0;
      int64_t one_bit = 1;
      for (int i = 0; i < mask_str.size(); ++i) {
        switch (mask_str[i]) {
          case '0': {
            mask |= (one_bit << (35 - i));
            break;
          }
          case '1': {
            mask |= (one_bit << (35 - i));
            mask_val |= (one_bit << (35 - i));
            break;
          }
          case 'X':
            break;
          default:
            return Error("Bad mask char: ", mask_str.substr(i, 1), "; ",
                         mask_str);
        }
      }

    } else if (RE2::FullMatch(str, "mem\\[(\\d+)\\] = (\\d+)", &addr, &val)) {
      mem[addr] = (val & ~mask) | mask_val;
    } else {
      return Error("Bad Input: ", str);
    }
  }
  int64_t sum = 0;
  for (const auto& pair : mem) {
    sum += pair.second;
  }
  return IntReturn(sum);
}

absl::StatusOr<std::vector<std::string>> Day14_2020::Part2(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_map<int64_t, int64_t> mem;
  int64_t mask = 0;
  int64_t mask_val = 0;
  std::vector<int64_t> floating;
  for (absl::string_view str : input) {
    absl::string_view mask_str;
    int64_t addr;
    int64_t val;
    if (RE2::FullMatch(str, "mask = ([10X]+)", &mask_str)) {
      if (mask_str.size() != 36)
        return Error("Bad mask:", mask_str, "; ", mask_str.size());
      mask = 0;
      mask_val = 0;
      floating.clear();
      int64_t one_bit = 1;
      for (int i = 0; i < mask_str.size(); ++i) {
        switch (mask_str[i]) {
          case '0': {
            // Leave be...
            break;
          }
          case '1': {
            mask |= (one_bit << (35 - i));
            mask_val |= (one_bit << (35 - i));
            break;
          }
          case 'X': {
            mask |= (one_bit << (35 - i));
            if (floating.empty()) {
              floating.push_back(0);
              floating.push_back(one_bit << (35 - i));
            } else {
              int floating_start_size = floating.size();
              for (int j = 0; j < floating_start_size; ++j) {
                floating.push_back(floating[j] | (one_bit << (35 - i)));
              }
            }
            break;
          }
          default:
            return Error("Bad mask char: ", mask_str.substr(i, 1), "; ",
                         mask_str);
        }
      }
      VLOG(1) << absl::StrCat("mask = 0x", absl::Hex(mask),
                              "; mask_val = ", mask_val);
      VLOG(1) << "floating = " << absl::StrJoin(floating, ",");

    } else if (RE2::FullMatch(str, "mem\\[(\\d+)\\] = (\\d+)", &addr, &val)) {
      int64_t base_addr = (addr & ~mask) | mask_val;
      VLOG(1) << "addr = " << (addr);
      VLOG(1) << "addr & ~mask = " << (addr & ~mask);
      VLOG(1) << "((addr & ~mask) | mask_val) = "
              << ((addr & ~mask) | mask_val);
      VLOG(1) << "base_addr = " << base_addr;
      for (int64_t f : floating) {
        mem[base_addr | f] = val;
        VLOG(2) << "Set [" << (base_addr | f) << "]: " << val;
      }
    } else {
      return Error("Bad Input: ", str);
    }
  }
  int64_t sum = 0;
  for (const auto& pair : mem) {
    sum += pair.second;
  }
  return IntReturn(sum);
}

}  // namespace advent_of_code
