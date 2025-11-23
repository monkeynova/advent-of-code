// http://adventofcode.com/2024/day/16

#include "advent_of_code/2024/day16/day16.h"

#include "absl/log/log.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/fast_board.h"

namespace advent_of_code {

namespace {

class ReindeerPath : public BFSInterface<ReindeerPath> {
 public:
  ReindeerPath(const FastBoard& b, FastBoard::Point start)
      : b_(&b), cur_(start) {}

  bool operator==(const ReindeerPath& o) const {
    return cur_ == o.cur_ && dir_ == o.dir_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const ReindeerPath& rp) {
    return H::combine(std::move(h), rp.cur_, rp.dir_);
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const ReindeerPath& rp) {
    absl::Format(&sink, "%v,%v @%d", rp.cur_, rp.dir_, rp.num_steps());
  }

  const ReindeerPath& identifier() const override { return *this; }

  bool IsFinal() const override { return (*b_)[cur_] == 'E'; }

  void AddNextSteps(State* state) const override {
    FastBoard::Point n = b_->Add(cur_, dir_);
    if (b_->OnBoard(n) && (*b_)[n] != '#') {
      ReindeerPath next = *this;
      next.cur_ = n;
      state->AddNextStep(next);
    }
    {
      ReindeerPath left = *this;
      left.dir_ = FastBoard::kRotateLeft[left.dir_];
      left.add_steps(999);
      state->AddNextStep(left);
    }
    {
      ReindeerPath right = *this;
      right.dir_ = FastBoard::kRotateRight[right.dir_];
      right.add_steps(999);
      state->AddNextStep(right);
    }
  }

 private:
  const FastBoard* b_;
  FastBoard::Point cur_;
  FastBoard::Dir dir_ = FastBoard::kEast;
};

class BestPathHistory {
 public:
  explicit BestPathHistory(const FastBoard& fb) : fb_(fb), chain_(fb, {}) {}

  void MergePaths(FastBoard::PointDir p1, FastBoard::PointDir p2) {
    chain_.GetMutable(p2).push_back(p1);
  }

  void ReplacePaths(FastBoard::PointDir p1, FastBoard::PointDir p2) {
    chain_.Set(p2, {p1});
  }

  std::vector<FastBoard::Point> CollectFrom(FastBoard::Point p) {
    std::vector<FastBoard::Point> ret = {p};
    FastBoard::PointMap<bool> ret_hist(fb_, false);
    ret_hist.Set(p, true);

    std::deque<FastBoard::PointDir> frontier = {
        {p, FastBoard::kNorth},
        {p, FastBoard::kSouth},
        {p, FastBoard::kWest},
        {p, FastBoard::kEast},
    };
    FastBoard::PointDirMap<bool> hist(fb_, false);
    for (const FastBoard::PointDir& pd : frontier) hist.Set(pd, true);
    for (/*nop*/; !frontier.empty(); frontier.pop_front()) {
      for (const FastBoard::PointDir& pd : chain_.Get(frontier.front())) {
        if (hist.Get(pd)) continue;
        hist.Set(pd, true);
        frontier.push_back(pd);
        if (!ret_hist.Get(pd.p)) {
          ret_hist.Set(pd.p, true);
          ret.push_back(pd.p);
        }
      }
    }

    return ret;
  }

 private:
  const FastBoard& fb_;
  FastBoard::PointDirMap<std::vector<FastBoard::PointDir>> chain_;
};

class ReindeerPath2 : public BFSInterface<ReindeerPath2> {
 public:
  struct GlobalState {
    const FastBoard& b;
    std::optional<int> best_steps;
    BestPathHistory best_path_set;
  };

  ReindeerPath2(GlobalState& gs, FastBoard::Point start)
      : gs_(&gs), cur_(start) {}

  bool operator==(const ReindeerPath2& o) const {
    return cur_ == o.cur_ && dir_ == o.dir_;
  }

  template <typename H>
  friend H AbslHashValue(H h, const ReindeerPath2& rp) {
    return H::combine(std::move(h), rp.cur_, rp.dir_);
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const ReindeerPath2& rp) {
    absl::Format(&sink, "%v,%v @%d", rp.cur_, rp.dir_, rp.num_steps());
  }

  const ReindeerPath2& identifier() const override { return *this; }

  bool IsFinal() const override {
    if (gs_->best_steps) return num_steps() > *gs_->best_steps;
    if (!gs_->best_steps) {
      if (gs_->b[cur_] == 'E') {
        gs_->best_steps = num_steps();
      }
    }
    return false;
  }

  FastBoard::PointDir ToPointDir() const { return {.p = cur_, .d = dir_}; }

  void AddNextStepUpdatePaths(State* state, ReindeerPath2 next) const {
    switch (state->AddNextStep(next)) {
      case AddNextStepResult::kAdded: {
        gs_->best_path_set.ReplacePaths(ToPointDir(), next.ToPointDir());
        break;
      }
      case AddNextStepResult::kMerged: {
        gs_->best_path_set.MergePaths(ToPointDir(), next.ToPointDir());
        break;
      }
      case AddNextStepResult::kSkipped:
        break;
      default:
        LOG(FATAL) << "Bad AddNextStep";
    }
  }

  void AddNextSteps(State* state) const override {
    FastBoard::Point test = gs_->b.Add(cur_, dir_);
    if (gs_->b.OnBoard(test) && gs_->b[test] != '#') {
      ReindeerPath2 next = *this;
      next.cur_ = test;
      AddNextStepUpdatePaths(state, next);
    }
    {
      ReindeerPath2 left = *this;
      left.dir_ = FastBoard::kRotateLeft[left.dir_];
      left.add_steps(999);
      AddNextStepUpdatePaths(state, left);
    }
    {
      ReindeerPath2 right = *this;
      right.dir_ = FastBoard::kRotateLeft[right.dir_];
      right.add_steps(999);
      AddNextStepUpdatePaths(state, right);
    }
  }

 private:
  GlobalState* gs_;
  FastBoard::Point cur_;
  FastBoard::Dir dir_ = FastBoard::kEast;
};

}  // namespace

absl::StatusOr<std::string> Day_2024_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  FastBoard fb(b);
  FastBoard::Point start = fb.FindUnique('S');
  return AdventReturn(ReindeerPath(fb, start).FindMinStepsAStar());

  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2024_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  FastBoard fb(b);
  FastBoard::Point start = fb.FindUnique('S');

  // TODO(monkeynova): The BFS A* implementation uses absl hash containers,
  //                   which are noticible in the performance here. If we need
  //                   to squeeze more cycles, hand-rolling a BFS
  //                   implementation with a FastBoard::PointDirMap would help.
  ReindeerPath2::GlobalState gs{
      .b = fb,
      .best_path_set = BestPathHistory(fb),
  };
  ReindeerPath2 rp2(gs, start);
  std::optional<int> dist = rp2.FindMinStepsAStar();
  CHECK(gs.best_steps);
  CHECK(dist);
  CHECK(*dist > *gs.best_steps);

  FastBoard::Point end = fb.FindUnique('E');
  std::vector<FastBoard::Point> final_set = gs.best_path_set.CollectFrom(end);

  if (VLOG_IS_ON(2)) {
    ASSIGN_OR_RETURN(CharBoard draw, CharBoard::Parse(input));
    for (FastBoard::Point p : final_set) draw[fb.To(p)] = 'O';
    VLOG(2) << "With path:\n" << draw;
  }

  return AdventReturn(final_set.size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/16,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_16()); });

}  // namespace advent_of_code
