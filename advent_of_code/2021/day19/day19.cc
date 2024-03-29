#include "advent_of_code/2021/day19/day19.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point3.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Scanner {
  std::vector<Point3> relative_beacons;
  Point3 absolute;
  Orientation3 o;

  Point3 RelativeToAbsolute(Point3 in) const { return absolute + o.Apply(in); }
};

int max_dist(Point3 p) {
  return std::max(abs(p.x), std::max(abs(p.y), abs(p.z)));
}

absl::StatusOr<int> CountOverlap(const Scanner& l, const Scanner& r) {
  absl::flat_hash_set<Point3> absolute_beacons;
  for (const auto& b : l.relative_beacons) {
    absolute_beacons.insert(l.RelativeToAbsolute(b));
  }
  int visible_l_in_r = 0;
  absl::flat_hash_set<Point3> found_both;
  for (const auto& r_b : r.relative_beacons) {
    Point3 a_b = r.RelativeToAbsolute(r_b);
    if (absolute_beacons.contains(a_b)) {
      found_both.insert(a_b);
    } else {
      if (max_dist(a_b - l.absolute) <= 1000) {
        ++visible_l_in_r;
      }
    }
  }
  if (found_both.size() >= 12) {
    // Validate the problems assertion that if 12 overlap and we put these
    // together that there aren't any beacons that should have been visble
    // that we didn't align.
    if (visible_l_in_r != 0) {
      return Error("visible_l_in_r = ", visible_l_in_r);
    }
    int visible_r_in_l = 0;
    for (Point3 a_b : absolute_beacons) {
      if (found_both.contains(a_b)) continue;
      if (max_dist(a_b - r.absolute) <= 1000) {
        ++visible_r_in_l;
      }
    }
    if (visible_r_in_l != 0) {
      return Error("visible_r_in_l = ", visible_r_in_l);
    }
  }
  return found_both.size();
}

absl::StatusOr<bool> TryPosition(Scanner* dest, const Scanner& src) {
  for (const Orientation3& o : Orientation3::All()) {
    dest->o = o;
    VLOG(2) << "Trying o=" << o;
    absl::flat_hash_map<Point3, int> try_absolute_counts;
    for (Point3 r_p1 : src.relative_beacons) {
      Point3 a_p1 = src.RelativeToAbsolute(r_p1);
      VLOG(2) << "Src: " << r_p1 << " -> " << a_p1;
      for (Point3 r_p2 : dest->relative_beacons) {
        // RelativeToAbsolute:
        // a_p2 = dest->absolute + o.Apply(r_p2)
        Point3 test_absolute = a_p1 - o.Apply(r_p2);
        VLOG(2) << "Dst: " << r_p2 << " -> " << test_absolute;
        ++try_absolute_counts[test_absolute];
      }
    }
    for (const auto& [p, c] : try_absolute_counts) {
      if (c < 12) continue;
      dest->absolute = p;
      ASSIGN_OR_RETURN(int overlap, CountOverlap(*dest, src));
      if (overlap >= 12) {
        return true;
      }
    }
  }
  return false;
}

absl::StatusOr<std::vector<Scanner>> Parse(absl::Span<std::string_view> input) {
  std::vector<Scanner> scanners;
  Scanner cur;
  for (std::string_view line : input) {
    if (line.empty()) continue;
    Point3 beacon;
    if (RE2::FullMatch(line, "--- scanner \\d+ ---")) {
      if (!cur.relative_beacons.empty()) scanners.push_back(cur);
      cur.relative_beacons.clear();
    } else if (RE2::FullMatch(line, "(-?\\d+,-?\\d+,-?\\d+)",
                              beacon.Capture())) {
      cur.relative_beacons.push_back(beacon);
    } else {
      return Error("Bad line: ", line);
    }
  }
  scanners.push_back(cur);
  return scanners;
}

absl::Status PositionScanners(std::vector<Scanner>& scanners) {
  scanners[0].absolute = Point3{0, 0, 0};
  scanners[0].o = Orientation3::Aligned();
  absl::flat_hash_set<int> positioned = {0};
  absl::flat_hash_map<int, absl::flat_hash_set<int>> attempted;
  while (positioned.size() < scanners.size()) {
    absl::flat_hash_set<int> new_positioned;
    for (int i = 0; i < scanners.size(); ++i) {
      if (positioned.contains(i)) continue;
      absl::flat_hash_set<int>& this_attempt = attempted[i];
      for (int p_idx : positioned) {
        if (this_attempt.contains(p_idx)) continue;

        VLOG(2) << "Trying " << i << " with " << p_idx;
        ASSIGN_OR_RETURN(bool collapsed,
                         TryPosition(&scanners[i], scanners[p_idx]));
        if (collapsed) {
          VLOG(1) << "Collapsed " << i << " with " << p_idx;
          new_positioned.insert(i);
          break;
        }
        this_attempt.insert(p_idx);
      }
    }
    if (new_positioned.empty()) {
      return Error("Cannot add more positioning");
    }
    for (int p_idx : new_positioned) positioned.insert(p_idx);
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_19::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<Scanner> scanners, Parse(input));

  RETURN_IF_ERROR(PositionScanners(scanners));

  absl::flat_hash_set<Point3> absolute_becons;
  for (const auto& s : scanners) {
    for (const auto& b : s.relative_beacons) {
      absolute_becons.insert(s.RelativeToAbsolute(b));
    }
  }

  return AdventReturn(absolute_becons.size());
}

absl::StatusOr<std::string> Day_2021_19::Part2(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(std::vector<Scanner> scanners, Parse(input));

  RETURN_IF_ERROR(PositionScanners(scanners));

  int max_dist = 0;
  for (int i = 0; i < scanners.size(); ++i) {
    for (int j = i + 1; j < scanners.size(); ++j) {
      int dist = (scanners[i].absolute - scanners[j].absolute).dist();
      max_dist = std::max(max_dist, dist);
    }
  }

  return AdventReturn(max_dist);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2021, /*day=*/19,
    []() { return std::unique_ptr<AdventDay>(new Day_2021_19()); });

}  // namespace advent_of_code
