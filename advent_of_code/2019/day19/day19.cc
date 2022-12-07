// https://adventofcode.com/2019/day/19
//
// --- Day 19: Tractor Beam ---
// ----------------------------
// 
// Unsure of the state of Santa's ship, you borrowed the tractor beam
// technology from Triton. Time to test it out.
// 
// When you're safely away from anything else, you activate the tractor
// beam, but nothing happens. It's hard to tell whether it's working if
// there's nothing to use it on. Fortunately, your ship's drone system
// can be configured to deploy a drone to specific coordinates and then
// check whether it's being pulled. There's even an Intcode program (your
// puzzle input) that gives you access to the drone system.
// 
// The program uses two input instructions to request the X and Y
// position to which the drone should be deployed. Negative numbers are
// invalid and will confuse the drone; all numbers should be zero or
// positive.
// 
// Then, the program will output whether the drone is stationary (0) or
// being pulled by something (1). For example, the coordinate X=0, Y=0 is
// directly in front of the tractor beam emitter, so the drone control
// program will always report 1 at that location.
// 
// To better understand the tractor beam, it is important to get a good
// picture of the beam itself. For example, suppose you scan the 10x10
// grid of points closest to the emitter:
// 
// X
// 0->      9
// 0#.........
// |.#........
// v..##......
// ...###....
// ....###...
// Y .....####.
// ......####
// ......####
// .......###
// 9........##
// 
// In this example, the number of points affected by the tractor beam in
// the 10x10 area closest to the emitter is 27.
// 
// However, you'll need to scan a larger area to understand the shape of
// the beam. How many points are affected by the tractor beam in the
// 50x50 area closest to the emitter? (For each of X and Y, this will be
// 0 through 49.)
//
// --- Part Two ---
// ----------------
// 
// You aren't sure how large Santa's ship is. You aren't even sure if
// you'll need to use this thing on Santa's ship, but it doesn't hurt to
// be prepared. You figure Santa's ship might fit in a 100x100 square.
// 
// The beam gets wider as it travels away from the emitter; you'll need
// to be a minimum distance away to fit a square of that size into the
// beam fully. (Don't rotate the square; it should be aligned to the same
// axes as the drone grid.)
// 
// For example, suppose you have the following tractor beam readings:
// 
// #.......................................
// .#......................................
// ..##....................................
// ...###..................................
// ....###.................................
// .....####...............................
// ......#####.............................
// ......######............................
// .......#######..........................
// ........########........................
// .........#########......................
// ..........#########.....................
// ...........##########...................
// ...........############.................
// ............############................
// .............#############..............
// ..............##############............
// ...............###############..........
// ................###############.........
// ................#################.......
// .................########   O   OOOOOOOOO.....
// ..................#######OOOOOOOOOO#....
// ...................######OOOOOOOOOO###..
// ....................#####OOOOOOOOOO#####
// .....................####OOOOOOOOOO#####
// .....................####OOOOOOOOOO#####
// ......................###OOOOOOOOOO#####
// .......................##OOOOOOOOOO#####
// ........................#OOOOOOOOOO#####
// .........................OOOOOOOOOO#####
// ..........................##############
// ..........................##############
// ...........................#############
// ............................############
// .............................###########
// 
// In this example, the 10x10 square closest to the emitter that fits
// entirely within the tractor beam has been marked O. Within it, the
// point closest to the emitter (the only highlighted O) is at X=25, Y=20.
// 
// Find the 100x100 square closest to the emitter that fits entirely
// within the tractor beam; within that square, find the point closest to
// the emitter. What value do you get if you take that point's X
// coordinate, multiply it by 10000, then add the point's Y coordinate?
// (In the example above, this would be 250020.)


#include "advent_of_code/2019/day19/day19.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
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
    return board.AsString();
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
    if (absl::Status st = new_codes.Run(&prober); !st.ok()) {
      return st;
    }
    VLOG(2) << "ScanPoint: " << p << " = " << prober.output();

    return prober.output();
  }

 private:
  IntCode codes_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_19::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  TractorSearch search(std::move(*codes));

  return IntReturn(search.ScanRange(Point{0, 0}, Point{50, 50}));
}

absl::StatusOr<std::string> Day_2019_19::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  TractorSearch search(std::move(*codes));
  absl::StatusOr<Point> space = search.FindSquareSpace(100);
  if (!space.ok()) return space.status();

  return IntReturn(space->x * 10000 + space->y);
}

}  // namespace advent_of_code
