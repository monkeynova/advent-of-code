#include "advent_of_code/char_board.h"

#include "absl/strings/str_cat.h"

namespace advent_of_code {

template <bool is_mutable>
absl::StatusOr<CharBoard> CharBoardBase<is_mutable>::SubBoard(
    PointRectangle sub_range) const {
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

template <bool is_mutable>
Point CharBoardBase<is_mutable>::TorusPoint(Point p) const {
  Point ret = {p.x % width(), p.y % height()};
  if (ret.x < 0) ret.x += width();
  if (ret.y < 0) ret.y += height();
  return ret;
}

template <bool is_mutable>
absl::flat_hash_set<Point> CharBoardBase<is_mutable>::Find(char c) const {
  absl::flat_hash_set<Point> ret;
  CHECK_NE(c, '\n');
  for (int i = 0; i < buf_.size(); ++i) {
    if (buf_[i] == c) {
      ret.insert({.x = i % stride_, .y = i / stride_});
    }
  }
  return ret;
}

template <bool is_mutable>
std::vector<Point> CharBoardBase<is_mutable>::FindVec(char c) const {
  std::vector<Point> ret;
  CHECK_NE(c, '\n');
  for (int i = 0; i < buf_.size(); ++i) {
    if (buf_[i] == c) {
      ret.push_back({.x = i % stride_, .y = i / stride_});
    }
  }
  return ret;
}

template <bool is_mutable>
absl::StatusOr<Point> CharBoardBase<is_mutable>::FindUnique(char c) const {
  CHECK_NE(c, '\n');
  bool found = false;
  Point ret;
  for (int i = 0; i < buf_.size(); ++i) {
    if (buf_[i] == c) {
      if (found) return absl::InvalidArgumentError("Not unique");
      ret = {.x = i % stride_, .y = i / stride_};
      found = true;
    }
  }
  if (!found) return absl::NotFoundError("Not found");
  return ret;
}

template <bool is_mutable>
absl::flat_hash_set<Point> CharBoardBase<is_mutable>::Find(
    std::bitset<256> charset) const {
  absl::flat_hash_set<Point> ret;
  CHECK(!charset['\n']);
  for (int i = 0; i < buf_.size(); ++i) {
    if (charset[buf_[i]]) {
      ret.insert({.x = i % stride_, .y = i / stride_});
    }
  }
  return ret;
}

template <bool is_mutable>
int CharBoardBase<is_mutable>::CountChar(char test) const {
  int count = 0;
  for (int i = 0; i < buf_.size(); ++i) {
    if (i % stride_ == stride_ - 1) continue;
    if (buf_[i] == test) ++count;
  }
  return count;
}

template class CharBoardBase<false>;
template class CharBoardBase<true>;

}  // namespace advent_of_code