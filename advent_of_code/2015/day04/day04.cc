#include "advent_of_code/2015/day04/day04.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2015/day04/md5.h"
#include "glog/logging.h"
#include "re2/re2.h"

std::string Hex(absl::string_view buf) {
  static char hexchar[] = {'0', '1', '2', '3','4','5','6','7','8','9','a','b','c','d','e','f'};
  return absl::StrJoin(buf, "", 
    [](std::string* out, char c) {
      unsigned char uc = static_cast<unsigned char>(c);
      char tmp[] = {hexchar[uc >> 4], hexchar[uc & 0xf], '\0'};
      absl::StrAppend(out, tmp);
    });
}

absl::StatusOr<std::vector<std::string>> Day04_2015::Part1(
    const std::vector<absl::string_view>& input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  MD5_CTX ctx;
  unsigned char md5_result[16];
  absl::string_view md5_result_view(reinterpret_cast<char*>(md5_result), 16);
  for (int i = 0;; ++i) {
    VLOG(1) << i;
    MD5_Init(&ctx);
    std::string str = absl::StrCat(input[0], i);
    MD5_Update(&ctx, str.data(), str.size());
    MD5_Final(md5_result, &ctx);
    VLOG(2) << "MD5(" << str << "): " << Hex(md5_result_view);
    if (md5_result[0] == 0 &&
        md5_result[1] == 0 && 
        (md5_result[2] >> 4) == 0) {
      return IntReturn(i);
    }
  }
  return absl::InvalidArgumentError("Unreachable");
}

absl::StatusOr<std::vector<std::string>> Day04_2015::Part2(
    const std::vector<absl::string_view>& input) const {
  if (input.size() != 1) return absl::InvalidArgumentError("Bad input");
  MD5_CTX ctx;
  unsigned char md5_result[16];
  absl::string_view md5_result_view(reinterpret_cast<char*>(md5_result), 16);
  for (int i = 0;; ++i) {
    VLOG(1) << i;
    MD5_Init(&ctx);
    std::string str = absl::StrCat(input[0], i);
    MD5_Update(&ctx, str.data(), str.size());
    MD5_Final(md5_result, &ctx);
    VLOG(2) << "MD5(" << str << "): " << Hex(md5_result_view);
    if (md5_result[0] == 0 &&
        md5_result[1] == 0 && 
        md5_result[2] == 0) {
      return IntReturn(i);
    }
  }
  return absl::InvalidArgumentError("Unreachable");
}
