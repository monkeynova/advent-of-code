#ifndef ADVENT_OF_CODE_CHAR_BOARD_H
#define ADVENT_OF_CODE_CHAR_BOARD_H

#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

class CharBoard {
 public:
  template <typename Container>
  static absl::StatusOr<CharBoard> Parse(const Container& in);

  CharBoard() : CharBoard(0, 0) {}

  CharBoard(int width, int height)
      : stride_(width + 1), buf_(stride_ * height, '.') {
    for (int i = stride_ - 1; i < buf_.size(); i += stride_) {
      buf_[i] = '\n';
    }
  }

  explicit CharBoard(PointRectangle r)
      : CharBoard(r.max.x - r.min.x + 1, r.max.y - r.min.y + 1) {}

  CharBoard(const CharBoard&) = default;
  CharBoard& operator=(const CharBoard&) = default;

  int CountOn() const { return CountChar('#'); }
  int CountChar(char test) const;

  int height() const { return stride_ == 1 ? 0 : buf_.size() / stride_; }
  int width() const { return stride_ - 1; }

  PointRectangle range() const {
    return PointRectangle{.min = {.x = 0, .y = 0},
                          .max = {.x = width() - 1, .y = height() - 1}};
  }

  absl::flat_hash_set<Point> Find(char c) const;

  Point TorusPoint(Point p) const;

  char at(Point p) const { return buf_[p.y * stride_ + p.x]; }
  void set(Point p, char c) { buf_[p.y * stride_ + p.x] = c; }

  char operator[](Point p) const { return buf_[p.y * stride_ + p.x]; }
  char& operator[](Point p) { return buf_[p.y * stride_ + p.x]; }

  std::string AsString() const { return buf_; }

  bool OnBoard(Point p) const {
    if (p.y < 0) return false;
    if (p.y >= height()) return false;
    if (p.x < 0) return false;
    if (p.x >= width()) return false;
    return true;
  }

  absl::StatusOr<CharBoard> SubBoard(PointRectangle sub_range) const;

  template <typename H>
  friend H AbslHashValue(H h, const CharBoard& b) {
    return H::combine(std::move(h), b.buf_);
  }

  bool operator==(const CharBoard& o) const { return buf_ == o.buf_; }
  bool operator!=(const CharBoard& o) const { return !operator==(o); }

  friend std::ostream& operator<<(std::ostream& out, const CharBoard& b) {
    return out << b.buf_;
  }

 private:
  char* stride(int y) { return buf_.data() + stride_ * y; }
  const char* stride(int y) const { return buf_.data() + stride_ * y; }

  int stride_;
  std::string buf_;
};

template <typename Container>
absl::StatusOr<CharBoard> CharBoard::Parse(const Container& in) {
  // We do a 2-pass read of 'in' to allow Container to not support
  // empty()/operator[] calls. This is necessary to allow the value returned
  // by absl::StrSplit to be passed directly in.
  int width = -1;
  int height = 0;
  for (absl::string_view line : in) {
    ++height;
    if (width == -1) {
      width = line.size();
    } else if (width != line.size()) {
      return absl::InvalidArgumentError("Inconsistent width");
    }
  }
  if (height == 0) return CharBoard(0, 0);

  CharBoard ret(width, height);
  char* dst = ret.buf_.data();
  for (absl::string_view line : in) {
    memcpy(dst, line.data(), ret.width());
    dst += ret.stride_;
  }
  return ret;
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CHAR_BOARD_H