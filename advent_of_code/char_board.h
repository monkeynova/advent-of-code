#ifndef ADVENT_OF_CODE_CHAR_BOARD_H
#define ADVENT_OF_CODE_CHAR_BOARD_H

#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

// Stores a two dimension board of ascii characters as is commonly used within
// AOC puzzles. A board has a fixed width and height upon construction but
// mutable cells within the board. While any characters can be stored, the
// most common form of use in AOC stores '#' as "on" and '.' as "off", and as
// such methods like 'Draw' and 'CountOn' lean into those defaults.
// An example board might look like the following:
//
// #######
// #.#...#
// #.###.#
// #..#..#
// ## ..##
// #######
class CharBoard {
 public:
  class const_iterator {
   public:
    static const_iterator begin(const CharBoard& board) {
      return const_iterator(board);
    }
    static const_iterator end(const CharBoard& board) {
      const_iterator ret(board);
      ret.it_ = ret.range_.end();
      return ret;
    }

    const_iterator& operator++() {
      ++it_;
      return *this;
    }

    bool operator==(const const_iterator& o) const {
      return &board_ == &o.board_ && it_ == o.it_;
    }
    bool operator!=(const const_iterator& o) const { return !operator==(o); }

    std::pair<Point, char> operator*() const { return std::make_pair(*it_, board_[*it_]); }

   private:
    const_iterator(const CharBoard& board)
     : board_(board), range_(board.range()), it_(range_.begin()) {}

    const CharBoard& board_;
    PointRectangle range_;
    PointRectangle::iterator it_;
  };

  const_iterator begin() const { return const_iterator::begin(*this); }
  const_iterator end() const { return const_iterator::end(*this); }

  // Constructs a CharBoard by iterating over `in` and adding one row to the
  // board for each iteration (line). Returns an error if board construction
  // fails (for example if not all lines have the same size).
  // `in` must allow multiple iterations and the iteration value must be
  // implicitly castable to absl::string_view.
  template <typename Container>
  static absl::StatusOr<CharBoard> Parse(const Container& in);

  // Sets each point in `points` in the board to '#'. If `bounds_ret` is not
  // nullptr, it contains the (inclusive) axis aligned bounding box for those
  // points.
  template <typename Container>
  static CharBoard Draw(const Container& points,
                        PointRectangle* bound_ret = nullptr) {
    PointRectangle bounds = PointRectangle::Bounding(points);
    CharBoard ret(bounds);
    for (Point p : points) ret[p - bounds.min] = '#';
    if (bound_ret != nullptr) *bound_ret = bounds;
    return ret;
  }

  CharBoard() : CharBoard(0, 0) {}

  CharBoard(int width, int height)
      : stride_(width + 1), buf_(stride_ * height, '.') {
    for (int i = stride_ - 1; i < buf_.size(); i += stride_) {
      buf_[i] = '\n';
    }
  }

  explicit CharBoard(PointRectangle r)
      : CharBoard(r.max.x >= r.min.x ? r.max.x - r.min.x + 1 : 0,
                  r.max.y >= r.min.y ? r.max.y - r.min.y + 1 : 0) {}

  CharBoard(const CharBoard&) = default;
  CharBoard& operator=(const CharBoard&) = default;

  int height() const { return stride_ == 1 ? 0 : buf_.size() / stride_; }
  int width() const { return stride_ - 1; }

  // Returns the (inclusive) bounds for the the board. This is use for, among
  // other things, for (Point p : board.range()) { ... }.
  PointRectangle range() const {
    return PointRectangle{.min = {.x = 0, .y = 0},
                          .max = {.x = width() - 1, .y = height() - 1}};
  }

  // Returns true if `p` is a valid point on the board.
  bool OnBoard(Point p) const {
    if (p.y < 0) return false;
    if (p.y >= height()) return false;
    if (p.x < 0) return false;
    if (p.x >= width()) return false;
    return true;
  }

  // at, set and operator[] all provide pointwise getter and setters for the
  // individual cells on the board. It is undefined behavior to call with a
  // point outside of range().
  char at(Point p) const { return buf_[p.y * stride_ + p.x]; }
  void set(Point p, char c) { buf_[p.y * stride_ + p.x] = c; }

  char operator[](Point p) const { return buf_[p.y * stride_ + p.x]; }
  char& operator[](Point p) { return buf_[p.y * stride_ + p.x]; }

  // Retuns the set of points p such that at(p) == c.
  absl::flat_hash_set<Point> Find(char c) const;

  // Returns Find(test).size(), though doesn't use the temporary space.
  int CountChar(char test) const;
  int CountOn() const { return CountChar('#'); }

  // Returns the canonical value for `p` assuming the board is a torus (that
  // is, assuming the edges wrap around like in Asteroids).
  Point TorusPoint(Point p) const;

  // Returns a CharBoard which represent the subset of `this` defined in
  // `sub_range`. If any part of `sub_range` is off of the board, returns an
  // error.
  absl::StatusOr<CharBoard> SubBoard(PointRectangle sub_range) const;

  // A board is hashable, so implements AbslHashValue and operator==.
  template <typename H>
  friend H AbslHashValue(H h, const CharBoard& b) {
    return H::combine(std::move(h), b.buf_);
  }
  bool operator==(const CharBoard& o) const { return buf_ == o.buf_; }
  bool operator!=(const CharBoard& o) const { return !operator==(o); }

  // For debugging purposes, implements stringification.
  template <typename Sink>
  friend void AbslStringify(Sink& sink, const CharBoard& b) {
    absl::Format(&sink, "%s", b.buf_);
  }

 private:
  char* stride(int y) { return buf_.data() + stride_ * y; }
  const char* stride(int y) const { return buf_.data() + stride_ * y; }

  // The board is implemented as a single contiguous buffer where each row is
  // `stride_` bytes further in. `stride_` is width() + 1 so that we can
  // include '\n' on each line and make stringification trivial.
  int stride_;
  std::string buf_;
};

template <typename Container>
absl::StatusOr<CharBoard> CharBoard::Parse(const Container& in) {
  // We do a 2-pass read of 'in' to allow Container to not support
  // empty()/operator[] calls. This is necessary to allow the value returned
  // by absl::StrSplit to be passed directly in.
  static_assert(std::is_same_v<absl::string_view,
                               decltype(absl::string_view(*in.begin()))>,
                "Container must iterate over absl::string_view");
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