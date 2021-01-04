#include "advent_of_code/2018/day23/day23.h"

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

struct NanoBot {
  Point3 p;
  int64_t r;

  bool operator==(const NanoBot& o) const {
    return p == o.p && r == o.r;
  }

  template <typename H>
  friend H AbslHashValue(H h, const NanoBot& b) {
    return H::combine(std::move(h), b.p, b.r);
  }
};

absl::StatusOr<std::vector<NanoBot>> Parse(absl::Span<absl::string_view> input) {
  std::vector<NanoBot> ret;
  for (absl::string_view row : input) {
    NanoBot nb;
    if (!RE2::FullMatch(row, "pos=<(-?\\d+),(-?\\d+),(-?\\d+)>, r=(-?\\d+)",
                        &nb.p.x, &nb.p.y, &nb.p.z, &nb.r)) {
      return AdventDay::Error("Bad input: ", row);
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

absl::flat_hash_set<NanoBot> FindBots(
    const Cube& cube, const absl::flat_hash_set<NanoBot>& in) {
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
  VLOG(2) << "SplitCube({" << in.range.min << "-" << in.range.max <<"}, " << candidate_size << ")";
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
        VLOG(3) << "  {" << x_split << "," << y_split << "," << z_split << "} = " << test.bots.size();
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
  for (const NanoBot &nb : bots_in) {
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
    VLOG(1) << candidates.size() << "; " << (candidates[0].range.max - candidates[0].range.min).dist()
            << "; " << candidates[0].range.min << "-" << candidates[0].range.max;
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
          return AdventDay::Error("Bad candidate size");
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

  if (final.empty()) return AdventDay::Error("No final results");
  Point3 best = final[0].range.min;
  for (const CubeHits& hit : final) {
    if (hit.range.min != hit.range.max) {
      return AdventDay::Error("Integrity check");
    }
    if (best.dist() > hit.range.min.dist()) {
      best = hit.range.min;
    }
  }
  return best;

  /*
  // Brute Force!
  Point3 best;
  int in_range = 0;
  Point3 p = min;
  VLOG(1) << "Searching: " << min << " to " << max;
  for (p.z = min.z; p.z <= max.z; ++p.z) {
    for (p.y = min.y; p.y <= max.y; ++p.y) {
      for (p.x = min.x; p.x <= max.x; ++p.x) {
        int this_in_range = 0;
        for (const NanoBot& b : bots_in) {
          if ((b.p - p).dist() <= b.r) {
            ++this_in_range;
          }
        }
        if (this_in_range > in_range) {
          VLOG(2) << "  Found: " << this_in_range << " @" << p;
          in_range = this_in_range;
          best = p;
        } else if (this_in_range == in_range && p.dist() < best.dist()) {
          VLOG(2) << "  Found: " << this_in_range << " @" << p;
          in_range = this_in_range;
          best = p;
        }
      }
    } 
  }

  return best;
  */
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day23_2018::Part1(
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

absl::StatusOr<std::vector<std::string>> Day23_2018::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<NanoBot>> bots = Parse(input);
  if (!bots.ok()) return bots.status();
  if (bots->empty()) return Error("No bots");

  absl::StatusOr<Point3> best = FindBest(*bots);
  if (!best.ok()) return best.status();

  return IntReturn(best->dist());
}

}  // namespace advent_of_code
