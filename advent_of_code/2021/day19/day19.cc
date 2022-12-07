// https://adventofcode.com/2021/day/19
//
// --- Day 19: Beacon Scanner ---
// ------------------------------
//
// As your probe drifted down through this area, it released an
// assortment of beacons and scanners into the water. It's difficult to
// navigate in the pitch black open waters of the ocean trench, but if
// you can build a map of the trench using data from the scanners, you
// should be able to safely reach the bottom.
//
// The beacons and scanners float motionless in the water; they're
// designed to maintain the same position for long periods of time. Each
// scanner is capable of detecting all beacons in a large cube centered
// on the scanner; beacons that are at most 1000 units away from the
// scanner in each of the three axes (x, y, and z) have their precise
// position determined relative to the scanner. However, scanners cannot
// detect other scanners. The submarine has automatically summarized the
// relative positions of beacons detected by each scanner (your puzzle
// input).
//
// For example, if a scanner is at x,y,z coordinates 500,0,-500 and there
// are beacons at -500,1000,-1500 and 1501,0,-500, the scanner could
// report that the first beacon is at -1000,1000,-1000 (relative to the
// scanner) but would not detect the second beacon at all.
//
// Unfortunately, while each scanner can report the positions of all
// detected beacons relative to itself, the scanners do not know their
// own position. You'll need to determine the positions of the beacons
// and scanners yourself.
//
// The scanners and beacons map a single contiguous 3d region. This
// region can be reconstructed by finding pairs of scanners that have
// overlapping detection regions such that there are at least 12 beacons
// that both scanners detect within the overlap. By establishing 12
// common beacons, you can precisely determine where the scanners are
// relative to each other, allowing you to reconstruct the beacon map one
// scanner at a time.
//
// For a moment, consider only two dimensions. Suppose you have the
// following scanner reports:
//
// --- scanner 0 ---
// 0,2
// 4,1
// 3,3
//
// --- scanner 1 ---
// -1,-1
// -5,0
// -2,1
//
// Drawing x increasing rightward, y increasing upward, scanners as S,
// and beacons as B, scanner 0 detects this:
//
// ...B.
// B....
// ....B
// S....
//
// Scanner 1 detects this:
//
// ...B..
// B....S
// ....B.
//
// For this example, assume scanners only need 3 overlapping beacons.
// Then, the beacons visible to both scanners overlap to produce the
// following complete map:
//
// ...B..
// B....S
// ....B.
// S.....
//
// Unfortunately, there's a second problem: the scanners also don't know
// their rotation or facing direction. Due to magnetic alignment, each
// scanner is rotated some integer number of 90-degree turns around all
// of the x, y, and z axes. That is, one scanner might call a direction
// positive x, while another scanner might call that direction negative y.
// Or, two scanners might agree on which direction is positive x, but one
// scanner might be upside-down from the perspective of the other
// scanner. In total, each scanner could be in any of 24 different
// orientations: facing positive or negative x, y, or z, and considering
// any of four directions "up" from that facing.
//
// For example, here is an arrangement of beacons as seen from a scanner
// in the same position but in different orientations:
//
// --- scanner 0 ---
// -1,-1,1
// -2,-2,2
// -3,-3,3
// -2,-3,1
// 5,6,-4
// 8,0,7
//
// --- scanner 0 ---
// 1,-1,1
// 2,-2,2
// 3,-3,3
// 2,-1,3
// -5,4,-6
// -8,-7,0
//
// --- scanner 0 ---
// -1,-1,-1
// -2,-2,-2
// -3,-3,-3
// -1,-3,-2
// 4,6,5
// -7,0,8
//
// --- scanner 0 ---
// 1,1,-1
// 2,2,-2
// 3,3,-3
// 1,3,-2
// -4,-6,5
// 7,0,8
//
// --- scanner 0 ---
// 1,1,1
// 2,2,2
// 3,3,3
// 3,1,2
// -6,-4,-5
// 0,7,-8
//
// By finding pairs of scanners that both see at least 12 of the same
// beacons, you can assemble the entire map. For example, consider the
// following report:
//
// --- scanner 0 ---
// 404,-588,-901
// 528,-643,409
// -838,591,734
// 390,-675,-793
// -537,-823,-458
// -485,-357,347
// -345,-311,381
// -661,-816,-575
// -876,649,763
// -618,-824,-621
// 553,345,-567
// 474,580,667
// -447,-329,318
// -584,868,-557
// 544,-627,-890
// 564,392,-477
// 455,729,728
// -892,524,684
// -689,845,-530
// 423,-701,434
// 7,-33,-71
// 630,319,-379
// 443,580,662
// -789,900,-551
// 459,-707,401
//
// --- scanner 1 ---
// 686,422,578
// 605,423,415
// 515,917,-361
// -336,658,858
// 95,138,22
// -476,619,847
// -340,-569,-846
// 567,-361,727
// -460,603,-452
// 669,-402,600
// 729,430,532
// -500,-761,534
// -322,571,750
// -466,-666,-811
// -429,-592,574
// -355,545,-477
// 703,-491,-529
// -328,-685,520
// 413,935,-424
// -391,539,-444
// 586,-435,557
// -364,-763,-893
// 807,-499,-711
// 755,-354,-619
// 553,889,-390
//
// --- scanner 2 ---
// 649,640,665
// 682,-795,504
// -784,533,-524
// -644,584,-595
// -588,-843,648
// -30,6,44
// -674,560,763
// 500,723,-460
// 609,671,-379
// -555,-800,653
// -675,-892,-343
// 697,-426,-610
// 578,704,681
// 493,664,-388
// -671,-858,530
// -667,343,800
// 571,-461,-707
// -138,-166,112
// -889,563,-600
// 646,-828,498
// 640,759,510
// -630,509,768
// -681,-892,-333
// 673,-379,-804
// -742,-814,-386
// 577,-820,562
//
// --- scanner 3 ---
// -589,542,597
// 605,-692,669
// -500,565,-823
// -660,373,557
// -458,-679,-417
// -488,449,543
// -626,468,-788
// 338,-750,-386
// 528,-832,-391
// 562,-778,733
// -938,-730,414
// 543,643,-506
// -524,371,-870
// 407,773,750
// -104,29,83
// 378,-903,-323
// -778,-728,485
// 426,699,580
// -438,-605,-362
// -469,-447,-387
// 509,732,623
// 647,635,-688
// -868,-804,481
// 614,-800,639
// 595,780,-596
//
// --- scanner 4 ---
// 727,592,562
// -293,-554,779
// 441,611,-461
// -714,465,-776
// -743,427,-804
// -660,-479,-426
// 832,-632,460
// 927,-485,-438
// 408,393,-506
// 466,436,-512
// 110,16,151
// -258,-428,682
// -393,719,612
// -211,-452,876
// 808,-476,-593
// -575,615,604
// -485,667,467
// -680,325,-822
// -627,-443,-432
// 872,-547,-609
// 833,512,582
// 807,604,487
// 839,-516,451
// 891,-625,532
// -652,-548,-490
// 30,-46,-14
//
// Because all coordinates are relative, in this example, all "absolute"
// positions will be expressed relative to scanner 0 (using the
// orientation of scanner 0 and as if scanner 0 is at coordinates 0,0,0).
//
// Scanners 0 and 1 have overlapping detection cubes; the 12 beacons they
// both detect (relative to scanner 0) are at the following coordinates:
//
// -618,-824,-621
// -537,-823,-458
// -447,-329,318
// 404,-588,-901
// 544,-627,-890
// 528,-643,409
// -661,-816,-575
// 390,-675,-793
// 423,-701,434
// -345,-311,381
// 459,-707,401
// -485,-357,347
//
// These same 12 beacons (in the same order) but from the perspective of
// scanner 1 are:
//
// 686,422,578
// 605,423,415
// 515,917,-361
// -336,658,858
// -476,619,847
// -460,603,-452
// 729,430,532
// -322,571,750
// -355,545,-477
// 413,935,-424
// -391,539,-444
// 553,889,-390
//
// Because of this, scanner 1 must be at 68,-1246,-43 (relative to
// scanner 0).
//
// Scanner 4 overlaps with scanner 1; the 12 beacons they both detect
// (relative to scanner 0) are:
//
// 459,-707,401
// -739,-1745,668
// -485,-357,347
// 432,-2009,850
// 528,-643,409
// 423,-701,434
// -345,-311,381
// 408,-1815,803
// 534,-1912,768
// -687,-1600,576
// -447,-329,318
// -635,-1737,486
//
// So, scanner 4 is at -20,-1133,1061 (relative to scanner 0).
//
// Following this process, scanner 2 must be at 1105,-1205,1229 (relative
// to scanner 0) and scanner 3 must be at -92,-2380,-20 (relative to
// scanner 0).
//
// The full list of beacons (relative to scanner 0) is:
//
// -892,524,684
// -876,649,763
// -838,591,734
// -789,900,-551
// -739,-1745,668
// -706,-3180,-659
// -697,-3072,-689
// -689,845,-530
// -687,-1600,576
// -661,-816,-575
// -654,-3158,-753
// -635,-1737,486
// -631,-672,1502
// -624,-1620,1868
// -620,-3212,371
// -618,-824,-621
// -612,-1695,1788
// -601,-1648,-643
// -584,868,-557
// -537,-823,-458
// -532,-1715,1894
// -518,-1681,-600
// -499,-1607,-770
// -485,-357,347
// -470,-3283,303
// -456,-621,1527
// -447,-329,318
// -430,-3130,366
// -413,-627,1469
// -345,-311,381
// -36,-1284,1171
// -27,-1108,-65
// 7,-33,-71
// 12,-2351,-103
// 26,-1119,1091
// 346,-2985,342
// 366,-3059,397
// 377,-2827,367
// 390,-675,-793
// 396,-1931,-563
// 404,-588,-901
// 408,-1815,803
// 423,-701,434
// 432,-2009,850
// 443,580,662
// 455,729,728
// 456,-540,1869
// 459,-707,401
// 465,-695,1988
// 474,580,667
// 496,-1584,1900
// 497,-1838,-617
// 527,-524,1933
// 528,-643,409
// 534,-1912,768
// 544,-627,-890
// 553,345,-567
// 564,392,-477
// 568,-2007,-577
// 605,-1665,1952
// 612,-1593,1893
// 630,319,-379
// 686,-3108,-505
// 776,-3184,-501
// 846,-3110,-434
// 1135,-1161,1235
// 1243,-1093,1063
// 1660,-552,429
// 1693,-557,386
// 1735,-437,1738
// 1749,-1800,1813
// 1772,-405,1572
// 1776,-675,371
// 1779,-442,1789
// 1780,-1548,337
// 1786,-1538,337
// 1847,-1591,415
// 1889,-1729,1762
// 1994,-1805,1792
//
// In total, there are 79 beacons.
//
// Assemble the full map of beacons. How many beacons are there?
//
// --- Part Two ---
// ----------------
//
// Sometimes, it's a good idea to appreciate just how big the ocean is.
// Using the Manhattan distance, how far apart do the scanners get?
//
// In the above example, scanners 2 (1105,-1205,1229) and 3 (-92,-2380,-20)
// are the largest Manhattan distance apart. In total, they are 1197 +
// 1175 + 1249 = 3621 units apart.
//
// What is the largest Manhattan distance between any two scanners?

#include "advent_of_code/2021/day19/day19.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

class Orientation3 {
 public:
  static Orientation3 Aligned() { return Orientation3(); }

  static const std::vector<Orientation3>& All();

  Orientation3() : Orientation3(Cardinal3::kXHat, Cardinal3::kYHat) {}

  Orientation3(Point3 x_hat, Point3 y_hat)
      : x_hat_(x_hat), y_hat_(y_hat), z_hat_(x_hat.Cross(y_hat)) {
    CHECK_EQ(x_hat_.dist(), 1);
    CHECK_EQ(y_hat_.dist(), 1);
    CHECK_EQ(z_hat_.dist(), 1);
    CHECK_EQ(x_hat_.Dot(y_hat_), 0);
    CHECK_EQ(x_hat_.Dot(z_hat_), 0);
    CHECK_EQ(y_hat_.Dot(z_hat_), 0);
  }

  Point3 Apply(Point3 in) const {
    return in.x * x_hat_ + in.y * y_hat_ + in.z * z_hat_;
  }

  friend std::ostream& operator<<(std::ostream& out, const Orientation3& o) {
    return out << o.x_hat_ << "/" << o.y_hat_ << "/" << o.z_hat_;
  }

 private:
  Point3 x_hat_;
  Point3 y_hat_;
  Point3 z_hat_;
};

const std::vector<Orientation3>& Orientation3::All() {
  static std::vector<Orientation3> kMemo;
  if (!kMemo.empty()) return kMemo;

  for (Point3 x_hat : Cardinal3::kNeighborDirs) {
    if (x_hat.dist() != 1) continue;
    for (Point3 y_hat : Cardinal3::kNeighborDirs) {
      if (y_hat.dist() != 1) continue;
      if (y_hat == x_hat) continue;
      if (y_hat == -x_hat) continue;
      kMemo.push_back(Orientation3(x_hat, y_hat));
    }
  }
  CHECK_EQ(kMemo.size(), 24);
  return kMemo;
}

struct Scanner {
  std::vector<Point3> relative_beacons;
  Point3 absolute;
  Orientation3 o;

  Point3 RelativeToAbsolute(Point3 in) const { return absolute + o.Apply(in); }
};

int max_dist(Point3 p) {
  return std::max(abs(p.x), std::max(abs(p.y), abs(p.z)));
}

int CountOverlap(const Scanner& l, const Scanner& r) {
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
    CHECK_EQ(visible_l_in_r, 0);
    int visible_r_in_l = 0;
    for (Point3 a_b : absolute_beacons) {
      if (found_both.contains(a_b)) continue;
      if (max_dist(a_b - r.absolute) <= 1000) {
        ++visible_r_in_l;
      }
    }
    CHECK_EQ(visible_r_in_l, 0);
  }
  return found_both.size();
}

bool TryPosition(Scanner* dest, const Scanner& src) {
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
      if (CountOverlap(*dest, src) >= 12) {
        return true;
      }
    }
  }
  return false;
}

absl::StatusOr<std::vector<Scanner>> Parse(
    absl::Span<absl::string_view> input) {
  std::vector<Scanner> scanners;
  Scanner cur;
  for (absl::string_view line : input) {
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
        if (TryPosition(&scanners[i], scanners[p_idx])) {
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
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Scanner>> scanners = Parse(input);
  if (!scanners.ok()) return scanners.status();

  if (auto st = PositionScanners(*scanners); !st.ok()) return st;

  absl::flat_hash_set<Point3> absolute_becons;
  for (const auto& s : *scanners) {
    for (const auto& b : s.relative_beacons) {
      absolute_becons.insert(s.RelativeToAbsolute(b));
    }
  }

  return IntReturn(absolute_becons.size());
}

absl::StatusOr<std::string> Day_2021_19::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Scanner>> scanners = Parse(input);
  if (!scanners.ok()) return scanners.status();

  if (auto st = PositionScanners(*scanners); !st.ok()) return st;

  int max_dist = 0;
  for (int i = 0; i < scanners->size(); ++i) {
    for (int j = i + 1; j < scanners->size(); ++j) {
      int dist = ((*scanners)[i].absolute - (*scanners)[j].absolute).dist();
      max_dist = std::max(max_dist, dist);
    }
  }

  return IntReturn(max_dist);
}

}  // namespace advent_of_code
