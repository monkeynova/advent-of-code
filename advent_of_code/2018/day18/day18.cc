#include "advent_of_code/2018/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
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
      default:
        LOG(FATAL) << "Bad board char @" << p << ": " << in[p];
    }
  }
  return ret;
}
// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2018_18::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> in = CharBoard::Parse(input);
  if (!in.ok()) return in.status();
  CharBoard step = *in;
  for (int i = 0; i < 10; ++i) {
    VLOG(1) << "Step [" << i << "]:\n" << step;
    step = Update(step);
  }
  int trees = step.CountChar('|');
  int lumber = step.CountChar('#');
  return AdventReturn(trees * lumber);
}

absl::StatusOr<std::string> Day_2018_18::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<CharBoard> in = CharBoard::Parse(input);
  if (!in.ok()) return in.status();
  CharBoard step = *in;
  constexpr int kNumSteps = 1'000'000'000;
  absl::flat_hash_map<CharBoard, int> hist;
  for (int i = 0; i < kNumSteps; ++i) {
    if (hist.contains(step)) {
      int loop_size = i - hist[step];
      int loop_offset = hist[step];
      LOG(INFO) << "Found loop at " << loop_offset << " of size " << loop_size;
      int final_idx = (kNumSteps - loop_offset) % loop_size + loop_offset;
      bool found = false;
      for (const auto& [board, idx] : hist) {
        if (idx == final_idx) {
          step = board;
          found = true;
          break;
        }
      }
      if (!found) return Error("Could not find loop value!");
      break;
    }
    hist[step] = i;
    VLOG(1) << "Step [" << i << "]:\n" << step;
    step = Update(step);
  }
  int trees = step.CountChar('|');
  int lumber = step.CountChar('#');
  return AdventReturn(trees * lumber);
}

}  // namespace advent_of_code
