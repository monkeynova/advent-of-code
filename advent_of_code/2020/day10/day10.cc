#include "advent_of_code/2020/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

absl::StatusOr<std::vector<std::string>> Day10_2020::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> jolts = ParseAsInts(input);
  if (!jolts.ok()) return jolts.status();
  std::sort(jolts->begin(), jolts->end());
  int cur_j = 0;
  absl::flat_hash_map<int, int> deltas;
  for (int j : *jolts) {
    if (j - cur_j > 3) return absl::InvalidArgumentError("Can't chain");
    ++deltas[j - cur_j];
    cur_j = j;
  }
  // Final adapter.
  ++deltas[3];
  return IntReturn(deltas[1] * deltas[3]);
}

absl::StatusOr<int64_t> CountValidArrangements(
    const std::vector<int64_t>& jolts,
    absl::flat_hash_map<std::pair<int, int>, int64_t>* memo, int cur_val,
    int offset) {
  VLOG(2) << cur_val << "; " << offset;
  if (auto it = memo->find(std::make_pair(cur_val, offset));
      it != memo->end()) {
    return it->second;
  }
  if (offset == jolts.size()) return 1;
  VLOG(2) << "Calculating";
  int64_t count = 0;
  for (int delta = offset; delta < jolts.size(); ++delta) {
    if (jolts[delta] > cur_val + 3) break;
    absl::StatusOr<int64_t> next =
        CountValidArrangements(jolts, memo, jolts[delta], delta + 1);
    if (!next.ok()) return next.status();
    count += *next;
  }

  VLOG(2) << "  = " << count;
  memo->emplace(std::make_pair(cur_val, offset), count);
  return count;
}

absl::StatusOr<std::vector<std::string>> Day10_2020::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<std::vector<int64_t>> jolts = ParseAsInts(input);
  if (!jolts.ok()) return jolts.status();
  std::sort(jolts->begin(), jolts->end());

  absl::flat_hash_map<std::pair<int, int>, int64_t> memo;
  // TODO(@monkeynova): Maybe handle if max is not unique.
  return IntReturn(CountValidArrangements(*jolts, &memo, 0, 0));
}
