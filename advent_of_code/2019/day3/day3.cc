#include "advent_of_code/2019/day3/day3.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"

struct Line {
  struct Point {
    int x;
    int y;

    int dist() const { return abs(x) + abs(y); }
  };

  Point from;
  Point to;
};

absl::StatusOr<std::vector<Line>> Parse(absl::string_view input) {
  std::vector<absl::string_view> runs = absl::StrSplit(input, ",");

  std::vector<Line> ret;
  ret.reserve(runs.size());
  Line::Point cur{0, 0};
  for (absl::string_view run : runs) {
    Line next;
    next.from = cur;
    next.to = cur;
    if (run.size() < 2) {
      return absl::InvalidArgumentError(
          absl::StrCat("Bad run (length): ", run));
    }
    int run_len;
    if (!absl::SimpleAtoi(run.substr(1), &run_len)) {
      return absl::InvalidArgumentError(absl::StrCat("Bad run (atoi): ", run));
    }
    switch (run[0]) {
      case 'R': {
        next.to.x += run_len;
        break;
      }
      case 'L': {
        next.to.x -= run_len;
        break;
      }
      case 'U': {
        next.to.y += run_len;
        break;
      }
      case 'D': {
        next.to.y -= run_len;
        break;
      }
      default: {
        return absl::InvalidArgumentError(absl::StrCat("Bad run (dir): ", run));
      }
    }

    cur = next.to;
    ret.push_back(next);
  }

  return ret;
}

bool Between(int test, int bound1, int bound2) {
  if (bound1 < bound2) {
    return bound1 <= test && test <= bound2;
  }
  return bound2 <= test && test <= bound1;
}

bool Overlap(int bound11, int bound12, int bound21, int bound22, int* out) {
  if (bound11 > bound12) std::swap(bound11, bound12);
  if (bound21 > bound22) std::swap(bound21, bound22);
  if (Between(bound11, bound21, bound22)) {
    *out = bound11;
    return true;
  }
  if (Between(bound21, bound11, bound12)) {
    *out = bound21;
    return true;
  }
  return false;
}

absl::StatusOr<absl::optional<Line::Point>> Intersect(Line l1, Line l2) {
  if (l1.from.x == l1.to.x) {
    if (l2.from.x == l2.to.x) {
      if (l1.from.x == l2.from.x) {
        int miny;
        if (Overlap(l1.from.y, l1.to.y, l2.from.y, l2.to.y, &miny)) {
          return Line::Point{l1.from.x, miny};
        }
        // No intersect.
      }
      // No intersect.
    } else if (l2.from.y == l2.to.y) {
      if (Between(l1.from.x, l2.from.x, l2.to.x) &&
          Between(l2.from.y, l1.from.y, l1.to.y)) {
        return Line::Point{l1.from.x, l2.from.y};
      }
      // No intersect.
    } else {
      return absl::InvalidArgumentError("l2 isn't axis aligned");
    }
  } else if (l1.from.y == l1.to.y) {
    if (l2.from.x == l2.to.x) {
      if (Between(l2.from.x, l1.from.x, l1.to.x) &&
          Between(l1.from.y, l2.from.y, l2.to.y)) {
        return Line::Point{l2.from.x, l1.from.y};
      }
      // No intersect.
    } else if (l2.from.y == l2.to.y) {
      if (l1.from.y == l2.from.y) {
        int minx;
        if (Overlap(l1.from.x, l1.to.x, l2.from.x, l2.to.x, &minx)) {
          return Line::Point{minx, l1.from.y};
        }
        // No intersect.
      }
      // No intersect.
    } else {
      return absl::InvalidArgumentError("l2 isn't axis aligned");
    }
  } else {
    return absl::InvalidArgumentError("l1 isn't axis aligned");
  }
  return absl::nullopt;
}

absl::StatusOr<std::vector<Line::Point>> Intersect(std::vector<Line> wire1,
                                                   std::vector<Line> wire2) {
  std::vector<Line::Point> ret;
  for (const auto& l1 : wire1) {
    for (const auto& l2 : wire2) {
      absl::StatusOr<absl::optional<Line::Point>> intersect = Intersect(l1, l2);
      if (!intersect.ok()) return intersect.status();
      if (*intersect) {
        ret.push_back(**intersect);
      }
    }
  }

  return ret;
}

absl::StatusOr<std::vector<std::string>> Day3_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  if (input.size() < 2) {
    return absl::InvalidArgumentError("input does not contain 2 lines");
  }
  absl::StatusOr<std::vector<Line>> wire1 = Parse(input[0]);
  if (!wire1.ok()) return wire1.status();
  absl::StatusOr<std::vector<Line>> wire2 = Parse(input[1]);
  if (!wire2.ok()) return wire2.status();
  for (int i = 2; i < input.size(); ++i) {
    if (!input[i].empty()) {
      return absl::InvalidArgumentError("Input contains a 3rd non-empty line");
    }
  }

  absl::StatusOr<std::vector<Line::Point>> overlap = Intersect(*wire1, *wire2);
  if (!overlap.ok()) return overlap.status();
  absl::optional<Line::Point> min;
  for (const auto& point : *overlap) {
    if (point.dist() > 0 && (!min || min->dist() > point.dist())) {
      min = point;
    }
  }

  if (!min) {
    return absl::InvalidArgumentError("No intersect found");
  }

  return std::vector<std::string>{absl::StrCat(min->dist())};
}

absl::StatusOr<std::vector<std::string>> Day3_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
