#include "advent_of_code/2017/day20/day20.h"

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

struct Particle {
  Point3 p;
  Point3 v;
  Point3 a;
};

std::ostream& operator<<(std::ostream& o, const Particle& p) {
  return o << "p:" << p.p << ", v:" << p.v << ", a:" << p.a;
}

absl::StatusOr<std::vector<Particle>> Parse(
    absl::Span<absl::string_view> input) {
  std::vector<Particle> particles;
  for (absl::string_view row : input) {
    Particle p;
    if (!RE2::FullMatch(
            row,
            "p=<(-?\\d+),(-?\\d+),(-?\\d+)>, v=<(-?\\d+),(-?\\d+),(-?\\d+)>, "
            "a=<(-?\\d+),(-?\\d+),(-?\\d+)>",
            &p.p.x, &p.p.y, &p.p.z, &p.v.x, &p.v.y, &p.v.z, &p.a.x, &p.a.y,
            &p.a.z)) {
      return AdventDay::Error("Bad line: ", row);
    }
    particles.push_back(p);
  }
  return particles;
}

bool SameDir(int a, int b) {
  if (a == 0)
    return b == 0;
  else if (a > 0)
    return b > 0;
  else
    return b < 0;
}

bool SameDir(Point3 a, Point3 b) {
  if (!SameDir(a.x, b.x)) return false;
  if (!SameDir(a.y, b.y)) return false;
  if (!SameDir(a.z, b.z)) return false;
  return true;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day_2017_20::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Particle>> particles = Parse(input);
  if (!particles.ok()) return particles.status();

  int min_acc = std::numeric_limits<int>::max();
  int min_vec = std::numeric_limits<int>::max();
  int min_idx = -1;
  for (int i = 0; i < particles->size(); ++i) {
    const Particle& p = particles->at(i);
    if (p.a.dist() < min_acc) {
      min_acc = p.a.dist();
      min_vec = (p.a - p.v).dist();
      min_idx = i;
    } else if (p.a.dist() == min_acc && (p.a - p.v).dist() < min_vec) {
      min_acc = p.a.dist();
      min_vec = (p.a - p.v).dist();
      min_idx = i;
    }
  }

  return IntReturn(min_idx);
}

absl::StatusOr<std::vector<std::string>> Day_2017_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Particle>> particles = Parse(input);
  if (!particles.ok()) return particles.status();

  std::vector<Particle> alive = *particles;
  // TODO(@monkeynova): 1'000 was picked after watching the output. This could
  //                    probably be determined in a more sophisticated manner.
  for (int step = 0; step < 1'000; ++step) {
    // Remove collisions.
    absl::flat_hash_map<Point3, int> position_to_count;
    bool to_remove = false;
    for (const Particle& p : alive) {
      if (++position_to_count[p.p] > 1) to_remove = true;
    }
    if (to_remove) {
      std::vector<Particle> new_alive;
      for (const Particle& p : alive) {
        if (position_to_count[p.p] == 1) new_alive.push_back(p);
      }
      if (new_alive.size() >= alive.size()) return Error("Didn't remove");
      VLOG(1) << "Removed: " << alive.size() - new_alive.size() << " @" << step;
      alive = std::move(new_alive);
    }
    // Update positions.
    for (Particle& p : alive) {
      p.v += p.a;
      p.p += p.v;
    }
  }

  return IntReturn(alive.size());
}

}  // namespace advent_of_code
