#include "advent_of_code/interval.h"

#include "advent_of_code/vlog.h"

namespace advent_of_code {

int Interval1D::Size() const {
  int size = 0;
  for (int i = 0; i < x_.size(); i += 2) {
    size += x_[i + 1] - x_[i];
  }
  return size;
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
    int lval = *left_it;
    int rval = *right_it;
    int t;
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
    int t = *left_it;
    left_on = !left_on;
    ++left_it;
    if (ret_on != merge_fn(left_on, right_on)) {
      ret.x_.push_back(t);
      ret_on = !ret_on;
    }
  }
  while (right_it != o.x_.end()) {
    int t = *right_it;
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