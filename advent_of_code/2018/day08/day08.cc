// https://adventofcode.com/2018/day/8
//
// --- Day 8: Memory Maneuver ---
// ------------------------------
//
// The sleigh is much easier to pull than you'd expect for something its
// weight. Unfortunately, neither you nor the Elves know which way the
// North Pole is from here.
//
// You check your wrist device for anything that might help. It seems to
// have some kind of navigation system! Activating the navigation system
// produces more bad news: "Failed to start navigation system. Could not
// read software license file."
//
// The navigation system's license file consists of a list of numbers
// (your puzzle input). The numbers define a data structure which, when
// processed, produces some kind of tree that can be used to calculate
// the license number.
//
// The tree is made up of nodes; a single, outermost node forms the
// tree's root, and it contains all other nodes in the tree (or contains
// nodes that contain nodes, and so on).
//
// Specifically, a node consists of:
//
// * A header, which is always exactly two numbers:
//
// * The quantity of child nodes.
//
// * The quantity of metadata entries.
//
// * Zero or more child nodes (as specified in the header).
//
// * One or more metadata entries (as specified in the header).
//
// Each child node is itself a node that has its own header, child nodes,
// and metadata. For example:
//
// 2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2
// A----------------------------------
// B----------- C-----------
// D-----
//
// In this example, each node of the tree is also marked with an
// underline starting with a letter for easier identification. In it,
// there are four nodes:
//
// * A, which has 2 child nodes (B, C) and 3 metadata entries (1, 1, 2).
//
// * B, which has 0 child nodes and 3 metadata entries (10, 11, 12).
//
// * C, which has 1 child node (D) and 1 metadata entry (2).
//
// * D, which has 0 child nodes and 1 metadata entry (99).
//
// The first check done on the license file is to simply add up all of
// the metadata entries. In this example, that sum is
// 1+1+2+10+11+12+2+99=138.
//
// What is the sum of all metadata entries?
//
// --- Part Two ---
// ----------------
//
// The second check is slightly more complicated: you need to find the
// value of the root node (A in the example above).
//
// The value of a node depends on whether it has child nodes.
//
// If a node has no child nodes, its value is the sum of its metadata
// entries. So, the value of node B is 10+11+12=33, and the value of node
// D is 99.
//
// However, if a node does have child nodes, the metadata entries become
// indexes which refer to those child nodes. A metadata entry of 1 refers
// to the first child node, 2 to the second, 3 to the third, and so on.
// The value of this node is the sum of the values of the child nodes
// referenced by the metadata entries. If a referenced child node does
// not exist, that reference is skipped. A child node can be referenced
// multiple time and counts each time it is referenced. A metadata entry
// of 0 does not refer to any child node.
//
// For example, again using the above nodes:
//
// * Node C has one metadata entry, 2. Because node C has only one
// child node, 2 references a child node which does not exist, and so
// the value of node C is 0.
//
// * Node A has three metadata entries: 1, 1, and 2. The 1 references
// node A's first child node, B, and the 2 references node A's second
// child node, C. Because node B has a value of 33 and node C has a
// value of 0, the value of node A is 33+33+0=66.
//
// So, in this example, the value of the root node is 66.
//
// What is the value of the root node?

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
    absl::StatusOr<int> sub_metadata = SumMetadata(range);
    if (!sub_metadata.ok()) return sub_metadata.status();
    metadata += *sub_metadata;
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
    absl::StatusOr<int> this_sub_value = NodeValue(range);
    if (!this_sub_value.ok()) return this_sub_value.status();
    sub_values[i] = *this_sub_value;
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
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> int_strs = absl::StrSplit(input[0], " ");
  absl::StatusOr<std::vector<int64_t>> ints = ParseAsInts(int_strs);
  if (!ints.ok()) return ints.status();
  absl::Span<int64_t> int_span = absl::MakeSpan(*ints);
  absl::StatusOr<int> ret = SumMetadata(int_span);
  if (!ret.ok()) return ret.status();
  if (!int_span.empty()) return Error("Bad parse");
  return IntReturn(*ret);
}

absl::StatusOr<std::string> Day_2018_08::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Bad size");
  std::vector<absl::string_view> int_strs = absl::StrSplit(input[0], " ");
  absl::StatusOr<std::vector<int64_t>> ints = ParseAsInts(int_strs);
  if (!ints.ok()) return ints.status();
  absl::Span<int64_t> int_span = absl::MakeSpan(*ints);
  absl::StatusOr<int> ret = NodeValue(int_span);
  if (!ret.ok()) return ret.status();
  if (!int_span.empty()) return Error("Bad parse");
  return IntReturn(*ret);
}

}  // namespace advent_of_code
