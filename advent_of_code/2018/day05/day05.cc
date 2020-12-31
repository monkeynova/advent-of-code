#include "advent_of_code/2018/day05/day05.h"

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

std::string React(std::string in) {
  std::string ret;
  ret.resize(in.size());
  int out = 0;
  for (int i = 0; i < in.size(); ++i) {
    bool remove = false;
    if (i < in.size() - 1) {
      if (in[i] == in[i + 1] + 'a' - 'A') {
        remove = true;
      }
      if (in[i] == in[i + 1] + 'A' - 'a') {
        remove = true;
      }
    }
    if (remove) {
      ++i;
    } else {
      ret[out++] = in[i];
    }
  }
  ret.resize(out);
  return ret;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day05_2018::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad input size");
  std::string comp = std::string(input[0]);
  while (true) {
    VLOG(1) << comp;
    std::string new_comp = React(comp);
    if (new_comp.size() == comp.size()) return IntReturn(new_comp.size());
    comp = std::move(new_comp);
  }
  return Error("left infinite loop");
}

absl::StatusOr<std::vector<std::string>> Day05_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
