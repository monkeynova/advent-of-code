#include "advent_of_code/point.h"

#include "absl/log/check.h"

namespace advent_of_code {

void PointRectangle::SetDifference(const PointRectangle& o,
                                   std::vector<PointRectangle>* out) const {
  if (!Overlaps(o)) {
    out->push_back(*this);
    return;
  }

  PointRectangle overlap = *this;
  std::vector<PointRectangle> ret;
  // Cleave off sub-rectangles that cannot overlap with o, until a fully
  // contained sub-rectangle is all that remains, and then discard it.
  for (int Point::*dim : {&Point::x, &Point::y}) {
    if (o.min.*dim > overlap.min.*dim && o.min.*dim <= overlap.max.*dim) {
      PointRectangle cleave = overlap;
      cleave.max.*dim = o.min.*dim - 1;
      out->push_back(cleave);
      overlap.min.*dim = o.min.*dim;
    }
    if (o.max.*dim >= overlap.min.*dim && o.max.*dim < overlap.max.*dim) {
      PointRectangle cleave = overlap;
      cleave.min.*dim = o.max.*dim + 1;
      out->push_back(cleave);
      overlap.max.*dim = o.max.*dim;
    }
  }
  DCHECK(o.FullyContains(overlap));
}

void Cube::SetDifference(const Cube& o, std::vector<Cube>* out) const {
  if (!Overlaps(o)) {
    out->push_back(*this);
    return;
  }

  Cube overlap = *this;
  std::vector<Cube> ret;
  // Cleave off sub-cubes that cannot overlap with o, until a fully contained
  // sub-cube is all that remains, and then discard it.
  for (int Point3::*dim : {&Point3::x, &Point3::y, &Point3::z}) {
    if (o.min.*dim > overlap.min.*dim && o.min.*dim <= overlap.max.*dim) {
      Cube cleave = overlap;
      cleave.max.*dim = o.min.*dim - 1;
      out->push_back(cleave);
      overlap.min.*dim = o.min.*dim;
    }
    if (o.max.*dim >= overlap.min.*dim && o.max.*dim < overlap.max.*dim) {
      Cube cleave = overlap;
      cleave.min.*dim = o.max.*dim + 1;
      out->push_back(cleave);
      overlap.max.*dim = o.max.*dim;
    }
  }
  DCHECK(o.FullyContains(overlap));
}

const std::vector<Orientation3>& Orientation3::All() {
  static std::vector<Orientation3> kMemo;
  if (!kMemo.empty()) return kMemo;

  for (Point3 x_hat : Cardinal3::kNeighborDirs) {
    if (x_hat.dist() != 1) continue;
    for (Point3 y_hat : Cardinal3::kNeighborDirs) {
      if (y_hat.dist() != 1) continue;
      if (y_hat == x_hat) continue;
      if (y_hat == -x_hat) continue;
      kMemo.push_back(Orientation3(x_hat, y_hat));
    }
  }
  CHECK_EQ(kMemo.size(), 24);
  return kMemo;
}

}  // namespace advent_of_code