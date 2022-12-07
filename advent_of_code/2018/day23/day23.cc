// https://adventofcode.com/2018/day/23
//
// --- Day 23: Experimental Emergency Teleportation ---
// ----------------------------------------------------
// 
// Using your torch to search the darkness of the rocky cavern, you
// finally locate the man's friend: a small reindeer.
// 
// You're not sure how it got so far in this cave. It looks sick - too
// sick to walk - and too heavy for you to carry all the way back.
// Sleighs won't be invented for another 1500 years, of course.
// 
// The only option is experimental emergency teleportation.
// 
// You hit the "experimental emergency teleportation" button on the
// device and push I accept the risk on no fewer than 18 different
// warning messages. Immediately, the device deploys hundreds of tiny
// nanobots which fly around the cavern, apparently assembling themselves
// into a very specific formation. The device lists the X,Y,Z position (pos)
// for each nanobot as well as its signal radius (r) on its tiny screen
// (your puzzle input).
// 
// Each nanobot can transmit signals to any integer coordinate which is a
// distance away from it less than or equal to its signal radius (as
// measured by Manhattan distance). Coordinates a distance away of less
// than or equal to a nanobot's signal radius are said to be in range of
// that nanobot.
// 
// Before you start the teleportation process, you should determine which
// nanobot is the strongest (that is, which has the largest signal
// radius) and then, for that nanobot, the total number of nanobots that
// are in range of it, including itself.
// 
// For example, given the following nanobots:
// 
// pos=<0,0,0>, r=4
// pos=<1,0,0>, r=1
// pos=<4,0,0>, r=3
// pos=<0,2,0>, r=1
// pos=<0,5,0>, r=3
// pos=<0,0,3>, r=1
// pos=<1,1,1>, r=1
// pos=<1,1,2>, r=1
// pos=<1,3,1>, r=1
// 
// The strongest nanobot is the first one (position 0,0,0) because its
// signal radius, 4 is the largest. Using that nanobot's location and
// signal radius, the following nanobots are in or out of range:
// 
// * The nanobot at 0,0,0 is distance 0 away, and so it is in range.
// 
// * The nanobot at 1,0,0 is distance 1 away, and so it is in range.
// 
// * The nanobot at 4,0,0 is distance 4 away, and so it is in range.
// 
// * The nanobot at 0,2,0 is distance 2 away, and so it is in range.
// 
// * The nanobot at 0,5,0 is distance 5 away, and so it is not in
// range.
// 
// * The nanobot at 0,0,3 is distance 3 away, and so it is in range.
// 
// * The nanobot at 1,1,1 is distance 3 away, and so it is in range.
// 
// * The nanobot at 1,1,2 is distance 4 away, and so it is in range.
// 
// * The nanobot at 1,3,1 is distance 5 away, and so it is not in
// range.
// 
// In this example, in total, 7 nanobots are in range of the nanobot with
// the largest signal radius.
// 
// Find the nanobot with the largest signal radius. How many nanobots are
// in range of its signals?
//
// --- Part Two ---
// ----------------
// 
// Now, you just need to figure out where to position yourself so that
// you're actually teleported when the nanobots activate.
// 
// To increase the probability of success, you need to find the
// coordinate which puts you in range of the largest number of nanobots.
// If there are multiple, choose one closest to your position (0,0,0,
// measured by manhattan distance).
// 
// For example, given the following nanobot formation:
// 
// pos=<10,12,12>, r=2
// pos=<12,14,12>, r=2
// pos=<16,12,12>, r=4
// pos=<14,14,14>, r=6
// pos=<50,50,50>, r=200
// pos=<10,10,10>, r=5
// 
// Many coordinates are in range of some of the nanobots in this
// formation. However, only the coordinate 12,12,12 is in range of the
// most nanobots: it is in range of the first five, but is not in range
// of the nanobot at 10,10,10. (All other coordinates are in range of
// fewer than five nanobots.) This coordinate's distance from 0,0,0 is 36.
// 
// Find the coordinates that are in range of the largest number of
// nanobots. What is the shortest manhattan distance between any of those
// points and 0,0,0?


#include "advent_of_code/2018/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct NanoBot {
  Point3 p;
  int64_t r;

  bool operator==(const NanoBot& o) const { return p == o.p && r == o.r; }

  template <typename H>
  friend H AbslHashValue(H h, const NanoBot& b) {
    return H::combine(std::move(h), b.p, b.r);
  }
};

absl::StatusOr<std::vector<NanoBot>> Parse(
    absl::Span<absl::string_view> input) {
  std::vector<NanoBot> ret;
  for (absl::string_view row : input) {
    NanoBot nb;
    if (!RE2::FullMatch(row, "pos=<(-?\\d+,-?\\d+,-?\\d+)>, r=(-?\\d+)",
                        nb.p.Capture(), &nb.r)) {
      return Error("Bad input: ", row);
    }
    ret.push_back(nb);
  }
  return ret;
}

struct Cube {
  Point3 min;
  Point3 max;
};

struct CubeHits {
  Cube range;
  absl::flat_hash_set<NanoBot> bots;
};

absl::flat_hash_set<NanoBot> FindBots(const Cube& cube,
                                      const absl::flat_hash_set<NanoBot>& in) {
  absl::flat_hash_set<NanoBot> out;
  for (const NanoBot& test : in) {
    Point3 closest_in_cube = test.p;
    closest_in_cube.x = std::min(cube.max.x, closest_in_cube.x);
    closest_in_cube.x = std::max(cube.min.x, closest_in_cube.x);
    closest_in_cube.y = std::min(cube.max.y, closest_in_cube.y);
    closest_in_cube.y = std::max(cube.min.y, closest_in_cube.y);
    closest_in_cube.z = std::min(cube.max.z, closest_in_cube.z);
    closest_in_cube.z = std::max(cube.min.z, closest_in_cube.z);
    if ((closest_in_cube - test.p).dist() <= test.r) {
      out.insert(test);
    }
  }
  return out;
}

std::vector<CubeHits> SplitCube(const CubeHits& in, int candidate_size) {
  VLOG(2) << "SplitCube({" << in.range.min << "-" << in.range.max << "}, "
          << candidate_size << ")";
  std::vector<CubeHits> split;
  int64_t split_x = (in.range.max.x + in.range.min.x) / 2;
  int64_t split_y = (in.range.max.y + in.range.min.y) / 2;
  int64_t split_z = (in.range.max.z + in.range.min.z) / 2;
  Cube tmp;
  for (int z_split : {0, 1}) {
    if (z_split) {
      tmp.min.z = split_z + 1;
      tmp.max.z = in.range.max.z;
    } else {
      tmp.min.z = in.range.min.z;
      tmp.max.z = split_z;
    }
    if (tmp.max.z < tmp.min.z) continue;
    for (int y_split : {0, 1}) {
      if (y_split) {
        tmp.min.y = split_y + 1;
        tmp.max.y = in.range.max.y;
      } else {
        tmp.min.y = in.range.min.y;
        tmp.max.y = split_y;
      }
      if (tmp.max.y < tmp.min.y) continue;
      for (int x_split : {0, 1}) {
        if (x_split) {
          tmp.min.x = split_x + 1;
          tmp.max.x = in.range.max.x;
        } else {
          tmp.min.x = in.range.min.x;
          tmp.max.x = split_x;
        }
        if (tmp.max.x < tmp.min.x) continue;

        CubeHits test = {tmp, {}};
        test.bots = FindBots(tmp, in.bots);
        VLOG(3) << "  {" << x_split << "," << y_split << "," << z_split
                << "} = " << test.bots.size();
        if (candidate_size < test.bots.size()) {
          split.clear();
          candidate_size = test.bots.size();
          split.push_back(std::move(test));
        } else if (candidate_size == test.bots.size()) {
          split.push_back(std::move(test));
        }
      }
    }
  }
  VLOG(2) << "  = " << split.size();
  return split;
}

absl::StatusOr<Point3> FindBest(std::vector<NanoBot> bots_in) {
  CubeHits hits;
  hits.range.min = bots_in.begin()->p;
  hits.range.max = hits.range.min;
  for (const NanoBot& nb : bots_in) {
    hits.range.min.x = std::min(hits.range.min.x, nb.p.x);
    hits.range.min.y = std::min(hits.range.min.y, nb.p.y);
    hits.range.min.z = std::min(hits.range.min.z, nb.p.z);
    hits.range.max.x = std::max(hits.range.max.x, nb.p.x);
    hits.range.max.y = std::max(hits.range.max.y, nb.p.y);
    hits.range.max.z = std::max(hits.range.max.z, nb.p.z);
    hits.bots.insert(nb);
  }
  std::vector<CubeHits> candidates = {hits};
  std::vector<CubeHits> final;
  while (!candidates.empty()) {
    VLOG(1) << candidates.size() << "; "
            << (candidates[0].range.max - candidates[0].range.min).dist()
            << "; " << candidates[0].range.min << "-"
            << candidates[0].range.max;
    std::vector<CubeHits> new_candidates;
    int candidate_size = 0;
    for (const CubeHits& cube_hit : candidates) {
      std::vector<CubeHits> split = SplitCube(cube_hit, candidate_size);
      if (split.empty()) continue;

      if (candidate_size < split[0].bots.size()) {
        new_candidates.clear();
        candidate_size = split[0].bots.size();
      }
      for (CubeHits& hit : split) {
        if (hit.bots.size() != candidate_size) {
          return Error("Bad candidate size");
        }
        if (hit.range.min == hit.range.max) {
          final.push_back(std::move(hit));
        } else {
          new_candidates.push_back(std::move(hit));
        }
      }
    }
    candidates = std::move(new_candidates);
  }

  if (final.empty()) return Error("No final results");
  Point3 best = final[0].range.min;
  for (const CubeHits& hit : final) {
    if (hit.range.min != hit.range.max) {
      return Error("Integrity check");
    }
    if (best.dist() > hit.range.min.dist()) {
      best = hit.range.min;
    }
  }

  return best;
}

}  // namespace

absl::StatusOr<std::string> Day_2018_23::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<NanoBot>> bots = Parse(input);
  if (!bots.ok()) return bots.status();
  if (bots->empty()) return Error("No bots");

  NanoBot strongest = *bots->begin();
  for (const NanoBot& nb : *bots) {
    if (nb.r > strongest.r) strongest = nb;
  }
  int in_range = 0;
  for (const NanoBot& nb : *bots) {
    if ((nb.p - strongest.p).dist() <= strongest.r) {
      ++in_range;
    }
  }

  return IntReturn(in_range);
}

absl::StatusOr<std::string> Day_2018_23::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<NanoBot>> bots = Parse(input);
  if (!bots.ok()) return bots.status();
  if (bots->empty()) return Error("No bots");

  absl::StatusOr<Point3> best = FindBest(*bots);
  if (!best.ok()) return best.status();

  return IntReturn(best->dist());
}

}  // namespace advent_of_code
