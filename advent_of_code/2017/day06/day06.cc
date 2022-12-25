#include "advent_of_code/2017/day06/day06.h"

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

std::vector<int64_t> RunStep(std::vector<int64_t> in) {
  int64_t max_idx;
  int64_t max_value = std::numeric_limits<int64_t>::min();
  for (int i = 0; i < in.size(); ++i) {
    if (in[i] > max_value) {
      max_value = in[i];
      max_idx = i;
    }
  }
  int64_t q = in[max_idx];
  in[max_idx] = 0;
  int64_t per = q / in.size();
  int64_t rem = q % in.size();
  for (int i = 0; i < in.size(); ++i) {
    in[i] += per;
  }
  for (int j = 0; j < rem; ++j) {
    ++in[(max_idx + j + 1) % in.size()];
  }
  return in;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_06::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> split = absl::StrSplit(input[0], "\t");
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(split);
  if (!list.ok()) return list.status();
  std::vector<int64_t> tmp = *list;
  absl::flat_hash_set<std::vector<int64_t>> hist;
  while (!hist.contains(tmp)) {
    VLOG(2) << absl::StrJoin(tmp, ",");
    hist.insert(tmp);
    tmp = RunStep(std::move(tmp));
  }
  return AdventReturn(hist.size());
}

absl::StatusOr<std::string> Day_2017_06::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> split = absl::StrSplit(input[0], "\t");
  absl::StatusOr<std::vector<int64_t>> list = ParseAsInts(split);
  if (!list.ok()) return list.status();
  std::vector<int64_t> tmp = *list;
  absl::flat_hash_map<std::vector<int64_t>, int> hist;
  for (int i = 0; !hist.contains(tmp); ++i) {
    VLOG(2) << absl::StrJoin(tmp, ",");
    hist[tmp] = i;
    tmp = RunStep(std::move(tmp));
  }
  return AdventReturn(hist.size() - hist[tmp]);
}

}  // namespace advent_of_code
