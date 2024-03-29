#include "advent_of_code/2019/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/random/random.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point_walk.h"

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
    ASSIGN_OR_RETURN(InputDirection direction, PickDirection());
    dir_ = kInputDirectionMap[static_cast<int>(direction)];
    return static_cast<int64_t>(direction);
  }

  absl::StatusOr<InputDirection> PickDirection() {
    if (current_path_.empty()) {
      RETURN_IF_ERROR(ComputePathToNearestUnknown());
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

      bool IsFinal() const override {
        if (board_.contains(cur_)) return false;
        out_directions_->clear();
        out_directions_->insert(out_directions_->end(), directions_.begin(),
                                directions_.end());
        return true;
      }

      void AddNextSteps(State* state) const override {
        for (InputDirection dir :
             {InputDirection::kNorthInput, InputDirection::kSouthInput,
              InputDirection::kWestInput, InputDirection::kEastInput}) {
          Point new_location = cur_ + kInputDirectionMap[static_cast<int>(dir)];
          if (auto it = board_.find(new_location);
              it != board_.end() && it->second == 0) {
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

    std::optional<int> dist =
        PathWalk(board_, pos_, &current_path_).FindMinSteps();
    if (!dist) {
      // Fully explored the map.
      done_ = true;
      current_path_ = {InputDirection::kNorthInput};
    }
    return absl::OkStatus();
  }

  std::optional<int> DistanceToO2() {
    return PointWalk(
               {.start = Cardinal::kOrigin,
                .is_good =
                    [&](Point test, int num_steps) {
                      auto it = board_.find(test);
                      CHECK(it != board_.end());
                      return it->second != 0;
                    },
                .is_final = [&](Point test, int) { return test == o2_pos_; }})
        .FindMinSteps();
  }

  absl::StatusOr<int> GreatestDistanceFromO2() {
    int max_dist = -1;
    PointWalk({.start = o2_pos_,
               .is_good =
                   [&](Point test, int num_steps) {
                     auto it = board_.find(test);
                     CHECK(it != board_.end());
                     if (it->second == 0) return false;
                     return true;
                   },
               .is_final =
                   [&](Point, int num_steps) {
                     max_dist = std::max(max_dist, num_steps);
                     return false;
                   }})
        .Walk();
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
    VLOG(2) << "Board:\n" << DebugBoard();
    return absl::OkStatus();
  }

  CharBoard DebugBoard() const {
    std::vector<Point> wall;
    for (const auto& [p, v] : board_) {
      if (v == 0) wall.push_back(p);
    }
    PointRectangle bounds;
    CharBoard draw = CharBoard::DrawNew(wall, &bounds);
    draw[o2_pos_ - bounds.min] = '*';
    draw[pos_ - bounds.min] = 'R';
    draw[Cardinal::kOrigin - bounds.min] = '+';
    return draw;
  }

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

  absl::flat_hash_map<Point, int> board_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_15::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  Droid droid;
  RETURN_IF_ERROR(codes.Run(&droid));
  VLOG(1) << "Droid:\n" << droid.DebugBoard();

  return AdventReturn(droid.DistanceToO2());
}

absl::StatusOr<std::string> Day_2019_15::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  Droid droid;
  RETURN_IF_ERROR(codes.Run(&droid));
  VLOG(1) << "Droid:\n" << droid.DebugBoard();

  return AdventReturn(droid.GreatestDistanceFromO2());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/15,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_15()); });

}  // namespace advent_of_code
