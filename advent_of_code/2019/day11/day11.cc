#include "advent_of_code/2019/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

namespace advent_of_code {
namespace {

class Painter : public IntCode::IOModule {
 public:
  bool PauseIntCode() override { return false; }

  absl::StatusOr<int64_t> Fetch() override {
    auto it = panel_to_painted_color_.find(cur_);
    if (it == panel_to_painted_color_.end()) return 0;
    return it->second;
  }

  void Set(Point p, int val) { panel_to_painted_color_[p] = val; }

  absl::Status Put(int64_t val) override {
    if (val != 0 && val != 1)
      return absl::InvalidArgumentError("Bad output value");
    if (paint) {
      panel_to_painted_color_[cur_] = val;
    } else {
      if (!val /* left */) {
        dir_ = dir_.rotate_left();
      } else {
        dir_ = dir_.rotate_right();
      }
      cur_ += dir_;
    }
    paint = !paint;
    return absl::OkStatus();
  }

  int UniquePanelsPainted() { return panel_to_painted_color_.size(); }

  CharBoard Panels() {
    PointRectangle r = PointRectangle::Null();

    for (const auto& point_and_color : panel_to_painted_color_) {
      const Point& p = point_and_color.first;
      r.ExpandInclude(p);
    }
    VLOG(1) << r;
    CharBoard ret(r);
    for (Point p : r) {
      auto it = panel_to_painted_color_.find(p);
      bool is_white = (it != panel_to_painted_color_.end() && it->second == 1);
      ret[p - r.min] = is_white ? 'X' : ' ';
    }
    return ret;
  }

 private:
  bool paint = true;
  Point cur_ = Cardinal::kOrigin;
  Point dir_ = Cardinal::kNorth;
  absl::flat_hash_map<Point, int> panel_to_painted_color_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_11::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Painter painter;
  if (absl::Status st = codes->Run(&painter); !st.ok()) return st;

  return IntReturn(painter.UniquePanelsPainted());
}

absl::StatusOr<std::string> Day_2019_11::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Painter painter;
  painter.Set({.x = 0, .y = 0}, 1);
  if (absl::Status st = codes->Run(&painter); !st.ok()) return st;

  return absl::StrJoin(painter.Panels().rows, "\n");
}

}  // namespace advent_of_code
