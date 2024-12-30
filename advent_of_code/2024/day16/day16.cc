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
  ReindeerPath(const CharBoard& b, Point start)
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
    if (b_->OnBoard(cur_ + dir_) && (*b_)[cur_ + dir_] != '#') {
      ReindeerPath next = *this;
      next.cur_ = cur_ + dir_;
      state->AddNextStep(next);
    }
    {
      ReindeerPath left = *this;
      left.dir_ = left.dir_.rotate_left();
      left.add_steps(999);
      state->AddNextStep(left);
    }
    {
      ReindeerPath right = *this;
      right.dir_ = right.dir_.rotate_right();
      right.add_steps(999);
      state->AddNextStep(right);
    }
  }  

 private:
  const CharBoard* b_;
  Point cur_;
  Point dir_ = Cardinal::kEast;
};

class ReindeerPath2 : public BFSInterface<ReindeerPath2> {
 public:
  ReindeerPath2(const CharBoard& b, Point start,
                std::optional<int>* best_steps,
                absl::flat_hash_map<std::pair<Point, Point>, std::vector<std::pair<Point, Point>>>* best_path_set)
   : b_(&b), best_steps_(best_steps), best_path_set_(best_path_set), cur_(start) {}

  Point cur() const { return cur_; }

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

  void MergePaths(const ReindeerPath2& o) const {
    (*best_path_set_)[{o.cur_, o.dir_}].push_back({cur_, dir_});
  }

  void ReplacePaths(const ReindeerPath2& o) const {
    (*best_path_set_)[{o.cur_, o.dir_}].clear();
    MergePaths(o);
  }

  void AddNextSteps(State* state) const override {
    if (b_->OnBoard(cur_ + dir_) && (*b_)[cur_ + dir_] != '#') {
      ReindeerPath2 next = *this;
      next.cur_ = cur_ + dir_;
      switch(state->AddNextStep(next)) {
        case AddNextStepResult::kAdded: ReplacePaths(next); break;
        case AddNextStepResult::kMerged: MergePaths(next); break;
        case AddNextStepResult::kSkipped: break;
        default: LOG(FATAL) << "Bad AddNextStep";
      }
    }
    {
      ReindeerPath2 left = *this;
      left.dir_ = left.dir_.rotate_left();
      left.add_steps(999);
      switch(state->AddNextStep(left)) {
        case AddNextStepResult::kAdded: ReplacePaths(left); break;
        case AddNextStepResult::kMerged: MergePaths(left); break;
        case AddNextStepResult::kSkipped: break;
        default: LOG(FATAL) << "Bad AddNextStep";
      }
    }
    {
      ReindeerPath2 right = *this;
      right.dir_ = right.dir_.rotate_right();
      right.add_steps(999);
      switch(state->AddNextStep(right)) {
        case AddNextStepResult::kAdded: ReplacePaths(right); break;
        case AddNextStepResult::kMerged: MergePaths(right); break;
        case AddNextStepResult::kSkipped: break;
        default: LOG(FATAL) << "Bad AddNextStep";
      }
    }
  }  

 private:
  const CharBoard* b_;
  std::optional<int>* best_steps_;
  absl::flat_hash_map<std::pair<Point, Point>, std::vector<std::pair<Point, Point>>>* best_path_set_;
  Point cur_;
  Point dir_ = Cardinal::kEast;
};

}  // namespace

std::ostream& operator<<(std::ostream& o, const CharBoard& b) {
  return o << absl::StreamFormat("%v", b);
}

absl::StatusOr<std::string> Day_2024_16::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  return AdventReturn(ReindeerPath(b, start).FindMinStepsAStar());

  return absl::UnimplementedError("Problem not known");
}

absl::StatusOr<std::string> Day_2024_16::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(CharBoard b, CharBoard::Parse(input));
  ASSIGN_OR_RETURN(Point start, b.FindUnique('S'));
  
  LOG(ERROR) << "Start";
  absl::flat_hash_map<std::pair<Point, Point>, std::vector<std::pair<Point, Point>>> best_path_set;
  std::optional<int> best_steps;
  ReindeerPath2 rp2(b, start, &best_steps, &best_path_set);
  std::optional<int> dist = rp2.FindMinStepsAStar();
  CHECK(best_steps);
  CHECK(dist);
  CHECK(*dist > *best_steps);
  LOG(ERROR) << "End";

  ASSIGN_OR_RETURN(Point end, b.FindUnique('E'));
  absl::flat_hash_set<Point> final_set = {end};

  std::deque<std::pair<Point, Point>> frontier = {
    {end, Cardinal::kNorth}, {end, Cardinal::kSouth},
    {end, Cardinal::kWest}, {end, Cardinal::kEast}, 
  };
  absl::flat_hash_set<std::pair<Point, Point>> hist;
  for (const auto& pair : frontier) hist.insert(pair);
  for (/*nop*/;!frontier.empty(); frontier.pop_front()) {
    auto it = best_path_set.find(frontier.front());
    if (it != best_path_set.end()) {
      for (const auto& pair : it->second) {
        if (hist.contains(pair)) continue;
        hist.insert(pair);
        frontier.push_back(pair);
        final_set.insert(pair.first);
      }
    }
  }

  if (VLOG_IS_ON(2)) {
    CharBoard draw = b;
    for (Point p : final_set) draw[p] = 'O';
    VLOG(2) << "With path:\n" << draw;
  }

  return AdventReturn(final_set.size());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2024, /*day=*/16,
    []() { return std::unique_ptr<AdventDay>(new Day_2024_16()); });

}  // namespace advent_of_code
