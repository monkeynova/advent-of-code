#ifndef ADVENT_OF_CODE_CHAR_BOARD_H
#define ADVENT_OF_CODE_CHAR_BOARD_H

#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_join.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

struct CharBoard {
  CharBoard(int width, int height) {
    std::string empty_row;
    empty_row.resize(width, '.');
    rows.reserve(height);
    for (int i = 0; i < height; ++i) {
      rows.push_back(empty_row);
    }
  }

  int height() const { return rows.size(); }
  int width() const { return rows[0].size(); }

  PointRectangle range() const {
    return PointRectangle{.min = {.x = 0, .y = 0},
                          .max = {.x = width() - 1, .y = height() - 1}};
  }

  absl::Status Validate() const {
    if (rows.empty()) return absl::OkStatus();
    for (absl::string_view r : rows) {
      if (r.size() != width()) {
        return absl::InternalError("Bad row size");
      }
    }
    return absl::OkStatus();
  }

  char at(Point p) const { return rows[p.y][p.x]; }
  void set(Point p, char c) { rows[p.y][p.x] = c; }

  std::string DebugString() const {
    return absl::StrJoin(rows, "\n");
  }

  bool OnBoard(Point p) const {
    if (p.y < 0) return false;
    if (p.y >= rows.size()) return false;
    if (p.x < 0) return false;
    if (p.x >= rows[0].size()) return false;
    return true;
  }

  std::vector<std::string> rows;
};

std::ostream& operator<<(std::ostream& out, const CharBoard& b) {
  return out << b.DebugString();
}

#endif  // ADVENT_OF_CODE_CHAR_BOARD_H