#ifndef ADVENT_OF_CODE_CONWAY_H
#define ADVENT_OF_CODE_CONWAY_H

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/status/status.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/point.h"

namespace advent_of_code {

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
// Stores the state of a current iteration of Conway's game of life on a
// CharBoard.
class Conway {
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
  Conway(CharBoard b, std::string lookup = DefaultLookup())
      : b_(std::move(b)), lookup_(std::move(lookup)) {}

  // If called, will treat the board as the only nontrivial subset of an
  // infinitely large game of life. All surrounding locations will be
  // considered to have the value `fill`.
  void set_infinite(char fill = '.') {
    infinite_ = true;
    fill_ = fill;
  }

  CharBoard& board() { return b_; }
  const CharBoard& board() const { return b_; }
  char fill() const { return fill_; }

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
  bool infinite_ = false;
  char fill_ = '.';
};

template <typename Storage>
class ConwaySet {
 public:
  explicit ConwaySet(absl::flat_hash_set<Storage> set) : set_(std::move(set)) {}

  virtual ~ConwaySet() = default;

  int64_t CountLive() const { return set_.size(); }

  virtual const absl::flat_hash_set<Storage>& EmptyAllowed() const {
    static const absl::flat_hash_set<Storage> kEmpty;
    return kEmpty;
  }
  virtual std::vector<Storage> Neighbors(const Storage& s) const = 0;
  virtual bool IsLive(bool is_live, int neighbors) const = 0;

  bool Advance() {
    absl::flat_hash_map<Storage, int> neighbor_map;
    for (const Storage& s : EmptyAllowed()) {
      neighbor_map[s] = 0;
    }
    for (const Storage& s : set_) {
      for (Storage n : Neighbors(s)) {
        ++neighbor_map[n];
      }
    }

    bool changed = false;
    absl::flat_hash_set<Storage> next;
    for (const auto& [s, n] : neighbor_map) {
      if (IsLive(set_.contains(s), n)) {
        if (!set_.contains(s)) {
          changed = true;
        }
        next.insert(s);
      }
    }
    if (!changed && next.size() == set_.size()) {
      return false;
    }
    set_ = std::move(next);
    return true;
  }

  bool AdvanceN(int64_t n) {
    for (int i = 0; i < n; ++i) {
      if (!Advance()) return false;
    }
    return true;
  }

 protected:
  const absl::flat_hash_set<Storage>& set() const { return set_; }

 private:
  absl::flat_hash_set<Storage> set_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CONWAY_H