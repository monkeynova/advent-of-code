#include "advent_of_code/2019/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/random/random.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

namespace advent_of_code {
namespace {

class Droid : public IntCode::IOModule {
 public:
  Droid() {
    SetBoard(pos_, 1);
    dir_ = Cardinal::kNorth;
  }

  enum class InputDirection {
    kNorthInput = 1,
    kSouthInput = 2,
    kWestInput = 3,
    kEastInput = 4,
  };

  bool PauseIntCode() override { return done_; }

  absl::StatusOr<int64_t> Fetch() override {
    absl::StatusOr<InputDirection> direction = PickDirection();
    if (!direction.ok()) return direction.status();
    dir_ = kInputDirectionMap[static_cast<int>(*direction)];
    return static_cast<int64_t>(*direction);
  }

  absl::StatusOr<InputDirection> PickDirection() {
    if (current_path_.empty()) {
      if (absl::Status st = ComputePathToNearestUnknown(); !st.ok()) return st;
    }
    InputDirection dir = current_path_.front();
    current_path_.pop_front();
    return dir;
  }

  absl::Status ComputePathToNearestUnknown() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const absl::flat_hash_map<Point, int>& board, Point start,
               std::deque<InputDirection>* out_directions)
       : board_(board), cur_(start), out_directions_(out_directions) {}

      Point identifier() const override { return cur_; }

      bool IsFinal() override {
        if (board_.contains(cur_)) return false;
        out_directions_->clear();
        out_directions_->insert(out_directions_->end(), directions_.begin(), directions_.end());
        return true;
      }

      void AddNextSteps(State* state) override {
        for (InputDirection dir :
             {InputDirection::kNorthInput, InputDirection::kSouthInput,
              InputDirection::kWestInput, InputDirection::kEastInput}) {
          Point new_location = cur_ + kInputDirectionMap[static_cast<int>(dir)];
          if (auto it = board_.find(new_location); it != board_.end() && it->second == 0) {
            // Wall.
            continue;
          }
          PathWalk next = *this;
          next.cur_ = new_location;
          next.directions_.push_back(dir);
          state->AddNextStep(next);
        }
      }

     private:
      const absl::flat_hash_map<Point, int>& board_;
      Point cur_;
      std::vector<InputDirection> directions_;
      std::deque<InputDirection>* out_directions_;
    };

    absl::optional<int> dist =
        PathWalk(board_, pos_, &current_path_).FindMinSteps();
    if (!dist) {
      // Fully explored the map.
      done_ = true;
      current_path_ = {InputDirection::kNorthInput};
    }
    return absl::OkStatus();
  }

  absl::optional<int> DistanceToO2() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const absl::flat_hash_map<Point, int>& board, Point start)
       : board_(board), cur_(start) {}

      Point identifier() const override { return cur_; }

      bool IsFinal() override {
        auto it = board_.find(cur_);
        CHECK(it != board_.end());
        return it->second == 2;
      }

      void AddNextSteps(State* state) override {
        for (Point dir : Cardinal::kFourDirs) {
          Point next_cur = cur_ + dir;
          auto it = board_.find(next_cur);
          CHECK(it != board_.end());
          if (it->second == 0) continue;
          PathWalk next = *this;
          next.cur_ = next_cur;
          state->AddNextStep(next);
        }
      }
      
     private:
      const absl::flat_hash_map<Point, int>& board_;
      Point cur_;
    };
    return PathWalk(board_, Cardinal::kOrigin).FindMinSteps();
  }

  absl::StatusOr<int> GreatestDistanceFromO2() {
    class PathWalk : public BFSInterface<PathWalk, Point> {
     public:
      PathWalk(const absl::flat_hash_map<Point, int>& board, Point start, int* max_dist)
       : board_(board), cur_(start), max_dist_(max_dist) {}

      Point identifier() const override { return cur_; }

      bool IsFinal() override {
        return false;
      }

      void AddNextSteps(State* state) override {
        // TODO(@monkeynova): Move max_dist check to IsFinal once BFSInterface
        // checks history before calling IsFinal.
        *max_dist_ = std::max(*max_dist_, num_steps());
        for (Point dir : Cardinal::kFourDirs) {
          Point next_cur = cur_ + dir;
          auto it = board_.find(next_cur);
          CHECK(it != board_.end());
          if (it->second == 0) continue;
          PathWalk next = *this;
          next.cur_ = next_cur;
          state->AddNextStep(next);
        }
      }
      
     private:
      const absl::flat_hash_map<Point, int>& board_;
      Point cur_;
      int* max_dist_;
    };

    int max_dist = -1;
    absl::optional<int> should_be_empty =
        PathWalk(board_, o2_pos_, &max_dist).FindMinSteps();
    if (should_be_empty) {
      return AdventDay::Error("Internal error: GreatestDistanceFromO2 (found path)");
    }
    return max_dist;
  }

  enum class OutputStatus {
    kHitWall = 0,
    kMovedOneStep = 1,
    kMovedOneStepFoundO2 = 2,
  };

  void SetBoard(Point p, int val) {
    board_.emplace(p, val);
    if (val == 2) {
      o2_pos_ = p;
    }
    min_.x = std::min(min_.x, p.x);
    min_.y = std::min(min_.y, p.y);
    max_.x = std::max(max_.x, p.x);
    max_.y = std::max(max_.y, p.y);
  }

  absl::Status Put(int64_t val) override {
    Point next = pos_ + dir_;
    auto it = board_.find(next);
    if (it != board_.end()) {
      if (it->second != val) {
        return absl::InternalError("Board changed");
      }
      // All good.
    } else {
      SetBoard(next, val);
    }
    if (val != 0) {
      pos_ += dir_;
    }
    VLOG(2) << "Board:" << DebugBoard();
    return absl::OkStatus();
  }

  std::string DebugBoard() const {
    char render[] = {'#', ' ', '*'};
    std::string ret;
    ret.resize((max_.y - min_.y + 1) * (max_.x - min_.x + 4));
    int mark = 0;
    ret[mark++] = '\n';
    ret[mark++] = '|';
    for (int y = min_.y; y <= max_.y; ++y) {
      for (int x = min_.x; x <= max_.x; ++x) {
        Point p{.x = x, .y = y};
        char c;
        if (p == Point{.x = 0, .y = 0}) {
          c = '+';
        } else if (p == pos_) {
          c = 'R';
        } else {
          auto it = board_.find(p);
          c = it == board_.end() ? '?' : render[it->second];
        }
        ret[mark++] = c;
      }
      ret[mark++] = '|';
      if (y != max_.y) {
        ret[mark++] = '\n';
        ret[mark++] = '|';
      }
    }
    return ret;
  }

  Point min() const { return min_; }
  Point max() const { return max_; }

 private:
  absl::BitGen bitgen_;
  std::deque<InputDirection> current_path_;
  Point pos_ = Cardinal::kOrigin;
  Point dir_;
  Point o2_pos_;
  bool done_ = false;

  static constexpr Point kInputDirectionMap[] = {
      Cardinal::kOrigin, Cardinal::kNorth, Cardinal::kSouth, Cardinal::kWest,
      Cardinal::kEast};

  Point min_ = {.x = std::numeric_limits<int>::max(),
                .y = std::numeric_limits<int>::max()};
  Point max_ = {.x = std::numeric_limits<int>::min(),
                .y = std::numeric_limits<int>::min()};
  absl::flat_hash_map<Point, int> board_;
};

}  // namespace

absl::StatusOr<std::vector<std::string>> Day15_2019::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Droid droid;
  if (absl::Status st = codes->Run(&droid); !st.ok()) {
    return st;
  }
  VLOG(1) << droid.min() << "-" << droid.max();
  VLOG(1) << droid.DebugBoard();

  return IntReturn(droid.DistanceToO2());
}

absl::StatusOr<std::vector<std::string>> Day15_2019::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Droid droid;
  if (absl::Status st = codes->Run(&droid); !st.ok()) {
    return st;
  }
  VLOG(1) << droid.min() << "-" << droid.max();
  VLOG(1) << droid.DebugBoard();

  return IntReturn(droid.GreatestDistanceFromO2());
}

}  // namespace advent_of_code
