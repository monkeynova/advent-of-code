#include "advent_of_code/2021/day22/day22.h"

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

  std::vector<Cube> SetDifference(const Cube& o) const {
    std::vector<Cube> ret = {*this};
    {
      // Split this cube along o.min.x if appropriate.
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.min.x > c.min.x && o.min.x <= c.max.x) {
          new_ret.push_back({c.min, {o.min.x - 1, c.max.y, c.max.z}});
          new_ret.push_back({{o.min.x, c.min.y, c.min.z}, c.max});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      // Split this cube along o.max.x if appropriate.
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.max.x >= c.min.x && o.max.x < c.max.x) {
          new_ret.push_back({c.min, {o.max.x, c.max.y, c.max.z}});
          new_ret.push_back({{o.max.x + 1, c.min.y, c.min.z}, c.max});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      // Split this cube along o.min.y if appropriate.
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.min.y > c.min.y && o.min.y <= c.max.y) {
          new_ret.push_back({c.min, {c.max.x, o.min.y - 1, c.max.z}});
          new_ret.push_back({{c.min.x, o.min.y, c.min.z}, c.max});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      // Split this cube along o.max.y if appropriate.
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.max.y >= c.min.y && o.max.y < c.max.y) {
          new_ret.push_back({c.min, {c.max.x, o.max.y, c.max.z}});
          new_ret.push_back({{c.min.x, o.max.y + 1, c.min.z}, c.max});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      // Split this cube along o.min.z if appropriate.
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.min.z > c.min.z && o.min.z <= c.max.z) {
          new_ret.push_back({c.min, {c.max.x, c.max.y, o.min.z - 1}});
          new_ret.push_back({{c.min.x, c.min.y, o.min.z}, c.max});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      // Split this cube along o.max.z if appropriate.
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.max.z >= c.min.z && o.max.z < c.max.z) {
          new_ret.push_back({c.min, {c.max.x, c.max.y, o.max.z}});
          new_ret.push_back({{c.min.x, c.min.y, o.max.z + 1}, c.max});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.FullyContains(c)) continue;
        CHECK(!o.Overlaps(c));
        new_ret.push_back(c);
      }
      ret = std::move(new_ret);
    }
    return ret;
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
      if (c.Overlaps(in)) {
        std::vector<Cube> split_c = c.SetDifference(in);
        new_set.insert(new_set.end(), split_c.begin(), split_c.end());
      } else {
        new_set.push_back(c);
      }
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
  std::vector<Cube> removed = c1.SetDifference(c2);
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
  absl::flat_hash_set<Point3> map;
  Cube bound{{-50, -50, -50}, {50, 50, 50}};
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
    if (!c.Overlaps(bound)) continue;
    c.min.x = std::max(c.min.x, bound.min.x);
    c.min.y = std::max(c.min.y, bound.min.y);
    c.min.z = std::max(c.min.z, bound.min.z);
    c.max.x = std::min(c.max.x, bound.max.x);
    c.max.y = std::min(c.max.y, bound.max.y);
    c.max.z = std::min(c.max.z, bound.max.z);
    VLOG(1) << c.min << "-" << c.max;
    for (Point3 p : c) {
      if (type == "on") {
        map.insert(p);
      } else if (type == "off") {
        map.erase(p);
      } else {
        return Error("Neither on nor off?");
      }
    }
  }
  return IntReturn(map.size());
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
