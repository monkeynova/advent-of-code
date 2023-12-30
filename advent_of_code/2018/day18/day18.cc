#include "advent_of_code/2018/day18/day18.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/loop_history.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Forest : public ConwayMultiSet<Point, 3> {
 public:
  Forest(const CharBoard& b)
      : ConwayMultiSet({b.Find('.'), b.Find('|'), b.Find('#')}),
        bounds_(b.range()) {}

  std::string Draw() const override {
    CharBoard ret(bounds_);
    ret.Draw(sets()[0], '.');
    ret.Draw(sets()[1], '|');
    ret.Draw(sets()[2], '#');
    return absl::StrCat(ret);
  }

  int64_t Trees() const { return sets()[1].size(); }
  int64_t Lumber() const { return sets()[2].size(); }

  bool SetsAreComplete() const override { return true; }

  std::vector<Point> Neighbors(const Point& p) const override {
    std::vector<Point> ret;
    for (Point dir : Cardinal::kEightDirs) {
      Point test = p + dir;
      if (bounds_.Contains(test)) {
        ret.push_back(test);
      }
    }
    return ret;
  }
  int NextState(int cur_state, std::array<int, 3> neighbors) const override {
    switch (cur_state) {
      case 0: {
        return neighbors[1] >= 3 ? 1 : 0;
      }
      case 1: {
        return neighbors[2] >= 3 ? 2 : 1;
      }
      case 2: {
        return neighbors[1] == 0 || neighbors[2] == 0 ? 0 : 2;
      }
    }
    LOG(FATAL) << "Bad state: " << cur_state;
  }

  struct Summary {
    std::vector<Point> trees;
    std::vector<Point> lumber;
    bool operator==(const Summary& o) const {
      return trees == o.trees && lumber == o.lumber;
    }
    template <typename H>
    friend H AbslHashValue(H h, const Summary& s) {
      return H::combine(std::move(h), s.trees, s.lumber);
    }
  };

  Summary BuildSummary() {
    return {
        .trees = StablePointSet(sets()[1]),
        .lumber = StablePointSet(sets()[2]),
    };
  }

 private:
  PointRectangle bounds_;
};

}  // namespace

absl::StatusOr<std::string> Day_2018_18::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard in, CharBoard::Parse(input));

  Forest f(in);
  f.AdvanceN(10);
  return AdventReturn(f.Trees() * f.Lumber());
}

absl::StatusOr<std::string> Day_2018_18::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard in, CharBoard::Parse(input));
  constexpr int kNumSteps = 1'000'000'000;

  Forest f(in);
  LoopHistory<Forest::Summary> hist;
  for (int i = 0; i < kNumSteps; ++i) {
    if (hist.AddMaybeNew(f.BuildSummary())) {
      Forest::Summary summary = hist.FindInLoop(kNumSteps);
      return AdventReturn(summary.trees.size() * summary.lumber.size());
    }
    f.Advance();
  }
  return AdventReturn(f.Trees() * f.Lumber());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2018, /*day=*/18,
    []() { return std::unique_ptr<AdventDay>(new Day_2018_18()); });

}  // namespace advent_of_code
