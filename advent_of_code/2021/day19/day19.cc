#include "advent_of_code/2021/day19/day19.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct Orientation {
  Point3 x_hat;
  Point3 y_hat;
  Point3 z_hat;
};

const std::vector<Orientation>& AllOrientations() {
  static std::vector<Orientation> kMemo;
  if (!kMemo.empty()) return kMemo;

  for (Point3 x_hat : Cardinal3::kNeighborDirs) {
    if (x_hat.dist() != 1) continue;
    VLOG(2) << x_hat;
    for (Point3 y_hat : Cardinal3::kNeighborDirs) {
      if (y_hat.dist() != 1) continue;
      if (y_hat == x_hat || y_hat == -x_hat) continue;
      Point3 z_hat = {
        x_hat.y * y_hat.z - x_hat.z * y_hat.y,
        x_hat.z * y_hat.x - x_hat.x * y_hat.z,
        x_hat.x * y_hat.y - x_hat.y * y_hat.x,
      };
      VLOG(2) << "  " << y_hat;
      VLOG(2) << "  " << z_hat;
      kMemo.push_back({x_hat, y_hat, z_hat});
    }
  }
  CHECK_EQ(kMemo.size(), 24);
  return kMemo;
}

struct Scanner {
  std::vector<Point3> relative_beacons;
  Point3 absolute;
  Orientation o;

  Point3 RelativeToAbsolute(Point3 in) const {
    return absolute + in.x * o.x_hat + in.y * o.y_hat + in.z * o.z_hat;
  }
};

int CountOverlap(const Scanner& l, const Scanner& r) {
  absl::flat_hash_set<Point3> absolute_becons;
  for (const auto& b : l.relative_beacons) {
    absolute_becons.insert(l.RelativeToAbsolute(b));
  }
  int count = 0;
  for (const auto& b : r.relative_beacons) {
    if (absolute_becons.contains(r.RelativeToAbsolute(b))) {
      ++count;
    }
  }
  return count;
}

bool TryPosition(Scanner* dest, const Scanner& src) {
  for (const Orientation& o : AllOrientations()) {
    dest->o = o;
    VLOG(2) << "Trying o=" << o.x_hat << "/" << o.y_hat << "/" << o.z_hat;
    for (Point3 r_p1 : src.relative_beacons) {
      Point3 a_p1 = src.RelativeToAbsolute(r_p1);
      VLOG(2) << "Src: " << r_p1 << " -> " << a_p1;
      for (Point3 r_p2 : dest->relative_beacons) {
        // a_p1 == a_p2
        // a_p2 = dest->absolute + r_p2.x * o.x_hat + r_p2.y * o.y_hat + r_p2.z * o.z_hat
        // (a_p1 - r_p2.x * o.x_hat + r_p2.y * o.y_hat + r_p2.z * o.z_hat) = dest->absolute
        dest->absolute = a_p1 - r_p2.x * o.x_hat - r_p2.y * o.y_hat - r_p2.z * o.z_hat;
        Point3 a_p2 = src.RelativeToAbsolute(r_p2);
        VLOG(2) << "Dst: " << r_p2 << " -> " << a_p2;
        if (CountOverlap(*dest, src) >= 12) {
          return true;
        }
      }
    }
  }
  return false;
}

}  // namespace

absl::StatusOr<std::string> Day_2021_19::Part1(
    absl::Span<absl::string_view> input) const {
  std::vector<Scanner> scanners;
  Scanner cur;
  for (absl::string_view line : input) {
    if (line.empty()) continue;
    Point3 beacon;
    if (RE2::FullMatch(line, "--- scanner \\d+ ---")) {
      if (!cur.relative_beacons.empty()) scanners.push_back(cur);
      cur.relative_beacons.clear();
    } else if (RE2::FullMatch(line, "(-?\\d+,-?\\d+,-?\\d+)", beacon.Capture())) {
      cur.relative_beacons.push_back(beacon);
    } else {
      return Error("Bad line: ", line);
    }
  }
  scanners.push_back(cur);

  scanners[0].absolute = Point3{0, 0, 0};
  scanners[0].o = {.x_hat = Cardinal3::kXHat, .y_hat = Cardinal3::kYHat, .z_hat = Cardinal3::kZHat};
  absl::flat_hash_set<int> positioned = {0};
  while (positioned.size() < scanners.size()) {
    absl::flat_hash_set<int> new_positioned = positioned;
    for (int i = 0; i < scanners.size(); ++i) {
      if (positioned.contains(i)) continue;
      for (int p_idx : positioned) {
        VLOG(2) << "Trying " << i << " with " << p_idx;
        if (TryPosition(&scanners[i], scanners[p_idx])) {
          VLOG(1) << "Collapsed " << i << " with " << p_idx;
          new_positioned.insert(i);
          break;
        }
      }
    }
    if (new_positioned.size() == positioned.size()) {
      return Error("Cannot add more positioning");
    }
    positioned = std::move(new_positioned);
  }

  absl::flat_hash_set<Point3> absolute_becons;
  for (const auto& s : scanners) {
    for (const auto& b : s.relative_beacons) {
      absolute_becons.insert(s.RelativeToAbsolute(b));
    }
  }

  return IntReturn(absolute_becons.size());
}

absl::StatusOr<std::string> Day_2021_19::Part2(
    absl::Span<absl::string_view> input) const {
  std::vector<Scanner> scanners;
  Scanner cur;
  for (absl::string_view line : input) {
    if (line.empty()) continue;
    Point3 beacon;
    if (RE2::FullMatch(line, "--- scanner \\d+ ---")) {
      if (!cur.relative_beacons.empty()) scanners.push_back(cur);
      cur.relative_beacons.clear();
    } else if (RE2::FullMatch(line, "(-?\\d+,-?\\d+,-?\\d+)", beacon.Capture())) {
      cur.relative_beacons.push_back(beacon);
    } else {
      return Error("Bad line: ", line);
    }
  }
  scanners.push_back(cur);

  scanners[0].absolute = Point3{0, 0, 0};
  scanners[0].o = {.x_hat = Cardinal3::kXHat, .y_hat = Cardinal3::kYHat, .z_hat = Cardinal3::kZHat};
  absl::flat_hash_set<int> positioned = {0};
  while (positioned.size() < scanners.size()) {
    absl::flat_hash_set<int> new_positioned = positioned;
    for (int i = 0; i < scanners.size(); ++i) {
      if (positioned.contains(i)) continue;
      for (int p_idx : positioned) {
        VLOG(2) << "Trying " << i << " with " << p_idx;
        if (TryPosition(&scanners[i], scanners[p_idx])) {
          VLOG(1) << "Collapsed " << i << " with " << p_idx;
          new_positioned.insert(i);
          break;
        }
      }
    }
    if (new_positioned.size() == positioned.size()) {
      return Error("Cannot add more positioning");
    }
    positioned = std::move(new_positioned);
  }

  int max_dist = 0;
  for (int i = 0; i < scanners.size(); ++i) {
    for (int j = i + 1; j < scanners.size(); ++j) {
      int dist = (scanners[i].absolute - scanners[j].absolute).dist();
      max_dist = std::max(max_dist, dist);
    }
  }

  return IntReturn(max_dist);
}

}  // namespace advent_of_code
