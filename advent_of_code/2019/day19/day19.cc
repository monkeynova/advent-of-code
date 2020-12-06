#include "advent_of_code/2019/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

class Drone : public IntCode::IOModule {
 public:
  void SetScan(Point min, Point max) {
    min_ = min;
    max_ = max;
    cur_ = min_;
    board_.clear();
    std::string blank;
    blank.resize(max_.x - min_.x, '?');
    board_.resize(max_.y - min_.y, blank);
    state_ = State::kFetchX;
  }

  bool is_done() { return state_ == State::kDone; }

  bool PauseIntCode() override { return is_done(); }

  absl::StatusOr<int64_t> Fetch() override {
    switch (state_) {
      case State::kFetchX: {
        state_ = State::kFetchY;
        VLOG(2) << "Fetch (x): => " << cur_.x;
        return cur_.x;
      }
      case State::kFetchY: {
        state_ = State::kReceiveActive;
        VLOG(2) << "Fetch (y): => " << cur_.y;
        return cur_.y;
      }
      default: return absl::InvalidArgumentError("Can't fetch in this state");
    }
  }

  absl::Status Put(int64_t val) override {
    if (state_ != State::kReceiveActive) {
      return absl::InternalError("Not ready to recieve");
    }
    VLOG(2) << "Put: " << cur_ << "=" << val;
    board_[cur_.y - min_.y][cur_.x - min_.x] = val ? '#' : '.';
    state_ = State::kFetchX;
    if (++cur_.x == max_.x) {
      cur_.x = 0;
      if (++cur_.y == max_.y) {
        cur_.y = 0;
        state_ = State::kDone;
      }
    }
    return absl::OkStatus();
  }

  int ActiveTractorLocations() const {
    VLOG(1) << "\n" << absl::StrJoin(board_, "\n");
    int active = 0;
    for (const std::string& line : board_) {
      for (char c : line) {
        if (c == '#') ++active;
      }
    }
    return active;
  }

 private:
  enum class State {
    kNoState = 0,
    kFetchX = 1,
    kFetchY = 2,
    kReceiveActive = 3,
    kDone = 4,
  };
  State state_ = State::kNoState;
  Point min_;
  Point max_;
  Point cur_;
  std::vector<std::string> board_;
};

absl::StatusOr<std::vector<std::string>> Day19_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Drone drone;
  drone.SetScan(Point{0,0}, Point{50, 50});
  while (!drone.is_done()) {
    IntCode new_codes = codes->Clone();
    if (absl::Status st = new_codes.Run(&drone); !st.ok()) {
      return st;
    }
  }

  return std::vector<std::string>{absl::StrCat(drone.ActiveTractorLocations())};
}

absl::StatusOr<std::vector<std::string>> Day19_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
