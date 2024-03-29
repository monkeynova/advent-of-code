#include "advent_of_code/interval.h"

#include "vlog.h"

namespace advent_of_code {

int InfiniteBinarySearch(absl::FunctionRef<bool(int)> cmp) {
  int min = 1;
  int max = 0;
  while (max == 0) {
    int test = 2 * min;
    if (cmp(test)) {
      min = test;
    } else {
      max = test;
    }
  }
  while (min != max) {
    int test = (max + min) / 2;
    if (cmp(test)) {
      min = test + 1;
    } else {
      max = test;
    }
  }
  return min;
}

int64_t Interval1D::Size() const {
  int64_t size = 0;
  for (int i = 0; i < x_.size(); i += 2) {
    size += x_[i + 1] - x_[i];
  }
  return size;
}

bool Interval1D::HasOverlap(const Interval1D& o) const {
  // TODO(@monkeynova): Return early when possible.
  return !Intersect(o).empty();
}

Interval1D Interval1D::Translate(int64_t delta) const {
  Interval1D ret = *this;
  for (int64_t& x : ret.x_) {
    x += delta;
  }
  return ret;
}

Interval1D Interval1D::Union(const Interval1D& o) const {
  return Merge(o, [](bool left, bool right) { return left || right; });
}

Interval1D Interval1D::Intersect(const Interval1D& o) const {
  return Merge(o, [](bool left, bool right) { return left && right; });
}

Interval1D Interval1D::Minus(const Interval1D& o) const {
  return Merge(o, [](bool left, bool right) { return left && !right; });
}

Interval1D Interval1D::Merge(
    const Interval1D& o, absl::FunctionRef<bool(bool, bool)> merge_fn) const {
  auto left_it = x_.begin();
  bool left_on = false;
  auto right_it = o.x_.begin();
  bool right_on = false;
  bool ret_on = false;
  Interval1D ret;
  while (left_it != x_.end() && right_it != o.x_.end()) {
    int64_t lval = *left_it;
    int64_t rval = *right_it;
    int64_t t;
    if (lval <= rval) {
      t = lval;
      left_on = !left_on;
      ++left_it;
    }
    if (lval >= rval) {
      t = rval;
      right_on = !right_on;
      ++right_it;
    }
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x_.push_back(t);
      ret_on = !ret_on;
    }
  }
  while (left_it != x_.end()) {
    int64_t t = *left_it;
    left_on = !left_on;
    ++left_it;
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x_.push_back(t);
      ret_on = !ret_on;
    }
  }
  while (right_it != o.x_.end()) {
    int64_t t = *right_it;
    right_on = !right_on;
    ++right_it;
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x_.push_back(t);
      ret_on = !ret_on;
    }
  }
  VLOG(3) << *this << " + " << o << " = " << ret;
  return ret;
}

}  // namespace advent_of_code