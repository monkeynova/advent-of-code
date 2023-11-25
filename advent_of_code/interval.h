#ifndef ADVENT_OF_CODE_INTERVAL_H
#define ADVENT_OF_CODE_INTERVAL_H

#include <vector>

#include "absl/functional/function_ref.h"
#include "absl/strings/str_format.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"

namespace advent_of_code {

class Interval1D {
 public:
  static Interval1D OpenTop(int min) {
    Interval1D ret;
    ret.x_.push_back(min);
    return ret;
  } 

  Interval1D() = default;
  Interval1D(int start, int end) {
    if (end > start) {
      x_.push_back(start);
      x_.push_back(end);
    }
  }

  bool empty() const { return x_.empty(); }
  int Size() const;

  bool HasOverlap(const Interval1D& o) const;

  bool operator==(const Interval1D& o) const {
    return x_ == o.x_;
  }

  absl::Span<const int> x() const { return x_; }

  Interval1D Union(const Interval1D& o) const;
  Interval1D Intersect(const Interval1D& o) const;
  Interval1D Minus(const Interval1D& o) const;

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const Interval1D& iv) {
    if (iv.x_.empty()) {
      absl::Format(&sink, "∅");
    }
    for (int i = 0; i + 1 < iv.x_.size(); i += 2) {
      absl::Format(&sink, "[%d,%d)", iv.x_[i], iv.x_[i + 1]);
    }
    if (iv.x_.size() % 2 != 0) {
      absl::Format(&sink, "[%d,∞)", iv.x_.back());
    }
  }

  template <typename H>
  friend H AbslHashValue(H h, const Interval1D& iv) {
    return H::combine(std::move(h), iv.x_);
  }

 private:
  Interval1D Merge(const Interval1D& o,
                   absl::FunctionRef<bool(bool, bool)> merge_fn) const;

  std::vector<int> x_;
};

class RectangleSet {
 public:
  RectangleSet() = default;

  void Union(PointRectangle in) {
    SetDifference(in);
    set_.push_back(in);
  }

  void SetDifference(PointRectangle in) {
    std::vector<PointRectangle> new_set;
    for (const PointRectangle& r : set_) {
      r.SetDifference(in, &new_set);
    }
    set_ = std::move(new_set);
  }

  void SetDifference(const RectangleSet& o) {
    for (const PointRectangle& r : o.set_) {
      SetDifference(r);
    }
  }

  int64_t Area() const {
    int64_t sum = 0;
    for (const PointRectangle& r : set_) {
      sum += r.Area();
    }
    return sum;
  }

  absl::Span<const PointRectangle> rects() const { return set_; }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const RectangleSet& s) {
    for (const PointRectangle& r : s.set_) {
      absl::Format(&sink, "%v,", r);
    }
  }

 private:
  std::vector<PointRectangle> set_;
};

class CubeSet {
 public:
  CubeSet() = default;

  void Union(Cube in) {
    SetDifference(in);
    set_.push_back(in);
  }

  void SetDifference(Cube in) {
    std::vector<Cube> new_set;
    for (const Cube& c : set_) {
      c.SetDifference(in, &new_set);
    }
    set_ = std::move(new_set);
  }

  int64_t Volume() const {
    int64_t sum = 0;
    for (const Cube& c : set_) {
      sum += c.Volume();
    }
    return sum;
  }

 private:
  std::vector<Cube> set_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_INTERVAL_H