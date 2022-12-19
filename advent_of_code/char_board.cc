#include "advent_of_code/char_board.h"

#include "absl/strings/str_cat.h"

namespace advent_of_code {

absl::StatusOr<CharBoard> CharBoard::SubBoard(PointRectangle sub_range) const {
  if (!OnBoard(sub_range.min)) {
    return absl::InvalidArgumentError(
        absl::StrCat("SubBoard: min not on board: ", sub_range));
  }
  if (!OnBoard(sub_range.max)) {
    return absl::InvalidArgumentError(
        absl::StrCat("SubBoard: max not on board: ", sub_range));
  }
  CharBoard out(sub_range.max.x - sub_range.min.x + 1,
                sub_range.max.y - sub_range.min.y + 1);
  for (int i = sub_range.min.y; i <= sub_range.max.y; ++i) {
    memcpy(out.stride(i - sub_range.min.y), stride(i) + sub_range.min.x,
           out.width());
  }
  return out;
}

Point CharBoard::TorusPoint(Point p) const {
  Point ret = {p.x % width(), p.y % height()};
  if (ret.x < 0) ret.x += width();
  if (ret.y < 0) ret.y += height();
  return ret;
}

absl::flat_hash_set<Point> CharBoard::Find(char c) const {
  absl::flat_hash_set<Point> ret;
  for (Point p : range())
    if (at(p) == c) ret.insert(p);
  return ret;
}

int CharBoard::CountChar(char test) const {
  int count = 0;
  for (int i = 0; i < buf_.size(); ++i) {
    if (i % stride_ == stride_ - 1) continue;
    if (buf_[i] == test) ++count;
  }
  return count;
}

}  // namespace advent_of_code