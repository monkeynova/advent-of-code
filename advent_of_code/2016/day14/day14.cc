#include "advent_of_code/2016/day14/day14.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"
#include "third_party/md5/md5.h"

namespace advent_of_code {

namespace {

std::string Hex(absl::string_view buf) {
  static char hexchar[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  return absl::StrJoin(buf, "", [](std::string* out, char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    char tmp[] = {hexchar[uc >> 4], hexchar[uc & 0xf], '\0'};
    absl::StrAppend(out, tmp);
  });
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day14_2016::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");

  MD5_CTX ctx;
  unsigned char md5_result[16];
  absl::string_view md5_result_view(reinterpret_cast<char*>(md5_result), 16);
  std::vector<std::string> window;
  int found = 0;
  for (int index = 0; true; ++index) {
    MD5_Init(&ctx);
    std::string str = absl::StrCat(input[0], index);
    MD5_Update(&ctx, str.data(), str.size());
    MD5_Final(md5_result, &ctx);
    
    std::string hex = Hex(md5_result_view);
    window.push_back(hex);
    if (window.size() < 1001) continue;

    if (window.size() != index + 1) return Error("Bad append");

    int test_index = index - 1000;
    absl::flat_hash_set<char> hunt_set;
    {
      absl::string_view test = window[test_index];
      for (int i = 0; i + 2 < test.size(); ++i) {
        if (test[i] == test[i+1] &&
            test[i] == test[i+2]) {
          VLOG(2) << "Found 3-ple of " << test.substr(i, 1) << " in " << test << " @" << test_index << "," << i;
          hunt_set.insert(test[i]);
          break;
        }
      }
    }  
    for (char hunt : hunt_set) {
      bool found_5ple = false;
      for (int j = 1; j <= 1000; ++j) {
        absl::string_view test = window[test_index + j];
        for (int i = 0; i + 4 < test.size(); ++i) {
          if (test[i] == hunt &&
              test[i] == test[i+1] &&
              test[i] == test[i+2] &&
              test[i] == test[i+3] &&
              test[i] == test[i+4]) {
            VLOG(2) << "Found 5-ple of " << test.substr(i, 1) << " in " << test << " @" << test_index + j << "," << i;
            found_5ple = true;
          }
        }
      }
      if (found_5ple) {
        ++found;
        VLOG(1) << "Found Key #" << found << " @" << test_index;
        if (found == 64) return IntReturn(test_index);
        break;
      }
    }
  }

  return Error("Impossible");
}

absl::StatusOr<std::vector<std::string>> Day14_2016::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
