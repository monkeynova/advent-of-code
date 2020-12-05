#include "advent_of_code/2019/day15/day15.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/random/random.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

class Droid : public IntCode::IOModule {
 public:
  Droid() {
    SetBoard(pos_, 1);
    dir_ = kNorthDir;
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
    // return static_cast<InputDirection>(absl::Uniform(bitgen_, 1, 5));
  }

  absl::Status ComputePathToNearestUnknown() {
    struct Path {
      Point location;
      std::vector<InputDirection> directions;
    };
    absl::flat_hash_set<Point> computed_path = {pos_};
    std::deque<Path> paths;
    paths.push_back({.location = pos_});
    for (int depth = 1; !paths.empty(); ++depth) {
      const Path& to_extend = paths.front();
      for (InputDirection dir :
           {InputDirection::kNorthInput, InputDirection::kSouthInput,
            InputDirection::kWestInput, InputDirection::kEastInput}) {
        Point new_location =
            to_extend.location + kInputDirectionMap[static_cast<int>(dir)];
        if (computed_path.contains(new_location)) continue;
        if (!board_.contains(new_location)) {
          // Hit an unknown. Head here.
          current_path_.insert(current_path_.end(),
                               to_extend.directions.begin(),
                               to_extend.directions.end());
          current_path_.push_back(dir);
          return absl::OkStatus();
        }
        if (board_[new_location] == 0) {
          // Wall.
          continue;
        }
        paths.push_back(
            {.location = new_location, .directions = to_extend.directions});
        paths.back().directions.push_back(dir);
        computed_path.insert(new_location);
      }
      paths.pop_front();
    }
    // Fully explored the map.
    done_ = true;
    current_path_ = {InputDirection::kNorthInput};
    return absl::OkStatus();
  }

  absl::StatusOr<int> DistanceToO2() {
    struct Path {
      Point location;
      std::vector<InputDirection> directions;
    };
    absl::flat_hash_set<Point> computed_path = {Point{.x = 0, .y = 0}};
    std::deque<Path> paths;
    paths.push_back({.location = Point{.x = 0, .y = 0}});
    for (int depth = 1; !paths.empty(); ++depth) {
      const Path& to_extend = paths.front();
      for (InputDirection dir :
           {InputDirection::kNorthInput, InputDirection::kSouthInput,
            InputDirection::kWestInput, InputDirection::kEastInput}) {
        Point new_location =
            to_extend.location + kInputDirectionMap[static_cast<int>(dir)];
        if (computed_path.contains(new_location)) continue;
        if (!board_.contains(new_location)) {
          return absl::InternalError(
              "Path to unknown location finding O2 distaince");
        }
        if (board_[new_location] == 0) {
          // Wall.
          continue;
        }
        if (board_[new_location] == 2) {
          VLOG(1) << "Path = "
                  << absl::StrJoin(to_extend.directions, "",
                                   [](std::string* out, InputDirection dir) {
                                     constexpr absl::string_view rec[] = {
                                         "", "^", "\\/", "<", ">"};
                                     absl::StrAppend(
                                         out, rec[static_cast<int>(dir)]);
                                   });
          return to_extend.directions.size() + 1;
        }
        paths.push_back(
            {.location = new_location, .directions = to_extend.directions});
        paths.back().directions.push_back(dir);
        computed_path.insert(new_location);
      }
      paths.pop_front();
    }
    return absl::InternalError("Ran out of paths");
  }

  absl::StatusOr<int> GreatestDistanceFromO2() {
    struct Path {
      Point location;
      std::vector<InputDirection> directions;
    };
    absl::flat_hash_set<Point> computed_path = {o2_pos_};
    std::deque<Path> paths;
    paths.push_back({.location = o2_pos_});
    int max_path = 0;
    for (int depth = 0; !paths.empty(); ++depth) {
      const Path& to_extend = paths.front();
      max_path = std::max<int>(max_path, to_extend.directions.size());
      for (InputDirection dir :
           {InputDirection::kNorthInput, InputDirection::kSouthInput,
            InputDirection::kWestInput, InputDirection::kEastInput}) {
        Point new_location =
            to_extend.location + kInputDirectionMap[static_cast<int>(dir)];
        if (computed_path.contains(new_location)) continue;
        if (!board_.contains(new_location)) {
          return absl::InternalError(
              "Path to unknown location finding O2 distaince");
        }
        if (board_[new_location] == 0) {
          // Wall.
          continue;
        }
        paths.push_back(
            {.location = new_location, .directions = to_extend.directions});
        paths.back().directions.push_back(dir);
        computed_path.insert(new_location);
      }
      paths.pop_front();
    }
    return max_path;
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
  Point pos_{0, 0};
  Point dir_;
  Point o2_pos_;
  bool done_ = false;

  static constexpr Point kNorthDir = {.x = 0, .y = -1};
  static constexpr Point kSouthDir = {.x = 0, .y = 1};
  static constexpr Point kWestDir = {.x = -1, .y = 0};
  static constexpr Point kEastDir = {.x = 1, .y = 0};
  static constexpr Point kInputDirectionMap[] = {Point{}, kNorthDir, kSouthDir,
                                                 kWestDir, kEastDir};

  Point min_ = {.x = std::numeric_limits<int>::max(),
                .y = std::numeric_limits<int>::max()};
  Point max_ = {.x = std::numeric_limits<int>::min(),
                .y = std::numeric_limits<int>::min()};
  absl::flat_hash_map<Point, int> board_;
};

absl::StatusOr<std::vector<std::string>> Day15_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Droid droid;
  if (absl::Status st = codes->Run(&droid); !st.ok()) {
    return st;
  }
  LOG(WARNING) << droid.min() << "-" << droid.max();
  LOG(WARNING) << droid.DebugBoard();
  absl::StatusOr<int> distance = droid.DistanceToO2();
  if (!distance.ok()) return distance.status();

  return std::vector<std::string>{absl::StrCat(*distance)};
}

absl::StatusOr<std::vector<std::string>> Day15_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Droid droid;
  if (absl::Status st = codes->Run(&droid); !st.ok()) {
    return st;
  }
  LOG(WARNING) << droid.min() << "-" << droid.max();
  LOG(WARNING) << droid.DebugBoard();
  absl::StatusOr<int> distance = droid.GreatestDistanceFromO2();
  if (!distance.ok()) return distance.status();

  return std::vector<std::string>{absl::StrCat(*distance)};
}
