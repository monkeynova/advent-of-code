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
  for (int Point::* dim : {&Point::x, &Point::y}) {
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

// See https://en.wikipedia.org/wiki/Shoelace_formula for a description of the
// area calculation
// To consider the `count_perimeter` parameter consider the following:
//
//  ### Rectangle: {0,0} - {2,2}
//  ### Inner Area: 1; Outer Area: 9; Shoelace Area: 4
//  ### Perimeter: 8; IA = SA - P/2  + 1; OA = SA + P/2 + 1
int64_t LoopArea(const std::vector<Point>& loop, bool count_perimeter) {
  int64_t a = static_cast<int64_t>(loop[0].x) * (loop[1].y - loop.back().y);
  int64_t p = (loop[0] - loop.back()).dist();

  for (int i = 1; i < loop.size() - 1; ++i) {
    a += static_cast<int64_t>(loop[i].x) * (loop[i + 1].y - loop[i - 1].y);
    p += (loop[i] - loop[i - 1]).dist();
  }
  {
    int i = loop.size() - 1;
    p += (loop[i] - loop[i - 1]).dist();
  }
  a += static_cast<int64_t>(loop.back().x) *
       (loop[0].y - loop[loop.size() - 2].y);
  if (a < 0) a = -a;
  CHECK_EQ(a % 2, 0);
  CHECK_EQ(p % 2, 0);
  if (count_perimeter) return a / 2 + p / 2 + 1;
  return a / 2 - p / 2 + 1;
}

}  // namespace advent_of_code