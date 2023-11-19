#include "advent_of_code/2019/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

class Drone : public IntCode::IOModule {
 public:
  Drone(Point probe) : probe_(probe), state_(State::kFetchX) {}

  int output() { return output_; }

  bool PauseIntCode() override { return false; }

  absl::StatusOr<int64_t> Fetch() override {
    switch (state_) {
      case State::kFetchX: {
        state_ = State::kFetchY;
        VLOG(3) << "Fetch (x): => " << probe_.x;
        return probe_.x;
      }
      case State::kFetchY: {
        state_ = State::kReceiveActive;
        VLOG(3) << "Fetch (y): => " << probe_.y;
        return probe_.y;
      }
      default:
        return absl::InvalidArgumentError("Can't fetch in this state");
    }
  }

  absl::Status Put(int64_t val) override {
    if (state_ != State::kReceiveActive) {
      return absl::InternalError("Not ready to recieve");
    }
    VLOG(3) << "Put: " << probe_ << "=" << val;
    output_ = val;
    return absl::OkStatus();
  }

 private:
  Point probe_;
  int output_;
  enum class State {
    kNoState = 0,
    kFetchX = 1,
    kFetchY = 2,
    kReceiveActive = 3,
    kDone = 4,
  };
  State state_ = State::kNoState;
};

class TractorSearch {
 public:
  TractorSearch(IntCode codes) : codes_(std::move(codes)) {}

  absl::StatusOr<int> ScanRange(Point min, Point max) {
    int active = 0;
    for (int y = min.y; y < max.y; ++y) {
      for (int x = min.x; x < max.x; ++x) {
        Point p{x, y};
        absl::StatusOr<int> out = ScanPoint(p);
        if (!out.ok()) return out.status();
        VLOG(1) << p << "=" << *out;
        if (*out) ++active;
      }
    }
    return active;
  }

  std::string DebugBoard(Point min, Point max) {
    PointRectangle r{.min = min, .max = max};
    CharBoard board(r);
    for (Point p : r) {
      absl::StatusOr<int> out = ScanPoint(p);
      if (out.ok() && *out) board[p - r.min] = '#';
    }
    return absl::StrCat(board);
  }

  absl::StatusOr<Point> FindSquareSpace(int size) {
    VLOG(1) << "Find: " << size << "\n"
            << DebugBoard(Point{6, 8}, Point{30, 30});

    struct Range {
      Point min;
      Point max;
    };
    std::vector<Range> ranges;
    for (Point probe{.x = size, .y = 0};; ++probe.y) {
      absl::StatusOr<int> on = ScanPoint(probe);
      if (!on.ok()) return on.status();
      if (*on) {
        ranges.resize(probe.y + 1);
        ranges.back().min = probe;
        ranges.back().max = probe;
        break;
      }
      if (probe.y > 2 * size)
        return absl::InvalidArgumentError("Can't find start");
    }
    while (true) {
      absl::StatusOr<int> on = ScanPoint(ranges.back().min);
      if (!on.ok()) return on.status();
      if (!*on) {
        ++ranges.back().min.x;
        break;
      }
      --ranges.back().min.x;
      if (ranges.back().min.x < 0)
        return absl::InvalidArgumentError("Can't find min");
    }
    while (true) {
      absl::StatusOr<int> on = ScanPoint(ranges.back().max);
      if (!on.ok()) return on.status();
      if (!*on) {
        --ranges.back().max.x;
        break;
      }
      ++ranges.back().max.x;
      if (ranges.back().min.x < 0)
        return absl::InvalidArgumentError("Can't find max");
    }
    VLOG(2) << "Start Range[" << ranges.size() - 1 << "] " << ranges.back().min
            << "-" << ranges.back().max;
    while (true) {
      Range next_range = ranges.back();
      ++next_range.min.y;
      ++next_range.max.y;
      ++next_range.max.x;
      while (true) {
        absl::StatusOr<int> on = ScanPoint(next_range.min);
        if (!on.ok()) return on.status();
        if (*on) break;
        ++next_range.min.x;
      }
      if (next_range.min.x > next_range.max.x) {
        // Next range started after the end of the previous.
        next_range.max.x = next_range.min.x;
      }
      while (true) {
        absl::StatusOr<int> on = ScanPoint(next_range.max);
        if (!on.ok()) return on.status();
        if (!*on) {
          --next_range.max.x;
          break;
        }
        ++next_range.max.x;
      }
      ranges.push_back(next_range);
      VLOG(2) << "Range[" << ranges.size() - 1 << "] " << next_range.min << "-"
              << next_range.max;
      if (ranges.size() > 100 && ranges[ranges.size() - 100].max.x >=
                                     ranges[ranges.size() - 1].min.x + 99) {
        return Point{
            .x = ranges[ranges.size() - 1].min.x,
            .y = static_cast<int>(ranges.size() - 100),
        };
      }
    }

    return Point{0, 0};
  }

  absl::StatusOr<int> ScanPoint(Point p) {
    Drone prober(p);
    IntCode new_codes = codes_.Clone();
    RETURN_IF_ERROR(new_codes.Run(&prober));
    VLOG(2) << "ScanPoint: " << p << " = " << prober.output();

    return prober.output();
  }

 private:
  IntCode codes_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_19::Part1(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  TractorSearch search(std::move(*codes));

  return AdventReturn(search.ScanRange(Point{0, 0}, Point{50, 50}));
}

absl::StatusOr<std::string> Day_2019_19::Part2(
    absl::Span<std::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  TractorSearch search(std::move(*codes));
  absl::StatusOr<Point> space = search.FindSquareSpace(100);
  if (!space.ok()) return space.status();

  return AdventReturn(space->x * 10000 + space->y);
}

}  // namespace advent_of_code
