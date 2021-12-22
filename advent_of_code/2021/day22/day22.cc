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
  int64_t Volume() const {
    return (max.x - min.x + 1ll) * (max.y - min.y + 1ll) * (max.z - min.z + 1ll);
  }
  bool Within(Point3 p) const {
    return p.x >= min.x && p.x <= max.x &&
       p.y >= min.y && p.y <= max.y &&
       p.z >= min.z && p.z <= max.z;
  }
  bool ContainedWithin(const Cube& o) const {
    return min.x >= o.min.x && max.x <= o.max.x &&
       min.y >= o.min.y && max.y <= o.max.y &&
       min.z >= o.min.z && max.z <= o.max.z;
  }
  bool Overlaps(const Cube& o) const {
    if (max.x < o.min.x) return false;
    if (min.x > o.max.x) return false;
    if (max.y < o.min.y) return false;
    if (min.y > o.max.y) return false;
    if (max.z < o.min.z) return false;
    if (min.z > o.max.z) return false;
    return true;
    return min.x >= o.max.x && max.x <= o.min.x &&
       min.y >= o.max.y && max.y <= o.min.y &&
       min.z >= o.max.z && max.z <= o.min.z;
  }

  std::vector<Cube> Remove(const Cube& o) const {
    VLOG(2) << "{" << min << "," << max << "} - {" << o.min << "," << o.max << "}";
    std::vector<Cube> ret = {*this};
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.min.x > c.min.x && o.min.x <= c.max.x) {
          new_ret.push_back({{c.min.x, c.min.y, c.min.z}, {o.min.x - 1, c.max.y, c.max.z}});
          new_ret.push_back({{o.min.x, c.min.y, c.min.z}, {c.max.x, c.max.y, c.max.z}});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.max.x >= c.min.x && o.max.x < c.max.x) {
          new_ret.push_back({{c.min.x, c.min.y, c.min.z}, {o.max.x, c.max.y, c.max.z}});
          new_ret.push_back({{o.max.x + 1, c.min.y, c.min.z}, {c.max.x, c.max.y, c.max.z}});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    for (const auto& c : ret) {
      VLOG(2) << "  post-x: {" << c.min << "," << c.max << "}";
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.min.y > c.min.y && o.min.y <= c.max.y) {
          new_ret.push_back({{c.min.x, c.min.y, c.min.z}, {c.max.x, o.min.y - 1, c.max.z}});
          new_ret.push_back({{c.min.x, o.min.y, c.min.z}, {c.max.x, c.max.y, c.max.z}});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.max.y >= c.min.y && o.max.y < c.max.y) {
          new_ret.push_back({{c.min.x, c.min.y, c.min.z}, {c.max.x, o.max.y, c.max.z}});
          new_ret.push_back({{c.min.x, o.max.y + 1, c.min.z}, {c.max.x, c.max.y, c.max.z}});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    for (const auto& c : ret) {
      VLOG(2) << "  post-y: {" << c.min << "," << c.max << "}";
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.min.z > c.min.z && o.min.z <= c.max.z) {
          new_ret.push_back({{c.min.x, c.min.y, c.min.z}, {c.max.x, c.max.y, o.min.z - 1}});
          new_ret.push_back({{c.min.x, c.min.y, o.min.z}, {c.max.x, c.max.y, c.max.z}});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (o.max.z >= c.min.z && o.max.z < c.max.z) {
          new_ret.push_back({{c.min.x, c.min.y, c.min.z}, {c.max.x, c.max.y, o.max.z}});
          new_ret.push_back({{c.min.x, c.min.y, o.max.z + 1}, {c.max.x, c.max.y, c.max.z}});
        } else {
          new_ret.push_back(c);
        }
      }
      ret = std::move(new_ret);
    }
    for (const auto& c : ret) {
      VLOG(2) << "  pre-crop: {" << c.min << "," << c.max << "}";
    }
    {
      std::vector<Cube> new_ret;
      for (const Cube& c : ret) {
        if (c.ContainedWithin(o)) continue;
        new_ret.push_back(c);
      }
      ret = std::move(new_ret);
    }
    for (const auto& c : ret) {
      VLOG(2) << "  post-crop: {" << c.min << "," << c.max << "}";
    }
    return ret;
  }
};

struct Space {
  void AddOn(Cube in) {
    std::vector<Cube> new_on;
    for (const Cube& c : on) {
      if (c.Overlaps(in)) {
        std::vector<Cube> new_c = c.Remove(in);
        for (const Cube& c2 : new_c) {
          new_on.push_back(c2);
        }
      } else {
        new_on.push_back(c);
      }
    }
    new_on.push_back(in);
    on = std::move(new_on);
  }

  void AddOff(Cube in) {
    std::vector<Cube> new_on;
    for (const Cube& c : on) {
      if (c.Overlaps(in)) {
        std::vector<Cube> new_c = c.Remove(in);
        for (const Cube& c2 : new_c) {
          new_on.push_back(c2);
        }
      } else {
        new_on.push_back(c);
      }
    }
    on = std::move(new_on);
  }

  int64_t Volume() {
    int64_t sum = 0;
    for (const Cube& c : on) {
      sum += c.Volume();
    }
    return sum;
  }

  std::vector<Cube> on;
};

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2021_22::Part1(
    absl::Span<absl::string_view> input) const {
  absl::flat_hash_set<Point3> map;
  for (absl::string_view line : input) {
    absl::string_view type;
    int x0, x1, y0, y1, z0, z1;
    if (!RE2::FullMatch(line, "(on|off) x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)",
     &type, &x0, &x1, &y0, &y1, &z0, &z1)) {
       return Error("Bad line: ", line);
     }
    x0 = std::max(x0, -50);
    x1 = std::min(x1, 50);
    y0 = std::max(y0, -50);
    y1 = std::min(y1, 50);
    z0 = std::max(z0, -50);
    z1 = std::min(z1, 50);
    for (int x = x0; x <= x1; ++x) {
      for (int y = y0; y <= y1; ++y) {
        for (int z = z0; z <= z1; ++z) {
          Point3 p{x, y, z};
          if (type == "on") map.insert(p);
          else map.erase(p); 
        }
      }
    }
  }
  return IntReturn(map.size());
}

absl::StatusOr<std::string> Day_2021_22::Part2(
    absl::Span<absl::string_view> input) const {
  if (run_audit()) {
    Cube c1({{10,10,10},{12,12,12}});
    Cube c2({{11,11,11},{13,13,13}});
    std::vector<Cube> removed = c1.Remove(c2);
    for (int x = 10; x <= 12; ++x) {
      for (int y = 10; y <= 12; ++y) {
        for (int z = 10; z <= 12; ++z) {
          Point3 p{x, y, z};
          bool should_in = c1.Within(p) && !c2.Within(p);
          int in_count = 0;
          for (const Cube& r : removed) {
            if (r.Within(p)) {
              ++in_count;
            }
          }
          if (in_count != should_in ? 1 : 0) {
            VLOG(1) << "Bad point: " << p << "; should=" << (should_in ? "in" : "out");
            return Error("Bad point");
          }
        } 
      }
    }
  }
  Space s;
  int i = 0;
  for (absl::string_view line : input) {
    VLOG(1) << i << "/" << input.size();
    ++i;
    absl::string_view type;
    int x0, x1, y0, y1, z0, z1;
    if (!RE2::FullMatch(line, "(on|off) x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)",
     &type, &x0, &x1, &y0, &y1, &z0, &z1)) {
       return Error("Bad line: ", line);
     }
    if (type == "on") {
      s.AddOn(Cube{{x0, y0, z0}, {x1, y1, z1}});
    } else {
      s.AddOff(Cube{{x0, y0, z0}, {x1, y1, z1}});
    }
  }
  return IntReturn(s.Volume());
}

}  // namespace advent_of_code
