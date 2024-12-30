// http://adventofcode.com/2024/day/16

#include "advent_of_code/2024/day16/day16.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

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

  bool IsFinal() const override {
    return (*b_)[cur_] == 'E';
  }

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
      {p, FastBoard::kNorth}, {p, FastBoard::kSouth},
      {p, FastBoard::kWest}, {p, FastBoard::kEast}, 
    };
    FastBoard::PointDirMap<bool> hist(fb_, false);
    for (const auto& pair : frontier) hist.Set(pair, true);
    for (/*nop*/;!frontier.empty(); frontier.pop_front()) {
      for (const auto& pd : chain_.Get(frontier.front())) {
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
  ReindeerPath2(const FastBoard& b, FastBoard::Point start,
                std::optional<int>* best_steps,
                BestPathHistory* best_path_set)
   : b_(&b), best_steps_(best_steps), best_path_set_(best_path_set), cur_(start) {}

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
    if (!*best_steps_) {
      if ((*b_)[cur_] == 'E') {
        *best_steps_ = num_steps();
      }
    }
    if (*best_steps_ && num_steps() > **best_steps_) return true;
    return false;
  }

  FastBoard::PointDir ToPointDir() const { return {.p = cur_, .d = dir_}; }

  void AddNextStepUpdatePaths(State* state, ReindeerPath2 next) const {
    switch(state->AddNextStep(next)) {
      case AddNextStepResult::kAdded: {
        best_path_set_->ReplacePaths(ToPointDir(), next.ToPointDir());
        break;
      }
      case AddNextStepResult::kMerged: {
        best_path_set_->MergePaths(ToPointDir(), next.ToPointDir());
        break;
      }
      case AddNextStepResult::kSkipped: break;
      default: LOG(FATAL) << "Bad AddNextStep";
    }
  }

  void AddNextSteps(State* state) const override {
    FastBoard::Point test = b_->Add(cur_, dir_);
    if (b_->OnBoard(test) && (*b_)[test] != '#') {
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
  const FastBoard* b_;
  std::optional<int>* best_steps_;
  BestPathHistory* best_path_set_;
  FastBoard::Point cur_;
  FastBoard::Dir dir_ = FastBoard::kEast;
};

}  // namespace

std::ostream& operator<<(std::ostream& o, const CharBoard& b) {
  return o << absl::StreamFormat("%v", b);
}

absl::StatusOr<std::string> Day_2024_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  FastBoard fb(b);
  FastBoard::Point fast_start = fb.From(start);
  return AdventReturn(ReindeerPath(fb, fast_start).FindMinStepsAStar());

  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2024_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(ImmutableCharBoard b, ImmutableCharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  FastBoard fb(b);
  FastBoard::Point fast_start = fb.From(start);
  
  BestPathHistory best_path_set(fb);
  std::optional<int> best_steps;
  ReindeerPath2 rp2(fb, fast_start, &best_steps, &best_path_set);
  std::optional<int> dist = rp2.FindMinStepsAStar();
  CHECK(best_steps);
  CHECK(dist);
  CHECK(*dist > *best_steps);

  ASSIGN_OR_RETURN(Point end, b.FindUnique('E'));
  std::vector<FastBoard::Point> final_set = best_path_set.CollectFrom(fb.From(end));

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
