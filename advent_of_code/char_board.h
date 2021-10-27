#ifndef ADVENT_OF_CODE_CHAR_BOARD_H
#define ADVENT_OF_CODE_CHAR_BOARD_H

#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

namespace advent_of_code {

class CharBoard {
 public:
  template <typename Container>
  static absl::StatusOr<CharBoard> Parse(const Container& in) {
    CharBoard ret(0, 0);
    for (absl::string_view line : in) {
      ret.rows_.push_back(std::string(line));
    }
    if (absl::Status st = ret.Validate(); !st.ok()) return st;
    return ret;
  }

  CharBoard(int width, int height) : rows_(height, std::string(width, '.')) {}

  explicit CharBoard(PointRectangle r)
      : CharBoard(r.max.x - r.min.x + 1, r.max.y - r.min.y + 1) {}

  CharBoard(const CharBoard&) = default;
  CharBoard& operator=(const CharBoard&) = default;

  int height() const { return rows_.size(); }
  int width() const { return rows_[0].size(); }

  PointRectangle range() const {
    return PointRectangle{.min = {.x = 0, .y = 0},
                          .max = {.x = width() - 1, .y = height() - 1}};
  }

  absl::Status Validate() const {
    if (rows_.empty()) return absl::OkStatus();
    for (absl::string_view r : rows_) {
      if (r.size() != width()) {
        return absl::InternalError(absl::StrCat("Bad row size: ", r));
      }
    }
    return absl::OkStatus();
  }

  char at(Point p) const { return rows_[p.y][p.x]; }
  void set(Point p, char c) { rows_[p.y][p.x] = c; }

  char operator[](Point p) const { return rows_[p.y][p.x]; }
  char& operator[](Point p) { return rows_[p.y][p.x]; }

  std::string DebugString() const { return absl::StrJoin(rows_, "\n"); }

  bool OnBoard(Point p) const {
    if (p.y < 0) return false;
    if (p.y >= height()) return false;
    if (p.x < 0) return false;
    if (p.x >= width()) return false;
    return true;
  }

  template <typename H>
  friend H AbslHashValue(H h, const CharBoard& b) {
    return H::combine(std::move(h), b.rows_);
  }

  bool operator==(const CharBoard& o) const { return rows_ == o.rows_; }
  bool operator!=(const CharBoard& o) const { return !operator==(o); }

  friend std::ostream& operator<<(std::ostream& out, const CharBoard& b) {
    return out << b.DebugString();
  }

 private:
  std::vector<std::string> rows_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CHAR_BOARD_H