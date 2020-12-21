#include "advent_of_code/2016/day09/day09.h"

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

absl::StatusOr<std::string> DecompressV1(absl::string_view in) {
  std::string ret;
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] == '(') {
      int count;
      int len;
      absl::string_view skip;
      if (!RE2::FullMatch(in.substr(i), "(\\((\\d+)x(\\d+)\\)).*", &skip, &len, &count)) {
        return AdventDay::Error("Bad parse: ", in.substr(i));
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

absl::StatusOr<std::string> DecompressV2(absl::string_view in) {
  std::string ret;
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] == '(') {
      int count;
      int len;
      absl::string_view skip;
      if (!RE2::FullMatch(in.substr(i), "(\\((\\d+)x(\\d+)\\)).*", &skip, &len, &count)) {
        return AdventDay::Error("Bad parse: ", in.substr(i));
      }
      absl::StatusOr<std::string> tmp = DecompressV2(in.substr(i + skip.size(), len));
      if (!tmp.ok()) return tmp.status();
      for (int j = 0; j < count; ++j) {
        ret.append(*tmp);
      }
      i += skip.size() + len - 1;
    } else {
      ret.append(std::string(in.substr(i, 1)));
    }
  }
  return ret;
}

absl::StatusOr<int64_t> DecompressV2NonWhitespaceLen(absl::string_view in) {
  int64_t ret = 0;
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] == '(') {
      int64_t count;
      int64_t len;
      absl::string_view skip;
      if (!RE2::FullMatch(in.substr(i), "(\\((\\d+)x(\\d+)\\)).*", &skip, &len, &count)) {
        return AdventDay::Error("Bad parse: ", in.substr(i));
      }
      absl::StatusOr<int64_t> sub_len = DecompressV2NonWhitespaceLen(in.substr(i + skip.size(), len));
      if (!sub_len.ok()) return sub_len.status();
      ret += count * *sub_len;
      i += skip.size() + len - 1;
    } else {
      ret += in[i] == ' ' ? 0 : 1;
    }
  }
  return ret;
}

int NonWhitespaceLen(absl::string_view s) {
  int len = s.size();
  for (char s : s) if (s == ' ') --len;
  return len;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day09_2016::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  absl::StatusOr<std::string> dec = DecompressV1(input[0]);
  if (!dec.ok()) return dec.status();
  VLOG_IF(1, dec->size() < 100) << *dec;
  return IntReturn(NonWhitespaceLen(*dec));
}

absl::StatusOr<std::vector<std::string>> Day09_2016::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input");
  return IntReturn(DecompressV2NonWhitespaceLen(input[0]));
}

}  // namespace advent_of_code
