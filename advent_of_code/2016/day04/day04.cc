#include "advent_of_code/2016/day04/day04.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

bool Validate(absl::string_view room, absl::string_view sum) {
  absl::flat_hash_map<char, int> counts;
  for (char c : room) {
    if (c != '-') {
      ++counts[c];
    }
  }
  struct Sortable {
    char c;
    int count;
    bool operator<(const Sortable& o) const {
      if (count != o.count) return count > o.count;
      return c < o.c;
    }
  };
  std::vector<Sortable> sorted;
  for (const auto& [c, count] : counts) {
    sorted.push_back({.c = c, .count = count});
  }
  std::sort(sorted.begin(), sorted.end());
  std::string assert;
  assert.resize(5);
  for (int i = 0; i < 5; ++i) {
    assert[i] = sorted[i].c;
  }
  VLOG(2) << sum << " =?= " << assert;
  return sum == assert;
  ;
}

std::string Decrypt(absl::string_view name, int sector) {
  std::string ret = std::string(name);
  for (int i = 0; i < ret.size(); ++i) {
    if (ret[i] == '-') {
      ret[i] = ' ';
    } else {
      ret[i] = (((ret[i] - 'a') + sector) % 26) + 'a';
    }
  }
  return ret;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day04_2016::Part1(
    absl::Span<absl::string_view> input) const {
  int sector_sum = 0;
  for (absl::string_view in : input) {
    absl::string_view room;
    int sector;
    absl::string_view sum;
    if (!RE2::FullMatch(in, "([a-z\\-]+)-(\\d+)\\[(.....)\\]", &room, &sector,
                        &sum)) {
      return Error("Bad input: ", in);
    }
    if (Validate(room, sum)) {
      sector_sum += sector;
    }
  }
  return IntReturn(sector_sum);
}

absl::StatusOr<std::vector<std::string>> Day04_2016::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<std::string> ret;
  for (absl::string_view in : input) {
    absl::string_view room;
    int sector;
    absl::string_view sum;
    if (!RE2::FullMatch(in, "([a-z\\-]+)-(\\d+)\\[(.....)\\]", &room, &sector,
                        &sum)) {
      return Error("Bad input: ", in);
    }
    if (!Validate(room, sum)) continue;
    std::string decryped_room = Decrypt(room, sector);
    if (input.size() < 10 || RE2::PartialMatch(decryped_room, "northpole")) {
      ret.push_back(absl::StrCat(sector, ": ", decryped_room));
    }
  }
  return ret;
}

}  // namespace advent_of_code
