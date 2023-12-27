#include "advent_of_code/2016/day04/day04.h"

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

bool Validate(std::string_view room, std::string_view sum) {
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

std::string Decrypt(std::string_view name, int sector) {
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

absl::StatusOr<std::string> Day_2016_04::Part1(
    absl::Span<std::string_view> input) const {
  int sector_sum = 0;
  for (std::string_view in : input) {
    std::string_view room;
    int sector;
    std::string_view sum;
    if (!RE2::FullMatch(in, "([a-z\\-]+)-(\\d+)\\[(.....)\\]", &room, &sector,
                        &sum)) {
      return Error("Bad input: ", in);
    }
    if (Validate(room, sum)) {
      sector_sum += sector;
    }
  }
  return AdventReturn(sector_sum);
}

absl::StatusOr<std::string> Day_2016_04::Part2(
    absl::Span<std::string_view> input) const {
  std::string ret;
  for (std::string_view in : input) {
    std::string_view room;
    int sector;
    std::string_view sum;
    if (!RE2::FullMatch(in, "([a-z\\-]+)-(\\d+)\\[(.....)\\]", &room, &sector,
                        &sum)) {
      return Error("Bad input: ", in);
    }
    if (!Validate(room, sum)) continue;
    std::string decryped_room = Decrypt(room, sector);
    if (input.size() < 10 || RE2::PartialMatch(decryped_room, "northpole")) {
      if (!ret.empty()) return Error("Mutiple matching rooms");
      ret = absl::StrCat(sector, ": ", decryped_room);
    }
  }
  return ret;
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/4, []() {
  return std::unique_ptr<AdventDay>(new Day_2016_04());
});

}  // namespace advent_of_code
