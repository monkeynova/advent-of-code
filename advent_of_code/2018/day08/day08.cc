#include "advent_of_code/2018/day08/day08.h"

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

absl::StatusOr<int> SumMetadata(absl::Span<int64_t>& range) {
  if (range.size() < 2) return Error("Bad range size");
  int num_children = range[0];
  int num_metadata = range[1];
  range = range.subspan(2);
  int metadata = 0;
  for (int i = 0; i < num_children; ++i) {
    ASSIGN_OR_RETURN(int sub_metadata, SumMetadata(range));
    metadata += sub_metadata;
  }
  for (int m : range.subspan(0, num_metadata)) {
    metadata += m;
  }
  range = range.subspan(num_metadata);
  return metadata;
}

absl::StatusOr<int> NodeValue(absl::Span<int64_t>& range) {
  if (range.size() < 2) return Error("Bad range size");
  int num_children = range[0];
  int num_metadata = range[1];
  range = range.subspan(2);
  std::vector<int> sub_values(num_children, 0);
  for (int i = 0; i < num_children; ++i) {
    ASSIGN_OR_RETURN(int this_sub_value, NodeValue(range));
    sub_values[i] = this_sub_value;
  }
  int value = 0;
  if (num_children == 0) {
    for (int m : range.subspan(0, num_metadata)) {
      value += m;
    }
  } else {
    for (int m : range.subspan(0, num_metadata)) {
      if (m < 0) return Error("Negative metadata");
      if (m == 0) continue;
      if (m > sub_values.size()) continue;
      value += sub_values[m - 1];
    }
  }
  range = range.subspan(num_metadata);
  return value;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_08::Part1(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<std::string_view> int_strs = absl::StrSplit(input[0], " ");
  ASSIGN_OR_RETURN(std::vector<int64_t> ints, ParseAsInts(int_strs));
  absl::Span<int64_t> int_span = absl::MakeSpan(ints);
  ASSIGN_OR_RETURN(int ret, SumMetadata(int_span));
  if (!int_span.empty()) return Error("Bad parse");
  return AdventReturn(ret);
}

absl::StatusOr<std::string> Day_2018_08::Part2(
    absl::Span<std::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<std::string_view> int_strs = absl::StrSplit(input[0], " ");
  ASSIGN_OR_RETURN(std::vector<int64_t> ints, ParseAsInts(int_strs));
  absl::Span<int64_t> int_span = absl::MakeSpan(ints);
  ASSIGN_OR_RETURN(int ret, NodeValue(int_span));
  if (!int_span.empty()) return Error("Bad parse");
  return AdventReturn(ret);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/8, []() {
  return std::unique_ptr<AdventDay>(new Day_2018_08());
});

}  // namespace advent_of_code
