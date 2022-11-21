// http://adventofcode.com/2016/day/16

#include "advent_of_code/2016/day16/day16.h"

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

class DragonCurve {
 public:
  static std::string Expand(absl::string_view tmp) {
    std::string copy = std::string(tmp);
    std::reverse(copy.begin(), copy.end());
    for (int i = 0; i < copy.size(); ++i) {
      copy[i] = copy[i] == '0' ? '1' : '0';
    }
    std::string ret = absl::StrCat(tmp, "0", copy);
    VLOG(2) << ret;
    return ret;
  }

  static std::string Checksum(absl::string_view tmp) {
    std::string ret;
    ret.resize((tmp.size() + 1) / 2);
    for (int i = 0; i < tmp.size(); i += 2) {
      if (tmp[i] == tmp[i + 1]) {
        ret[i / 2] = '1';
      } else {
        ret[i / 2] = '0';
      }
    }
    VLOG(2) << ret;
    return ret;
  }

  explicit DragonCurve(absl::string_view str) : str_(str) {}

  char CharAt(int p) {
    // TODO(@monkeynova) We could be more efficient by calculating the needed
    // char on demand for it's position and doing the checksum in slices
    // of size = 2 ** N.
    return '\0';
    // size
    // 2 * size + 1
    // 4 * size + 3
    // 8 * size + 7
    // ...
    // 2 ** N * size + (2 ** N - 1)
    // 2 ** (N+1) * size + (2 ** (N+1) - 1)
  }

 private:
  absl::string_view str_;
};

std::string RunPart1(absl::string_view input, int size) {
  std::string s = std::string(input);
  VLOG(1) << "Start: " << s;
  while (s.size() < size) {
    s = DragonCurve::Expand(s);
  }
  VLOG(1) << "Expanded: " << s;
  s = s.substr(0, size);
  VLOG(1) << "Truncated:" << s;
  while (s.size() % 2 == 0) {
    s = DragonCurve::Checksum(s);
  }
  VLOG(1) << "Checksum: " << s;
  return s;
}

}  // namespace

absl::StatusOr<std::string> Day_2016_16::Part1(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    if (RunPart1("10000", 20) != "01100") return Error("Bad code");
  }
  if (input.size() != 1) return Error("Bad input size");
  return RunPart1(input[0], 272);
}

absl::StatusOr<std::string> Day_2016_16::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  return RunPart1(input[0], 35651584);
}

}  // namespace advent_of_code
