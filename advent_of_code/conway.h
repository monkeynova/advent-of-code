#ifndef ADVENT_OF_CODE_CONWAY_H
#define ADVENT_OF_CODE_CONWAY_H

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
// Stores the state of a current iteration of Conway's game of life on the
// non-trivial subset of an infinite CharBoard.
class InfiniteConway {
 public:
  // Returns the classic Conway game of life transition table which causes
  // a location to be alive if it has exactly 3 live neighbors or if it is
  // already alive and has exactly 2 live neighbors.
  static std::string DefaultLookup();

  // Initializes a game-of-life. The initial board `b` must contain only the
  // values '.' and '#' representing 'dead' and 'live' stats resepctively,
  // Lookup must be a 512 byte string such that if we consider the live bits
  // of the 9 blocks centered on a location as a lookup we return the value of
  // the location in the next iteration.
  // For example:
  // #.#
  // .#. -> 101'010'110 -> 344
  // ##.
  InfiniteConway(CharBoard b, std::string lookup = DefaultLookup())
      : b_(std::move(b)), lookup_(std::move(lookup)) {}

  absl::StatusOr<int64_t> CountLive() const {
    if (fill_ != '.') {
      return absl::FailedPreconditionError("Live count is infinite");
    }
    return board().CountOn();
  }

  CharBoard& board() { return b_; }
  const CharBoard& board() const { return b_; }

  // Move forward one iteration of the board (returns an error if lookup isn't
  // well formed, or if the board contains chars other than '.' or '#').
  absl::Status Advance();

  // Move forward `count` interations of the board.
  absl::Status AdvanceN(int count);

 private:
  // Looks for `fill_` in the outer frame of `b` and if either the outer rows
  // or columns are entirely `fill_` returns a subboard with those rows or
  // columns removed.
  absl::StatusOr<CharBoard> PruneFill(CharBoard b);

  CharBoard b_;
  std::string lookup_;
  char fill_ = '.';
};

template <typename Storage, int64_t size>
class ConwayMultiSet {
 public:
  explicit ConwayMultiSet(std::array<absl::flat_hash_set<Storage>, size> sets)
      : sets_(std::move(sets)) {}

  virtual ~ConwayMultiSet() = default;

  int64_t CountState(int state) const { return sets_[state].size(); }

  virtual std::string Draw() const { return ""; }
  virtual std::vector<Storage> Neighbors(const Storage& s) const = 0;
  virtual int NextState(int state, std::array<int, size> neighbors) const = 0;

  virtual bool SetsAreComplete() const { return false; }

  virtual bool Advance() {
    if (step_ == 0) AuditNextState();
    VLOG(2) << "Board[" << step_ << "]\n" << *this;
    ++step_;

    absl::flat_hash_map<Storage, std::array<int, size>> neighbor_map;
    for (int set_idx = 0; set_idx < size; ++set_idx) {
      for (const Storage& s : sets_[set_idx]) {
        for (Storage n : Neighbors(s)) {
          ++neighbor_map[n][set_idx];
        }
      }
    }

    auto add_set_size = [](int a, const absl::flat_hash_set<Storage>& s) {
      return a + s.size();
    };
    int pre_total = absl::c_accumulate(sets_, 0, add_set_size);
    std::array<absl::flat_hash_set<Storage>, size> next;
    bool unchanged = true;
    for (const auto& [s, n] : neighbor_map) {
      int cur_state = CurState(s);
      int next_state = NextState(cur_state, n);
      if (next_state >= 0) {
        next[next_state].insert(s);
      }
      unchanged &= cur_state == next_state;
    }
    if (unchanged) {
      for (int i = 0; i < size; ++i) {
        unchanged &= sets_[i].size() == next[i].size();
      }
    }
    int post_total = absl::c_accumulate(sets_, 0, add_set_size);
    if (SetsAreComplete()) {
      CHECK_EQ(pre_total, post_total);
    }
    if (unchanged) {
      return false;
    }
    sets_ = std::move(next);
    return true;
  }

  bool AdvanceN(int64_t n) {
    for (int i = 0; i < n; ++i) {
      if (!Advance()) return false;
    }
    return n > 0;
  }

  template <typename Sink>
  friend void AbslStringify(Sink& sink, const ConwayMultiSet& c) {
    sink.Append(c.Draw());
  }

 protected:
  const std::array<absl::flat_hash_set<Storage>, size>& sets() const {
    return sets_;
  }
  void Force(Storage s, int state) { sets_[state].insert(std::move(s)); }

 private:
  void AuditNextState() const {
    if (SetsAreComplete()) return;
    std::array<int, size> empty_neighbors;
    for (int& n : empty_neighbors) n = 0;
    for (int start_state = -1; start_state < size; ++start_state) {
      CHECK_EQ(-1, NextState(start_state, empty_neighbors)) << start_state;
    }
  }

  int CurState(const Storage& s) const {
    int ret = -1;
    for (int set_idx = 0; set_idx < sets_.size(); ++set_idx) {
      if (sets_[set_idx].contains(s)) {
        ret = set_idx;
        break;
      }
    }
    return ret;
  }

  std::array<absl::flat_hash_set<Storage>, size> sets_;
  int step_ = 0;
};

template <typename Storage>
class ConwaySet : public ConwayMultiSet<Storage, 1> {
 public:
  using Base = ConwayMultiSet<Storage, 1>;

  explicit ConwaySet(absl::flat_hash_set<Storage> set)
      : Base({std::move(set)}) {}

  int64_t CountLive() const { return Base::CountState(0); }

  virtual bool IsLive(bool is_live, int neighbors) const = 0;

  virtual int NextState(int state, std::array<int, 1> neighbors) const final {
    return IsLive(state == 0, neighbors[0]) ? 0 : -1;
  }

 protected:
  const absl::flat_hash_set<Storage>& set() const { return Base::sets()[0]; }
  void Force(Storage s) { Base::Force(s, 0); }
};

class ConwayBoard : public ConwaySet<Point> {
 public:
  explicit ConwayBoard(const CharBoard& b)
      : ConwaySet(b.Find('#')), bounds_(b.range()) {}

  std::string Draw() const override {
    return absl::StrCat(CharBoard::DrawNew(set()));
  }

  std::vector<Point> Neighbors(const Point& p) const override {
    std::vector<Point> ret;
    for (Point d : Cardinal::kEightDirs) {
      Point n = p + d;
      if (bounds_.Contains(n)) ret.push_back(n);
    }
    return ret;
  }

  bool IsLive(bool is_live, int neighbors) const override {
    if (neighbors == 3) return true;
    if (is_live && neighbors == 2) return true;
    return false;
  }

 protected:
  PointRectangle bounds() const { return bounds_; }

 private:
  PointRectangle bounds_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CONWAY_H