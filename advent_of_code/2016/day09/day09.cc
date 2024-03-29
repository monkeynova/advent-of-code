#include "advent_of_code/2016/day09/day09.h"

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

absl::StatusOr<std::string> DecompressV1(std::string_view in) {
  std::string ret;
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] == '(') {
      int count;
      int len;
      std::string_view skip;
      if (!RE2::FullMatch(in.substr(i), "(\\((\\d+)x(\\d+)\\)).*", &skip, &len,
                          &count)) {
        return Error("Bad parse: ", in.substr(i));
      }
      for (int j = 0; j < count; ++j) {
        ret.append(std::string(in.substr(i + skip.size(), len)));
      }
      i += skip.size() + len - 1;
    } else {
      ret.append(std::string(in.substr(i, 1)));
    }
  }
  return ret;
}

absl::StatusOr<int64_t> DecompressV2NonWhitespaceLen(std::string_view in) {
  int64_t ret = 0;
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] == '(') {
      int64_t count;
      int64_t len;
      std::string_view skip;
      if (!RE2::FullMatch(in.substr(i), "(\\((\\d+)x(\\d+)\\)).*", &skip, &len,
                          &count)) {
        return Error("Bad parse: ", in.substr(i));
      }
      ASSIGN_OR_RETURN(int64_t sub_len, DecompressV2NonWhitespaceLen(
                                            in.substr(i + skip.size(), len)));
      ret += count * sub_len;
      i += skip.size() + len - 1;
    } else {
      ret += in[i] == ' ' ? 0 : 1;
    }
  }
  return ret;
}

int NonWhitespaceLen(std::string_view s) {
  int len = s.size();
  for (char s : s)
    if (s == ' ') --len;
  return len;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_09::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  ASSIGN_OR_RETURN(std::string dec, DecompressV1(input[0]));
  VLOG_IF(1, dec.size() < 100) << dec;
  return AdventReturn(NonWhitespaceLen(dec));
}

absl::StatusOr<std::string> Day_2016_09::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  return AdventReturn(DecompressV2NonWhitespaceLen(input[0]));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2016, /*day=*/9,
    []() { return std::unique_ptr<AdventDay>(new Day_2016_09()); });

}  // namespace advent_of_code
