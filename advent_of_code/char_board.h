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

  int CountOn() const { return CountChar('#'); }
  int CountChar(char test) const {
    int count = 0;
    for (const auto& r : rows_) {
      for (char c : r) {
        if (c == test) ++count;
      }
    }
    return count;
  }

  int height() const { return rows_.size(); }
  int width() const { return rows_.empty() ? 0 : rows_[0].size(); }

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

  std::string AsString() const { return absl::StrJoin(rows_, "\n"); }

  bool OnBoard(Point p) const {
    if (p.y < 0) return false;
    if (p.y >= height()) return false;
    if (p.x < 0) return false;
    if (p.x >= width()) return false;
    return true;
  }

  absl::StatusOr<CharBoard> SubBoard(PointRectangle sub_range) const {
    if (!OnBoard(sub_range.min)) {
      return absl::InvalidArgumentError("SubBoard: min not on board");
    }
    if (!OnBoard(sub_range.max)) {
      return absl::InvalidArgumentError("SubBoard: max not on board");
    }
    CharBoard out(0, 0);
    int sub_off = sub_range.min.x;
    int sub_len = sub_range.max.x - sub_range.min.x + 1;
    out.rows_.resize(sub_range.max.y - sub_range.min.y + 1);
    for (int i = sub_range.min.y; i <= sub_range.max.y; ++i) {
      out.rows_[i - sub_range.min.y] = rows_[i].substr(sub_off, sub_len);
    }
    return out;
  }

  template <typename H>
  friend H AbslHashValue(H h, const CharBoard& b) {
    return H::combine(std::move(h), b.rows_);
  }

  bool operator==(const CharBoard& o) const { return rows_ == o.rows_; }
  bool operator!=(const CharBoard& o) const { return !operator==(o); }

  friend std::ostream& operator<<(std::ostream& out, const CharBoard& b) {
    for (absl::string_view s : b.rows_) {
      out << s << std::endl;
    }
    return out;
  }

 private:
  // TODO(@monkeynova): Migrate to a single flat-buffer string.
  std::vector<std::string> rows_;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_CHAR_BOARD_H