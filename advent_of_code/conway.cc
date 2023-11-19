#include "advent_of_code/conway.h"

#include "absl/log/log.h"

namespace advent_of_code {

// static
std::string InfiniteConway::DefaultLookup() {
  std::string ret(512, '.');
  for (uint i = 0; i < 512; ++i) {
    bool alive = i & (1 << 4);
    int neighbors = absl::popcount(i & ~(1 << 4));
    if (neighbors == 3) {
      ret[i] = '#';
    } else if (alive && neighbors == 2) {
      ret[i] = '#';
    }
  }
  return ret;
}

absl::Status InfiniteConway::Advance() {
  if (lookup_.size() != 512) {
    return absl::InternalError(
        absl::StrCat("Bad lookup size:", lookup_.size()));
  }
  static const std::array<Point, 9> kDirs = {
    Cardinal::kNorthWest, Cardinal::kNorth, Cardinal::kNorthEast,
    Cardinal::kWest, Cardinal::kOrigin, Cardinal::kEast,
    Cardinal::kSouthWest, Cardinal::kSouth, Cardinal::kSouthEast};
  
  CharBoard new_b(b_.width() + 2, b_.height() + 2);
  for (Point p : new_b.range()) {
    int bv = 0;
    for (Point d : kDirs) {
      Point src = p + d + Cardinal::kNorthWest;
      bv <<= 1;
      switch (char test = b_.OnBoard(src) ? b_[src] : fill_) {
        case '#':
          bv |= 1;
          continue;
        case '.':
          continue;
        default: {
          return absl::InternalError(absl::StrCat(
              "Bad board character: '", std::string_view(&test, 1), "'"));
        }
      }
    }
    new_b[p] = lookup_[bv];
  }

  if (fill_ == '.') {
    fill_ = lookup_[0];
  } else if (fill_ == '#') {
    fill_ = lookup_[511];
  }

  ASSIGN_OR_RETURN(b_, PruneFill(std::move(new_b)));
  return absl::OkStatus();
}

absl::StatusOr<CharBoard> InfiniteConway::PruneFill(CharBoard new_b) {
  PointRectangle non_fill_rect = new_b.range();
  for (bool is_fill = true;
       is_fill && non_fill_rect.min.y < non_fill_rect.max.y;) {
    for (int x = non_fill_rect.min.x; x < non_fill_rect.max.x; ++x) {
      if (new_b[{x, non_fill_rect.min.y}] != fill_) is_fill = false;
    }
    if (is_fill) ++non_fill_rect.min.y;
  }
  for (bool is_fill = true;
       is_fill && non_fill_rect.min.y < non_fill_rect.max.y;) {
    for (int x = non_fill_rect.min.x; x < non_fill_rect.max.x; ++x) {
      if (new_b[{x, non_fill_rect.max.y}] != fill_) is_fill = false;
    }
    if (is_fill) --non_fill_rect.max.y;
  }
  for (bool is_fill = true;
       is_fill && non_fill_rect.min.x < non_fill_rect.max.x;) {
    for (int y = non_fill_rect.min.y; y < non_fill_rect.max.y; ++y) {
      if (new_b[{non_fill_rect.min.x, y}] != fill_) is_fill = false;
    }
    if (is_fill) ++non_fill_rect.min.x;
  }
  for (bool is_fill = true;
       is_fill && non_fill_rect.min.x < non_fill_rect.max.x;) {
    for (int y = non_fill_rect.min.y; y < non_fill_rect.max.y; ++y) {
      if (new_b[{non_fill_rect.max.x, y}] != fill_) is_fill = false;
    }
    if (is_fill) --non_fill_rect.max.x;
  }
  if (non_fill_rect == new_b.range()) {
    return new_b;
  }

  return new_b.SubBoard(non_fill_rect);
}

absl::Status InfiniteConway::AdvanceN(int count) {
  for (int i = 0; i < count; ++i) {
    if (auto st = Advance(); !st.ok()) return st;
  }
  return absl::OkStatus();
}

}  // namespace advent_of_code
