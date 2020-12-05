#include "advent_of_code/2019/day3/day3.h"

#include <limits>

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"

struct Line {
  Point from;
  Point to;
};

absl::StatusOr<std::vector<Line>> Parse(absl::string_view input) {
  std::vector<absl::string_view> runs = absl::StrSplit(input, ",");

  std::vector<Line> ret;
  ret.reserve(runs.size());
  Point cur = Cardinal::kOrigin;
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

absl::StatusOr<absl::optional<Point>> Intersect(Line l1, Line l2) {
  if (l1.from.x == l1.to.x) {
    if (l2.from.x == l2.to.x) {
      if (l1.from.x == l2.from.x) {
        int miny;
        if (Overlap(l1.from.y, l1.to.y, l2.from.y, l2.to.y, &miny)) {
          return Point{l1.from.x, miny};
        }
        // No intersect.
      }
      // No intersect.
    } else if (l2.from.y == l2.to.y) {
      if (Between(l1.from.x, l2.from.x, l2.to.x) &&
          Between(l2.from.y, l1.from.y, l1.to.y)) {
        return Point{l1.from.x, l2.from.y};
      }
      // No intersect.
    } else {
      return absl::InvalidArgumentError("l2 isn't axis aligned");
    }
  } else if (l1.from.y == l1.to.y) {
    if (l2.from.x == l2.to.x) {
      if (Between(l2.from.x, l1.from.x, l1.to.x) &&
          Between(l1.from.y, l2.from.y, l2.to.y)) {
        return Point{l2.from.x, l1.from.y};
      }
      // No intersect.
    } else if (l2.from.y == l2.to.y) {
      if (l1.from.y == l2.from.y) {
        int minx;
        if (Overlap(l1.from.x, l1.to.x, l2.from.x, l2.to.x, &minx)) {
          return Point{minx, l1.from.y};
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

absl::StatusOr<std::vector<Point>> Intersect(std::vector<Line> wire1,
                                             std::vector<Line> wire2) {
  std::vector<Point> ret;
  for (const auto& l1 : wire1) {
    for (const auto& l2 : wire2) {
      absl::StatusOr<absl::optional<Point>> intersect = Intersect(l1, l2);
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
  if (input.size() != 2) {
    return absl::InvalidArgumentError("input does not contain 2 lines");
  }
  absl::StatusOr<std::vector<Line>> wire1 = Parse(input[0]);
  if (!wire1.ok()) return wire1.status();
  absl::StatusOr<std::vector<Line>> wire2 = Parse(input[1]);
  if (!wire2.ok()) return wire2.status();

  absl::StatusOr<std::vector<Point>> overlap = Intersect(*wire1, *wire2);
  if (!overlap.ok()) return overlap.status();
  absl::optional<Point> min;
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

absl::StatusOr<bool> PointOnLine(Point point, Line line) {
  if (line.from.x == line.to.x) {
    return point.x == line.from.x && Between(point.y, line.from.y, line.to.y);
  } else if (line.from.y == line.to.y) {
    return point.y == line.from.y && Between(point.x, line.from.x, line.to.x);
  }
  return absl::InvalidArgumentError("line isn't axis aligned");
}

absl::StatusOr<int> CostToOverlap(Point intersect,
                                  const std::vector<Line>& wire) {
  std::vector<int> cost_to_start(wire.size() + 1, 0);
  for (int i = 0; i < wire.size(); ++i) {
    absl::StatusOr<bool> on_line = PointOnLine(intersect, wire[i]);
    if (!on_line.ok()) return on_line.status();

    if (*on_line) {
      return cost_to_start[i] + wire[i].from.dist(intersect);
    }
    cost_to_start[i + 1] = cost_to_start[i] + wire[i].from.dist(wire[i].to);
  }

  return absl::InvalidArgumentError("Point not found on wire");
}

absl::StatusOr<std::vector<std::string>> Day3_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  if (input.size() != 2) {
    return absl::InvalidArgumentError("input does not contain 2 lines");
  }
  absl::StatusOr<std::vector<Line>> wire1 = Parse(input[0]);
  if (!wire1.ok()) return wire1.status();
  absl::StatusOr<std::vector<Line>> wire2 = Parse(input[1]);
  if (!wire2.ok()) return wire2.status();

  absl::StatusOr<std::vector<Point>> overlap = Intersect(*wire1, *wire2);
  if (!overlap.ok()) return overlap.status();
  absl::optional<Point> min;
  int min_score = std::numeric_limits<int>::max();
  for (const auto& point : *overlap) {
    if (point.dist() > 0) {
      absl::StatusOr<int> wire1_score = CostToOverlap(point, *wire1);
      if (!wire1_score.ok()) return wire1_score.status();
      absl::StatusOr<int> wire2_score = CostToOverlap(point, *wire2);
      if (!wire2_score.ok()) return wire2_score.status();

      int score = *wire1_score + *wire2_score;
      if (score < min_score) {
        min = point;
        min_score = score;
      }
    }
  }

  if (!min) {
    return absl::InvalidArgumentError("No intersect found");
  }

  return std::vector<std::string>{absl::StrCat(min_score)};
}
