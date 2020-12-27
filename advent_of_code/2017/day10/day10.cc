#include "advent_of_code/2017/day10/day10.h"

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

void RunLoop(const std::vector<int64_t>& lengths, std::vector<int>* loop) {
  int position = 0;
  int skip_size = 0;
  const int loop_size = loop->size();
  absl::Span<int> loop_span = absl::MakeSpan(*loop);
  for (int i = 0; i < lengths.size(); ++i) {
    int len = lengths[i];
    VLOG(1) << "len=" << len << "; loop="
            << absl::StrJoin(loop_span.subspan(0, position), ",")
            << (position > 0 ? ",(" : "(") << loop->at(position)
            << (position < loop_span.size() - 1 ? ")," : ")")
            << absl::StrJoin(loop_span.subspan(position + 1), ",");
    if (len > 0) {
      int first = position;
      int last = (position + len - 1) % loop_size;
      while (first != last) {
        VLOG(2) << "first=" << first << "; last=" << last;
        std::swap((*loop)[first], (*loop)[last]);
        first = (first + 1) % loop_size;
        // Check needed here for even sized loops.
        if (first == last) break;
        last = (loop_size + last - 1) % loop_size;
      }
    }
    
    position = (position + len + skip_size) % loop_size;
    ++skip_size;
  }
    VLOG(1) << "len=<DONE>; loop="
            << absl::StrJoin(loop_span.subspan(0, position), ",")
            << (position > 0 ? ",(" : "(") << loop->at(position)
            << (position < loop_span.size() - 1 ? ")," : ")")
            << absl::StrJoin(loop_span.subspan(position + 1), ",");
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day10_2017::Part1(
    absl::Span<absl::string_view> input) const {
  {
    std::vector<int> loop(5);
    for (int i = 0; i < loop.size(); ++i) loop[i] = i;
    RunLoop({3, 4, 1, 5}, &loop);
    if (loop[0] * loop[1] != 12) {
      return Error("Test case failed");
    }
  }

  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> length_strs = absl::StrSplit(input[0], ",");
  absl::StatusOr<std::vector<int64_t>> lengths = ParseAsInts(length_strs);

  std::vector<int> loop(256);
  for (int i = 0; i < loop.size(); ++i) loop[i] = i;
  RunLoop(*lengths, &loop);

  return IntReturn(loop[0] * loop[1]);
}

absl::StatusOr<std::vector<std::string>> Day10_2017::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
