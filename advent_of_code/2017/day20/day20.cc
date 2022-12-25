#include "advent_of_code/2017/day20/day20.h"

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

struct Particle {
  Point3 p;
  Point3 v;
  Point3 a;

  Point3 AtTime(int t) const { return p + v * t + a * (t * (t + 1) / 2); }

  bool operator==(const Particle& o) const {
    return p == o.p && v == o.v && a == o.a;
  }

  template <typename H>
  friend H AbslHashValue(H h, const Particle& p) {
    return H::combine(std::move(h), p.p, p.v, p.a);
  }

  friend std::ostream& operator<<(std::ostream& o, const Particle& p) {
    return o << "p:" << p.p << ", v:" << p.v << ", a:" << p.a;
  }
};

absl::StatusOr<std::vector<Particle>> Parse(
    absl::Span<absl::string_view> input) {
  std::vector<Particle> particles;
  for (absl::string_view row : input) {
    Particle p;
    if (!RE2::FullMatch(
            row,
            "p=<(-?\\d+,-?\\d+,-?\\d+)>, v=<(-?\\d+,-?\\d+,-?\\d+)>, "
            "a=<(-?\\d+,-?\\d+,-?\\d+)>",
            p.p.Capture(), p.v.Capture(), p.a.Capture())) {
      return Error("Bad line: ", row);
    }
    particles.push_back(p);
  }
  return particles;
}

std::optional<int64_t> SquareRoot(int64_t n) {
  // n % 4 in {2, 3} => not square.
  if (n & 2) return std::nullopt;

  int64_t a = 0;
  int64_t b = std::min<int64_t>(0xffffffff, n / 2);
  if (n < 0) return std::nullopt;
  if (n < 2) return n;
  if (n < 10) b = n;
  while (a < b) {
    int64_t c = (a + b) / 2;
    int64_t c_2 = c * c;
    if (c_2 < n) {
      a = c + 1;
    } else if (c_2 > n) {
      b = c;
    } else {
      return c;
    }
  }
  return std::nullopt;
}

int IntersectPaths(const std::vector<Particle>& particles) {
  struct Intersection {
    int p_idx1;
    int p_idx2;
    int t;
  };

  // x1 = p1 + v1 * t + a1 * t * (t + 1) / 2
  // x2 = p2 + v2 * t + a2 * t * (t + 1) / 2
  //
  // (a1 - a2) t * (t + 1) / 2 + (v1 - v2) t + (p1 - p2) = 0
  // (a1 - a2) t^2 + ((a1 - a2) + 2(v1 - v2)) t + 2(p1 - p2) = 0;
  std::vector<Intersection> collisions;
  for (int p_idx1 = 0; p_idx1 < particles.size(); ++p_idx1) {
    const Particle& p1 = particles[p_idx1];
    for (int p_idx2 = p_idx1 + 1; p_idx2 < particles.size(); ++p_idx2) {
      const Particle& p2 = particles[p_idx2];

      int64_t a = p1.a.x - p2.a.x;
      int64_t b = a + 2 * (p1.v.x - p2.v.x);
      int64_t c = 2 * (p1.p.x - p2.p.x);
      if (a == 0 && b == 0 && c == 0) {
        a = p1.a.y - p2.a.y;
        b = a + 2 * (p1.v.y - p2.v.y);
        c = 2 * (p1.p.y - p2.p.y);
      }
      if (a == 0 && b == 0 && c == 0) {
        a = p1.a.z - p2.a.z;
        b = a + 2 * (p1.v.z - p2.v.z);
        c = 2 * (p1.p.z - p2.p.z);
      }

      if (a == 0) {
        if (b != 0 && -c % b == 0) {
          int t = -c / b;
          if (t > 0 && p1.AtTime(t) == p2.AtTime(t)) {
            collisions.push_back({p_idx1, p_idx2, t});
          }
        }
        continue;
      }

      std::optional<int64_t> root = SquareRoot(b * b - 4 * a * c);
      if (!root) continue;
      if ((-b - *root) % (2 * a) == 0) {
        int t = (-b - *root) / (2 * a);
        if (t > 0 && p1.AtTime(t) == p2.AtTime(t)) {
          collisions.push_back({p_idx1, p_idx2, t});
        }
      }
      if ((-b + *root) % (2 * a) == 0) {
        int t = (-b + *root) / (2 * a);
        if (t > 0 && p1.AtTime(t) == p2.AtTime(t)) {
          collisions.push_back({p_idx1, p_idx2, t});
        }
      }
    }
  }
  absl::flat_hash_set<int> dead;
  absl::c_sort(collisions, [](const Intersection& a, const Intersection& b) {
    return a.t < b.t;
  });

  int last_t = -1;
  std::vector<int> add_dead;
  for (const Intersection& i : collisions) {
    if (i.t != last_t && last_t >= 0) {
      int pre_dead_size = dead.size();
      for (int new_d : add_dead) dead.insert(new_d);
      VLOG(1) << "Removed: " << dead.size() - pre_dead_size << " @" << last_t;
      add_dead.clear();
    }
    add_dead.push_back(i.p_idx1);
    add_dead.push_back(i.p_idx2);
    last_t = i.t;
  }
  int pre_dead_size = dead.size();
  for (int new_d : add_dead) dead.insert(new_d);
  VLOG(1) << "Removed: " << dead.size() - pre_dead_size << " @" << last_t;

  return particles.size() - dead.size();
}

int IntersectSimulation(const std::vector<Particle>& particles) {
  std::vector<Particle> alive = particles;
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
      CHECK_LE(new_alive.size(), alive.size());
      VLOG(1) << "Removed: " << alive.size() - new_alive.size() << " @" << step;
      alive = std::move(new_alive);
    }
    // Update positions.
    for (Particle& p : alive) {
      p.v += p.a;
      p.p += p.v;
    }
  }

  return alive.size();
}

}  // namespace

absl::StatusOr<std::string> Day_2017_20::Part1(
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

  return AdventReturn(min_idx);
}

absl::StatusOr<std::string> Day_2017_20::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<Particle>> particles = Parse(input);
  if (!particles.ok()) return particles.status();

  if (run_audit()) {
    std::vector<Particle> progress = *particles;
    for (int step = 0; step < 100; ++step) {
      for (int i = 0; i < particles->size(); ++i) {
        if (progress[i].p != (*particles)[i].AtTime(step)) {
          return Error(progress[i].p, " != ", (*particles)[i].AtTime(step));
        }
        progress[i].v += progress[i].a;
        progress[i].p += progress[i].v;
      }
    }

    int paths_val = IntersectPaths(*particles);
    int simulation_val = IntersectSimulation(*particles);
    VLOG(1) << "paths = " << paths_val << "; simulation=" << simulation_val;
    if (paths_val != simulation_val) {
      return Error(paths_val, " != ", simulation_val);
    }
  }

  return AdventReturn(IntersectPaths(*particles));
}

}  // namespace advent_of_code
