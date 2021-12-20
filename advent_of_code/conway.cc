#include "advent_of_code/conway.h"

#include "glog/logging.h"

namespace advent_of_code {

absl::Status Conway::Advance() {
  CharBoard new_b(b_.width() + 2, b_.height() + 2);
  for (Point p : new_b.range()) {
    int bv = 0;
    for (Point d :
         {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast,
          Cardinal::kWest, Cardinal::kOrigin, Cardinal::kEast,
          Cardinal::kSouthWest, Cardinal::kSouth, Cardinal::kSouthEast}) {
      Point src = p + Cardinal::kNorthWest + d;
      bv *= 2;
      char src_val = b_.OnBoard(src) ? b_[src] : fill_;
      if (src_val == '#') {
        ++bv;
      } else if (src_val != '.') {
        return absl::InternalError("Bad board");
      }
    }
    if (bv >= lookup_.size()) return absl::InternalError("Bad lookup size");
    new_b[p] = lookup_[bv];
  }
  b_ = std::move(new_b);
  if (fill_ == '.') {
    fill_ = lookup_[0];
  } else {
    if (fill_ != '#') return absl::InternalError("Bad board");
    fill_ = lookup_[511];
  }
  return absl::OkStatus();
}

absl::Status Conway::AdvanceN(int count) {
  for (int i = 0; i < count; ++i) {
    if (auto st = Advance(); !st.ok()) return st;
  }
  return absl::OkStatus();
}

}  // namespace advent_of_code
