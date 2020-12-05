#include "advent_of_code/2020/day5/day5.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day5_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  int maxid = 0;
  for (absl::string_view rec : input) {
    int col = 0;
    if (rec.size() != 10) return absl::InvalidArgumentError("");
    for (int i = 0; i < 7; ++i) {
      if (rec[i] == 'F') {
        col <<= 1;
      } else if (rec[i] == 'B') {
        col = (col << 1) | 1;
      } else {
        return absl::InvalidArgumentError("");
      }
    }
    int row = 0;
    for (int i = 7; i < 10; ++i) {
      if (rec[i] == 'L') {
        row <<= 1;
      } else if (rec[i] == 'R') {
        row = (row << 1) | 1;
      } else {
        return absl::InvalidArgumentError("");
      }
    }
    int id = col * 8 + row;
    maxid = std::max(maxid, id);
  }
  return std::vector<std::string>{absl::StrCat(maxid)};
}

absl::StatusOr<std::vector<std::string>> Day5_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  std::vector<bool> present;
  present.resize(1 << 10);
  for (absl::string_view rec : input) {
    int col = 0;
    if (rec.size() != 10) return absl::InvalidArgumentError("");
    for (int i = 0; i < 7; ++i) {
      if (rec[i] == 'F') {
        col <<= 1;
      } else if (rec[i] == 'B') {
        col = (col << 1) | 1;
      } else {
        return absl::InvalidArgumentError("");
      }
    }
    int row = 0;
    for (int i = 7; i < 10; ++i) {
      if (rec[i] == 'L') {
        row <<= 1;
      } else if (rec[i] == 'R') {
        row = (row << 1) | 1;
      } else {
        return absl::InvalidArgumentError("");
      }
    }
    int id = col * 8 + row;
    present[id] = true;
  }
  int missingid = -1;
  for (int id = 1; id < present.size() - 1; ++id) {
    if (present[id-1] && present[id+1] && !present[id]) {
      missingid = id;
    }
  }
  return std::vector<std::string>{absl::StrCat(missingid)};
}
