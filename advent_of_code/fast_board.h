#ifndef ADVENT_OF_CODE_FAST_BOARD_H
#define ADVENT_OF_CODE_FAST_BOARD_H

#include <array>
#include <vector>

#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

class FastBoard {
 public:
  enum Dir {
    kNorth = 0,
    kSouth = 1,
    kWest = 2,
    kEast = 3,
  };

  static constexpr std::array<Dir, 4> kFourDirs = {kNorth, kSouth, kWest,
                                                     kEast};
  static constexpr std::array<Dir, 4> kRotateLeft = {kWest, kEast, kSouth,
                                                     kNorth};
  static constexpr std::array<Dir, 4> kRotateRight = {kEast, kWest, kNorth,
                                                      kSouth};
  static constexpr std::array<Dir, 4> kReverse = {kSouth, kNorth, kEast, kWest};

  class iterator;

  class Point {
   public:
    Point(const Point&) = default;
    Point& operator=(const Point&) = default;

    std::strong_ordering operator<=>(const Point& o) const = default;

    template <typename H>
    friend H AbslHashValue(H h, Point p) {
      return H::combine(std::move(h), p.idx_);
    }

    template <typename Sink>
    friend void AbslStringify(Sink& sink, Point p) {
      absl::Format(&sink, "FastPoint(%d)", p.idx_);
    }

   private:
    friend class FastBoard;
    friend class FastBoard::iterator;

    explicit Point(int idx) : idx_(idx) {}

    int idx_;
  };

  struct PointDir {
    Point p;
    Dir d;

    bool MoveAndCheckBoard(const FastBoard& fb) {
      p = fb.Add(p, d);
      return fb.OnBoard(p);
    }
    void Move(const FastBoard& fb) { p = fb.Add(p, d); }
  };

  template <typename Storage>
  class PointMap {
   public:
    PointMap(const FastBoard& b, Storage init) : map_(b.size_, init) {}

    template <typename T = Storage, typename = std::enable_if_t<sizeof(T) <= 8>>
    Storage Get(Point p) const {
      return map_[p.idx_];
    }

    template <typename T = Storage,
              typename = std::enable_if_t<(sizeof(T) > 8)>>
    Storage& Get(Point p) {
      return map_[p.idx_];
    }

    template <typename T = Storage,
              typename = std::enable_if_t<(sizeof(T) > 8)>>
    const Storage& Get(Point p) const {
      return map_[p.idx_];
    }

    void Set(Point p, Storage s) { map_[p.idx_] = s; }

   private:
    friend class FastBoard;
    std::vector<Storage> map_;
  };

  template <typename Storage>
  class PointDirMap {
   public:
    PointDirMap(const FastBoard& b, Storage init) : map_(4 * b.size_, init) {}

    Storage Get(PointDir pd) const { return map_[pd.p.idx_ * 4 + pd.d]; }
    Storage Get(Point p, Dir d) const { return map_[p.idx_ * 4 + d]; }
    Storage& GetMutable(PointDir pd) { return map_[pd.p.idx_ * 4 + pd.d]; }
    Storage& GetMutable(Point p, Dir d) { return map_[p.idx_ * 4 + d]; }
    void Set(PointDir pd, Storage s) { map_[pd.p.idx_ * 4 + pd.d] = s; }
    void Set(Point p, Dir d, Storage s) { map_[p.idx_ * 4 + d] = s; }

   private:
    friend class FastBoard;
    std::vector<Storage> map_;
  };

  template <typename Storage>
  class PointHalfDirMap {
   public:
    PointHalfDirMap(const FastBoard& b, Storage init)
        : map_(2 * b.size_, init) {}

    Storage Get(PointDir pd) const { return map_[pd.p.idx_ * 2 + pd.d / 2]; }
    void Set(PointDir pd, Storage s) { map_[pd.p.idx_ * 2 + pd.d / 2] = s; }

   private:
    friend class FastBoard;
    std::vector<Storage> map_;
  };

  template <typename Storage>
  class PointDirExtraMap {
   public:
    PointDirExtraMap(const FastBoard& b, Storage init)
        : stride_(b.stride_), map_(4 * (b.size_ + 2 * stride_), init) {}

    Storage Get(PointDir pd) const {
      return map_[(pd.p.idx_ + stride_) * 4 + pd.d];
    }
    void Set(PointDir pd, Storage s) {
      map_[(pd.p.idx_ + stride_) * 4 + pd.d] = s;
    }

   private:
    friend class FastBoard;
    int stride_;
    std::vector<Storage> map_;
  };

  class iterator {
   public:
    iterator& operator++() {
      ++idx_;
      if (!b_.OnBoard(Point(idx_))) ++idx_;
#ifdef _WIN32
      if (!b_.OnBoard(Point(idx_))) ++idx_;
#endif
      return *this;
    }
    Point operator*() const {
      return Point(idx_);
    }
    bool operator==(const iterator& o) const {
      return idx_ == o.idx_;
    }
   private:
    friend class FastBoard;
    iterator(const FastBoard& b, int idx) : b_(b), idx_(idx) {}
  
    const FastBoard& b_;
    int idx_;
  };

  explicit FastBoard(const ImmutableCharBoard& b)
      : base_(b.row(0).data()),
        stride_(b.row(1).data() - b.row(0).data()),
        size_(b.height() * stride_),
        dir_delta_({-stride_, stride_, -1, 1}),
        on_board_(size_, true) {
    for (int i = stride_ - 1; i < size_; i += stride_) {
      if constexpr (kLineEndWidth == 2) {
        on_board_[i - 1] = false;
      }
      on_board_[i] = false;
    }
  }

  iterator begin() const {
    return iterator(*this, 0);
  }
  iterator end() const {
    return iterator(*this, size_);
  }

  Point FindUnique(char c) {
    for (int i = 0; i < size_; ++i) {
      if (base_[i] == c) return Point(i);
    }
    return Point(-1);
  }

  Point From(advent_of_code::Point in) const {
    return Point(in.y * stride_ + in.x);
  }

  advent_of_code::Point To(Point p) const {
    return advent_of_code::Point{.x = p.idx_ % stride_, .y = p.idx_ / stride_};
  }

  bool OnBoard(Point p) const {
    if (p.idx_ < 0) return false;
    if (p.idx_ >= size_) return false;
    return on_board_[p.idx_];
  }

  Point Add(Point p, Dir d) const { return Point(p.idx_ + dir_delta_[d]); }

  char operator[](Point p) const { return base_[p.idx_]; }

 private:
  const char* base_;
  int stride_;
  int size_;
  std::array<int, 4> dir_delta_;
  std::vector<bool> on_board_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_FAST_BOARD_H
