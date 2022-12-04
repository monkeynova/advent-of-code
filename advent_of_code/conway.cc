#include "advent_of_code/conway.h"

#include "absl/log/log.h"

namespace advent_of_code {

// static
std::string Conway::DefaultLookup() {
  std::string ret(512, '.');
  for (int i = 0; i < 512; ++i) {
    bool alive = false;
    int neighbors = 0;
    for (int bit = 0; bit < 9; ++bit) {
      if (i & (1 << bit)) {
        if (bit == 4) {
          alive = true;
        } else {
          ++neighbors;
        }
      }
    }
    if (neighbors == 3) {
      ret[i] = '#';
    } else if (alive && neighbors == 2) {
      ret[i] = '#';
    }
  }
  return ret;
}

absl::Status Conway::Advance() {
  Point dst_to_src = infinite_ ? Cardinal::kNorthWest : Cardinal::kOrigin;
  int buffer = infinite_ ? 2 : 0;
  CharBoard new_b(b_.width() + buffer, b_.height() + buffer);
  for (Point p : new_b.range()) {
    int bv = 0;
    for (Point d :
         {Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast,
          Cardinal::kWest, Cardinal::kOrigin, Cardinal::kEast,
          Cardinal::kSouthWest, Cardinal::kSouth, Cardinal::kSouthEast}) {
      Point src = p + d + dst_to_src;
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
  if (!infinite_) {
    b_ = std::move(new_b);
    return absl::OkStatus();
  }

  if (fill_ == '.') {
    fill_ = lookup_[0];
  } else if (fill_ == '#') {
    if (511 >= lookup_.size()) return absl::InternalError("Bad lookup size");
    fill_ = lookup_[511];
  } else {
    return absl::InternalError("Bad board fill");
  }

  PointRectangle non_fill_rect = new_b.range();
  {
    bool top_is_fill = true;
    bool bottom_is_fill = true;
    for (int x = 0; x < new_b.width(); ++x) {
      if (new_b[{x, 0}] != fill_) top_is_fill = false;
      if (new_b[{x, new_b.height() - 1}] != fill_) bottom_is_fill = false;
    }
    if (top_is_fill) ++non_fill_rect.min.x;
    if (bottom_is_fill) --non_fill_rect.max.x;
  }
  {
    bool left_is_fill = true;
    bool right_is_fill = true;
    for (int y = 0; y < new_b.height(); ++y) {
      if (new_b[{0, y}] != fill_) left_is_fill = false;
      if (new_b[{new_b.height() - 1, y}] != fill_) right_is_fill = false;
    }
    if (left_is_fill) ++non_fill_rect.min.y;
    if (right_is_fill) --non_fill_rect.max.y;
  }
  if (non_fill_rect == new_b.range()) {
    b_ = std::move(new_b);
    return absl::OkStatus();
  }

  absl::StatusOr<CharBoard> non_fill = new_b.SubBoard(non_fill_rect);
  if (!non_fill.ok()) return non_fill.status();
  b_ = std::move(*non_fill);
  return absl::OkStatus();
}

absl::Status Conway::AdvanceN(int count) {
  for (int i = 0; i < count; ++i) {
    if (auto st = Advance(); !st.ok()) return st;
  }
  return absl::OkStatus();
}

}  // namespace advent_of_code
