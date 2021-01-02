#include "advent_of_code/2018/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

CharBoard Update(const CharBoard& in) {
  CharBoard ret = in;
  for (Point p : in.range()) {
    absl::flat_hash_map<char, int> neighbors;
    for (Point dir : Cardinal::kEightDirs) {
      Point test = p + dir;
      if (in.OnBoard(test)) {
        ++neighbors[in[test]];
      }
    }
    switch (in[p]) {
      case '.': {
        if (neighbors['|'] >= 3) {
          ret[p] = '|';
        }
        break;
      }
      case '|': {
        if (neighbors['#'] >= 3) {
          ret[p] = '#';
        }
        break;
      }
      case '#': {
        if (neighbors['#'] < 1 || neighbors['|'] < 1) {
          ret[p] = '.';
        }
        break;
      }
      default: LOG(FATAL) << "Bad board char @" << p << ": " << in[p];
    }
  }
  return ret;
}
// Helper methods go here.

}  // namespace

absl::StatusOr<std::vector<std::string>> Day18_2018::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<CharBoard> in = CharBoard::Parse(input);
  if (!in.ok()) return in.status();
  CharBoard step = *in;
  for (int i = 0; i < 10; ++i) {
    VLOG(1) << "Step [" << i << "]:\n" << step.DebugString();
    step = Update(step);
  }
  int trees = 0;
  int lumber = 0;
  for (Point p : step.range()) {
    if (step[p] == '|') ++trees;
    if (step[p] == '#') ++lumber;
  }
  return IntReturn(trees * lumber);
}

absl::StatusOr<std::vector<std::string>> Day18_2018::Part2(
    absl::Span<absl::string_view> input) const {
  return Error("Not implemented");
}

}  // namespace advent_of_code
