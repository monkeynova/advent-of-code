#include "advent_of_code/2020/day10/day10.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

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
    ASSIGN_OR_RETURN(int64_t next, CountValidArrangements(
                                       jolts, memo, jolts[delta], delta + 1));
    count += next;
  }

  VLOG(2) << "  = " << count;
  memo->emplace(std::make_pair(cur_val, offset), count);
  return count;
}

}  // namespace

absl::StatusOr<std::string> Day_2020_10::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> jolts, ParseAsInts(input));
  absl::c_sort(jolts);
  int cur_j = 0;
  absl::flat_hash_map<int, int> deltas;
  for (int j : jolts) {
    if (j - cur_j > 3) return absl::InvalidArgumentError("Can't chain");
    ++deltas[j - cur_j];
    cur_j = j;
  }
  // Final adapter.
  ++deltas[3];
  return AdventReturn(deltas[1] * deltas[3]);
}

absl::StatusOr<std::string> Day_2020_10::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<int64_t> jolts, ParseAsInts(input));
  absl::c_sort(jolts);

  absl::flat_hash_map<std::pair<int, int>, int64_t> memo;
  // TODO(@monkeynova): Maybe handle if max is not unique.
  return AdventReturn(CountValidArrangements(jolts, &memo, 0, 0));
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2020, /*day=*/10, []() {
  return std::unique_ptr<AdventDay>(new Day_2020_10());
});

}  // namespace advent_of_code
