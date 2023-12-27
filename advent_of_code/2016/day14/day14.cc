#include "advent_of_code/2016/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/md5.h"
#include "re2/re2.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2016_14::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");

  std::vector<std::string> window;
  int found = 0;
  for (int index = 0; true; ++index) {
    MD5 digest;
    std::string str = absl::StrCat(input[0], index);
    window.push_back(std::string(digest.DigestHex(str)));
    if (window.size() < 1001) continue;

    if (window.size() != index + 1) return Error("Bad append");

    int test_index = index - 1000;
    absl::flat_hash_set<char> hunt_set;
    {
      std::string_view test = window[test_index];
      for (int i = 0; i + 2 < test.size(); ++i) {
        if (test[i] == test[i + 1] && test[i] == test[i + 2]) {
          VLOG(2) << "Found 3-ple of " << test.substr(i, 1) << " in " << test
                  << " @" << test_index << "," << i;
          hunt_set.insert(test[i]);
          break;
        }
      }
    }
    for (char hunt : hunt_set) {
      bool found_5ple = false;
      for (int j = 1; j <= 1000; ++j) {
        std::string_view test = window[test_index + j];
        for (int i = 0; i + 4 < test.size(); ++i) {
          if (test[i] == hunt && test[i] == test[i + 1] &&
              test[i] == test[i + 2] && test[i] == test[i + 3] &&
              test[i] == test[i + 4]) {
            VLOG(2) << "Found 5-ple of " << test.substr(i, 1) << " in " << test
                    << " @" << test_index + j << "," << i;
            found_5ple = true;
          }
        }
      }
      if (found_5ple) {
        ++found;
        VLOG(1) << "Found Key #" << found << " @" << test_index;
        if (found == 64) return AdventReturn(test_index);
        break;
      }
    }
  }

  return Error("Impossible");
}

absl::StatusOr<std::string> Day_2016_14::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");

  std::vector<std::string> window;
  int found = 0;
  absl::flat_hash_map<char, int> has_5ple;
  for (int i = '0'; i <= '9'; ++i) has_5ple[i] = -1;
  for (int i = 'a'; i <= 'f'; ++i) has_5ple[i] = -1;
  MD5 digest;
  for (int index = 0; true; ++index) {
    std::string str(digest.DigestHex(absl::StrCat(input[0], index)));
    for (int i = 0; i < 2016; ++i) {
      memcpy(str.data(), digest.DigestHex(str).data(), 32);
    }

    for (int i = 0; i + 4 < str.size(); ++i) {
      if (str[i] == str[i + 1] && str[i] == str[i + 2] &&
          str[i] == str[i + 3] && str[i] == str[i + 4]) {
        VLOG(2) << "Found 5-ple of " << str.substr(i, 1) << " in " << str
                << " @" << index << "," << i;
        has_5ple[str[i]] = index;
      }
    }

    window.push_back(str);
    if (window.size() < 1001) continue;

    if (window.size() != index + 1) return Error("Bad append");

    int test_index = index - 1000;
    char hunt = '\0';
    {
      std::string_view test = window[test_index];
      for (int i = 0; i + 2 < test.size(); ++i) {
        if (test[i] == test[i + 1] && test[i] == test[i + 2]) {
          VLOG(2) << "Found 3-ple of " << test.substr(i, 1) << " in " << test
                  << " @" << test_index << "," << i;
          hunt = test[i];
          break;
        }
      }
    }
    if (has_5ple[hunt] > test_index) {
      ++found;
      VLOG(1) << "Found Key #" << found << " @" << test_index;
      if (found == 64) return AdventReturn(test_index);
    }
  }

  return Error("Impossible");
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/14, []() {
  return std::unique_ptr<AdventDay>(new Day_2016_14());
});

}  // namespace advent_of_code
