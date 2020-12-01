#include "advent_of_code/2019/day11/day11.h"

#include "absl/container/flat_hash_map.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2019/int_code.h"
#include "glog/logging.h"

struct Point {
  int x;
  int y;

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Point& other) const { return !operator==(other); }

  Point operator-(const Point& other) const {
    return {.x = x - other.x, .y = y - other.y};
  }

  Point operator+(const Point& other) const {
    return {.x = x + other.x, .y = y + other.y};
  }

  Point& operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Point min_step() const {
    int gcd = std::gcd(abs(x), abs(y));
    return {.x = x / gcd, .y = y / gcd};
  }
};

template <typename H>
H AbslHashValue(H h, const Point& p) {
  return H::combine(std::move(h), p.x, p.y);
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
  out << "{" << p.x << "," << p.y << "}";
  return out;
}

class Painter : public IntCode::InputSource, public IntCode::OutputSink {
 public:
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
      // (0, -1) + 0=left  -> (-1, 0)
      // (0, -1) + 1=right -> (1, 0)
      // (0, 1)  + 0=left  -> (1, 0)
      // (0, 1)  + 1=right -> (-1, 0)
      // (1, 0)  + 0=left  -> (0, -1)
      // (1, 0)  + 1=right -> (0, 1)
      // (-1, 0) + 0=left  -> (0, 1)
      // (-1, 0) + 1=right -> (0, -1)
      // Rotate left  = [[0, 1], [-1, 0]]
      // Rotate right = [[0, -1], [1, 0]]
      if (!val /* left */) {
        dir_ = {.x = dir_.y, .y = -dir_.x};
      } else {
        dir_ = {.x = -dir_.y, .y = dir_.x};
      }
      cur_ += dir_;
    }
    paint = !paint;
    return absl::OkStatus();
  }

  int UniquePanelsPainted() { return panel_to_painted_color_.size(); }

  std::vector<std::string> Panels() {
    Point min{.x = std::numeric_limits<int>::max(),
              .y = std::numeric_limits<int>::max()};
    Point max{.x = std::numeric_limits<int>::min(),
              .y = std::numeric_limits<int>::min()};
    for (const auto& point_and_color : panel_to_painted_color_) {
      const Point& p = point_and_color.first;
      min.x = std::min(min.x, p.x);
      min.y = std::min(min.y, p.y);
      max.x = std::max(max.x, p.x);
      max.y = std::max(max.y, p.y);
    }
    LOG(WARNING) << min << " => " << max;
    std::vector<std::string> ret;
    for (int y = min.y; y <= max.y; ++y) {
      std::string next;
      for (int x = min.x; x <= max.x; ++x) {
        auto it = panel_to_painted_color_.find({.x = x, .y = y});
        bool is_white =
            (it != panel_to_painted_color_.end() && it->second == 1);
        next.append(is_white ? "X" : " ");
      }
      ret.push_back(next);
    }
    return ret;
  }

 private:
  bool paint = true;
  Point cur_ = {0, 0};
  Point dir_ = {0, -1};
  absl::flat_hash_map<Point, int> panel_to_painted_color_;
};

absl::StatusOr<std::vector<std::string>> Day11_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Painter painter;
  if (absl::Status st = codes->Run(&painter, &painter); !st.ok()) return st;

  return std::vector<std::string>{absl::StrCat(painter.UniquePanelsPainted())};
}

absl::StatusOr<std::vector<std::string>> Day11_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  absl::StatusOr<IntCode> codes = IntCode::Parse(input);
  if (!codes.ok()) return codes.status();

  Painter painter;
  painter.Set({.x = 0, .y = 0}, 1);
  if (absl::Status st = codes->Run(&painter, &painter); !st.ok()) return st;

  return painter.Panels();
}