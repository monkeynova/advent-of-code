#include "advent_of_code/2018/day18/day18.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/node_hash_map.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// TODO(@monkeynova): Use a shared Conway interface?

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

template <typename Storage>
class LoopHistory {
 public:
  bool AddMaybeNew(Storage s) {
    auto [it, inserted] = hist_.emplace(std::move(s), idx_.size());
    if (inserted) {
      idx_.push_back(&it->first);
      return false;
    }
    loop_size_ = idx_.size() - it->second;
    loop_offset_ = it->second;
    return true;
  }

  const Storage& FindInLoop(int64_t offset) {
    int64_t loop_idx_ = (offset - loop_offset_) % loop_size_ + loop_offset_;
    return *idx_[loop_idx_];
  }

 private:
  absl::node_hash_map<Storage, int> hist_;
  std::vector<const Storage*> idx_;
  int loop_size_ = -1;
  int loop_offset_ = -1;
};

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
  LoopHistory<CharBoard> hist;
  for (int i = 0; i < kNumSteps; ++i) {
    if (hist.AddMaybeNew(step)) {
      step = hist.FindInLoop(kNumSteps);
      break;
    }
    VLOG(1) << "Step [" << i << "]:\n" << step;
    step = Update(step);
  }
  int trees = step.CountChar('|');
  int lumber = step.CountChar('#');
  return AdventReturn(trees * lumber);
}

}  // namespace advent_of_code
