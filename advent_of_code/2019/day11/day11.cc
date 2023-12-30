#include "advent_of_code/2019/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/ocr.h"
#include "advent_of_code/point.h"

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
    if (val != 0 && val != 1) {
      return absl::InvalidArgumentError("Bad output value");
    }
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
    std::vector<Point> white_points;
    for (const auto& [point, color] : panel_to_painted_color_) {
      if (color != 1) continue;
      white_points.push_back(point);
    }
    return CharBoard::DrawNew(white_points);
  }

 private:
  bool paint = true;
  Point cur_ = Cardinal::kOrigin;
  Point dir_ = Cardinal::kNorth;
  absl::flat_hash_map<Point, int> panel_to_painted_color_;
};

}  // namespace

absl::StatusOr<std::string> Day_2019_11::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  Painter painter;
  RETURN_IF_ERROR(codes.Run(&painter));

  return AdventReturn(painter.UniquePanelsPainted());
}

absl::StatusOr<std::string> Day_2019_11::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(IntCode codes, IntCode::Parse(input));

  Painter painter;
  painter.Set({.x = 0, .y = 0}, 1);
  RETURN_IF_ERROR(codes.Run(&painter));

  return OCRExtract(painter.Panels());
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/11,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_11()); });

}  // namespace advent_of_code
