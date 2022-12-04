#include "advent_of_code/2021/day22/day22.h"

#include "absl/algorithm/container.h"
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

struct Cube {
  Point3 min;
  Point3 max;

  struct iterator {
    Point3 cur = Point3{0, 0, 0};
    const Cube* base = nullptr;

    iterator operator++() {
      if (++cur.x > base->max.x) {
        cur.x = base->min.x;
        if (++cur.y > base->max.y) {
          cur.y = base->min.y;
          if (++cur.z > base->max.z) {
            base = nullptr;
            cur = Point3{0, 0, 0};
          }
        }
      }
      return *this;
    }

    bool operator==(const iterator& o) const {
      return base == o.base && cur == o.cur;
    }
    bool operator!=(const iterator& o) const { return !operator==(o); }

    Point3 operator*() const { return cur; }
  };

  iterator begin() const { return iterator{.cur = min, .base = this}; }
  iterator end() const { return iterator{}; }

  int64_t Volume() const {
    // 1ll forces the addition to return int64 before multiplying.
    return (max.x - min.x + 1ll) * (max.y - min.y + 1ll) *
           (max.z - min.z + 1ll);
  }
  bool Contains(Point3 p) const {
    if (p.x < min.x) return false;
    if (p.x > max.x) return false;
    if (p.y < min.y) return false;
    if (p.y > max.y) return false;
    if (p.z < min.z) return false;
    if (p.z > max.z) return false;
    return true;
  }
  bool FullyContains(const Cube& o) const {
    if (o.min.x < min.x) return false;
    if (o.max.x > max.x) return false;
    if (o.min.y < min.y) return false;
    if (o.max.y > max.y) return false;
    if (o.min.z < min.z) return false;
    if (o.max.z > max.z) return false;
    return true;
  }
  bool Overlaps(const Cube& o) const {
    if (max.x < o.min.x) return false;
    if (min.x > o.max.x) return false;
    if (max.y < o.min.y) return false;
    if (min.y > o.max.y) return false;
    if (max.z < o.min.z) return false;
    if (min.z > o.max.z) return false;
    return true;
  }

  absl::optional<Cube> Intersect(const Cube& o) const {
    if (!Overlaps(o)) return {};
    return Cube{
        {std::max(min.x, o.min.x), std::max(min.y, o.min.y),
         std::max(min.z, o.min.z)},
        {std::min(max.x, o.max.x), std::min(max.y, o.max.y),
         std::min(max.z, o.max.z)},
    };
  }

  void SetDifference(const Cube& o, std::vector<Cube>* out) const {
    if (!Overlaps(o)) {
      out->push_back(*this);
      return;
    }

    Cube overlap = *this;
    std::vector<Cube> ret;
    // Cleave off sub-cubes that cannot overlap with o, until a fully contained
    // sub-cube is all that remains, and then discard it.
    for (int Point3::*dim : {&Point3::x, &Point3::y, &Point3::z}) {
      if (o.min.*dim > overlap.min.*dim && o.min.*dim <= overlap.max.*dim) {
        Cube cleave = overlap;
        cleave.max.*dim = o.min.*dim - 1;
        out->push_back(cleave);
        overlap.min.*dim = o.min.*dim;
      }
      if (o.max.*dim >= overlap.min.*dim && o.max.*dim < overlap.max.*dim) {
        Cube cleave = overlap;
        cleave.min.*dim = o.max.*dim + 1;
        out->push_back(cleave);
        overlap.max.*dim = o.max.*dim;
      }
    }
    CHECK(o.FullyContains(overlap));
  }
};

struct CubeSet {
 public:
  CubeSet() = default;

  void Union(Cube in) {
    SetDifference(in);
    set_.push_back(in);
  }

  void SetDifference(Cube in) {
    std::vector<Cube> new_set;
    for (const Cube& c : set_) {
      c.SetDifference(in, &new_set);
    }
    set_ = std::move(new_set);
  }

  int64_t Volume() const {
    int64_t sum = 0;
    for (const Cube& c : set_) {
      sum += c.Volume();
    }
    return sum;
  }

 private:
  std::vector<Cube> set_;
};

absl::Status Audit() {
  Cube c1({{10, 10, 10}, {12, 12, 12}});
  Cube c2({{11, 11, 11}, {13, 13, 13}});
  std::vector<Cube> removed;
  c1.SetDifference(c2, &removed);
  for (Point3 p : c1) {
    bool should_in = c1.Contains(p) && !c2.Contains(p);
    int in_count = 0;
    for (const Cube& r : removed) {
      if (r.Contains(p)) {
        ++in_count;
      }
    }
    if (in_count != should_in ? 1 : 0) {
      VLOG(1) << "Bad point: " << p
              << "; should=" << (should_in ? "in" : "out");
      return Error("Bad point");
    }
  }
  return absl::OkStatus();
}

}  // namespace

absl::StatusOr<std::string> Day_2021_22::Part1(
    absl::Span<absl::string_view> input) const {
  Cube bound{{-50, -50, -50}, {50, 50, 50}};
  CubeSet s;
  absl::optional<absl::flat_hash_set<Point3>> map;
  if (run_audit()) {
    map = absl::flat_hash_set<Point3>{};
  }
  for (absl::string_view line : input) {
    absl::string_view type;
    int x0, x1, y0, y1, z0, z1;
    if (!RE2::FullMatch(
            line,
            "(on|off) "
            "x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)",
            &type, &x0, &x1, &y0, &y1, &z0, &z1)) {
      return Error("Bad line: ", line);
    }

    absl::optional<Cube> c = bound.Intersect(Cube{{x0, y0, z0}, {x1, y1, z1}});
    if (!c) continue;
    if (type == "on") {
      if (map)
        for (Point3 p : *c) map->insert(p);
      s.Union(*c);
    } else if (type == "off") {
      if (map)
        for (Point3 p : *c) map->erase(p);
      s.SetDifference(*c);
    } else {
      return Error("Neither on nor off?");
    }
  }
  if (map) {
    if (map->size() != s.Volume()) {
      return Error("map->size() != s.Volumne(): ", map->size(),
                   " != ", s.Volume());
    }
  }
  return IntReturn(s.Volume());
}

absl::StatusOr<std::string> Day_2021_22::Part2(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    if (auto st = Audit(); !st.ok()) return st;
  }
  CubeSet s;
  for (absl::string_view line : input) {
    absl::string_view type;
    int x0, x1, y0, y1, z0, z1;
    if (!RE2::FullMatch(
            line,
            "(on|off) "
            "x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)",
            &type, &x0, &x1, &y0, &y1, &z0, &z1)) {
      return Error("Bad line: ", line);
    }
    Cube c{{x0, y0, z0}, {x1, y1, z1}};
    if (type == "on") {
      s.Union(c);
    } else if (type == "off") {
      s.SetDifference(c);
    } else {
      return Error("Neither on nor off?");
    }
  }
  return IntReturn(s.Volume());
}

}  // namespace advent_of_code
