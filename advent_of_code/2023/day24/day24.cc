// http://adventofcode.com/2023/day/24

#include "advent_of_code/2023/day24/day24.h"

#include "absl/algorithm/container.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/bfs.h"
#include "advent_of_code/char_board.h"
#include "advent_of_code/conway.h"
#include "advent_of_code/directed_graph.h"
#include "advent_of_code/fast_board.h"
#include "advent_of_code/graph_walk.h"
#include "advent_of_code/interval.h"
#include "advent_of_code/loop_history.h"
#include "advent_of_code/mod.h"
#include "advent_of_code/point.h"
#include "advent_of_code/point3.h"
#include "advent_of_code/point_walk.h"
#include "advent_of_code/splice_ring.h"
#include "advent_of_code/tokenizer.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

struct DPoint {
  double x;
  double y;
};

struct DPoint3 {
  long double x;
  long double y;
  long double z;
};

struct Point64 {
  int64_t x;
  int64_t y;
  int64_t z;

  DPoint XY() const {
    return {
      static_cast<double>(x),
      static_cast<double>(y)
    };
  }

  DPoint3 ToDPoint3() const {
    return {
      static_cast<double>(x),
      static_cast<double>(y),
      static_cast<double>(z)
    };
  }
};

struct DLine {
  DPoint p;
  DPoint d;

  std::optional<DPoint> Intersect(DLine o) const {
    // d1.p + t1 * d1.d = d2.p + t2 * d2.d
    // t1 * d1.d - t2 * d2.d = d2.p - d1.p
    // [d1.d.x -d2.d.x] [t1]   [d2.p.x - d1.p.x]
    //                *      =
    // [d1.d.y -d2.d.y] [t2]   [d1.p.y - d1.p.y]
    //auto transform = StaticMatrix<double, 2, 2>({
    //    {d.x, -o.d.x},
    //    {d.y, -o.d.y},
    //});
    double det = d.x * -o.d.y - -o.d.x * d.y;
    if (det == 0) return std::nullopt;

    std::array<double, 2> t = {
      -o.d.y / det * (o.p.x - p.x) - (-o.d.x) / det * (o.p.y - p.y),
      -d.y / det * (o.p.x - p.x) + d.x / det * (o.p.y - p.y),
    };
    if (t[0] < 0) return std::nullopt;
    if (t[1] < 0) return std::nullopt;
    DPoint p_t1 = {p.x + t[0] * d.x, p.y + t[0] * d.y};
    // DPoint p_t2 = {o.p.x + t[1] * o.d.x, p.y + t[1] * o.d.y};
    //CHECK_NEAR(p_t1.x, p_t2.x, 1e-10);
    //CHECK_NEAR(p_t1.y, p_t2.y, 1e-10);
    return p_t1;
  }
};

struct Hail {
  Point64 p;
  Point64 v;
};

std::optional<int64_t> GradientDescent(const std::vector<Hail>& hail) {
  // pt + vt * tN = pN + vN * tN
  // (p - p0) = (v0 - v) * t

  // (px - p0x) / (vx - v0x) = (py - p0y) / (vy - v0y) = (pz - p0z) / (vz - v0z)

  // (px - p0x)(vy - v0y) - (py - p0y)(vx - v0x) = 0     [1]
  // ((px - p0x)(vy - v0y) - (py - p0y)(vx - v0x))^2 = 0 [2]

  // d/d(px) = 2([1])(vy - v0y)
  // d/d(py) = -2([1])(vx - v0x)
  // d/d(vx) = -2([1])(py - p0y)
  // d/d(vy) = 2([1])(px - p0x)

  bool big = hail.size() > 100;

  const long double kDeltaPEpsilon = big ? 1e-10 : 1e-5;
  const long double kDeltaVEpsilon = big ? 1e-35 : 1e-5;
  const long double kThreshEpsilon = big ? 1e-1 : 1e-10;
  for (int h = 0; h < hail.size(); ++h) {
    VLOG(1) << "New guess";
    DPoint3 guess_p = {0, 0, 0};
    DPoint3 guess_v = hail[h].v.ToDPoint3();

    for (int i = 0; !std::isnan(guess_p.x); ++i) {
      VLOG_IF(1, i % 100000 == 0) << guess_p.x << "," << guess_p.y << "," << guess_p.z;
      VLOG_IF(1, i % 100000 == 0) << guess_v.x << "," << guess_v.y << "," << guess_v.z;
      DPoint3 delta_p = {0, 0, 0};
      DPoint3 delta_v = {0, 0, 0};
  
      double error = 0;
      for (const Hail& h : hail) {
        {
          double dot = (guess_p.x - h.p.x) * (guess_v.y - h.v.y) - (guess_p.y - h.p.y) * (guess_v.x - h.v.x);
          error += dot * dot;
          delta_p.x -= kDeltaPEpsilon * dot * (guess_v.y - h.v.y);
          delta_p.y += kDeltaPEpsilon * dot * (guess_v.x - h.v.x);
          delta_v.x += kDeltaVEpsilon * dot * (guess_p.y - h.p.y);
          delta_v.y -= kDeltaVEpsilon * dot * (guess_p.x - h.p.x);
        }
        {
          double dot = (guess_p.x - h.p.x) * (guess_v.z - h.v.z) - (guess_p.z - h.p.z) * (guess_v.x - h.v.x);
          error += dot * dot;
          delta_p.x -= kDeltaPEpsilon * dot * (guess_v.z - h.v.z);
          delta_p.z += kDeltaPEpsilon * dot * (guess_v.x - h.v.x);
          delta_v.x += kDeltaVEpsilon * dot * (guess_p.z - h.p.z);
          delta_v.z -= kDeltaVEpsilon * dot * (guess_p.x - h.p.x);
        }
        {
          double dot = (guess_p.y - h.p.y) * (guess_v.z - h.v.z) - (guess_p.z - h.p.z) * (guess_v.y - h.v.y);
          error += dot * dot;
          delta_p.y -= kDeltaPEpsilon * dot * (guess_v.z - h.v.z);
          delta_p.z += kDeltaPEpsilon * dot * (guess_v.y - h.v.y);
          delta_v.y += kDeltaVEpsilon * dot * (guess_p.z - h.p.z);
          delta_v.z -= kDeltaVEpsilon * dot * (guess_p.y - h.p.y);
        }
      }
      VLOG_IF(1, i % 100000 == 0) << error;
  
      if (i > 10 && 
          delta_p.x < kThreshEpsilon && delta_p.y < kThreshEpsilon && delta_p.z < kThreshEpsilon &&
          delta_v.x < kThreshEpsilon && delta_v.y < kThreshEpsilon && delta_v.z < kThreshEpsilon) {
        Point64 ans_p = {static_cast<int64_t>(roundl(guess_p.x)), static_cast<int64_t>(roundl(guess_p.y)), static_cast<int64_t>(roundl(guess_p.z))};
        Point64 ans_v = {static_cast<int64_t>(roundl(guess_v.x)), static_cast<int64_t>(roundl(guess_v.y)), static_cast<int64_t>(roundl(guess_v.z))};

        VLOG(1) << i << ": " << ans_p.x << "," << ans_p.y << "," << ans_p.z;
        VLOG(1) << i << ": " << ans_v.x << "," << ans_v.y << "," << ans_v.z;
        bool valid = true;
        for (const Hail& h : hail) {
          absl::int128 dpx = ans_p.x - h.p.x;
          absl::int128 dpy = ans_p.y - h.p.y;
          absl::int128 dpz = ans_p.z - h.p.z;
          absl::int128 dvx = ans_v.x - h.v.x;
          absl::int128 dvy = ans_v.y - h.v.y;
          absl::int128 dvz = ans_v.z - h.v.z;
          if (dpx * dvy != dpy * dvx) {
            VLOG(1) << dpx * dvy << " != " << dpy * dvx;
            valid = false;
          }
          if (dpz * dvy != dpy * dvz) {
            VLOG(1) << dpz * dvy << " != " << dpy * dvz;
            valid = false;
          }
          if (dpz * dvx != dpx * dvz) {
            VLOG(1) << dpz * dvx << " != " << dpx * dvz;
            valid = false;
          }
        }
        if (!valid) {
          VLOG(1) << "  Invalid";
          break;
        }

        return ans_p.x + ans_p.y + ans_p.z;
      }
      guess_p.x += delta_p.x;
      guess_p.y += delta_p.y;
      guess_p.z += delta_p.z;
      guess_v.x += delta_v.x;
      guess_v.y += delta_v.y;
      guess_v.z += delta_v.z;
    }
  }

  return std::nullopt;
}

std::optional<int64_t> Solve(const std::vector<Hail>& hail) {
  // (apx - hp1x)(avy - hv1y) = (apy - hp1y)(avx - hv1x)
  // (apx - hp2x)(avy - hv2y) = (apy - hp2y)(avx - hv2x)
  
  // hp2x(avy - hv2y)(avy - hv1y) - hp1x(avy - hv1y)(avy - hv2y) = 
  //    (apy - hp1y)(avx - hv1x)(avy - hv2y) - (apy - hp2y)(avx - hv2x)(avy - hv1y) 
  //    apy[(avx - hv1x)(avy - hv2y) - (avx - hv2x)(avy - hv1y)] + hp2y(avx - hv2x)(avy - hv1y) - hp1y(avx - hv1x)(avy - hv2y)

  // hp2x(avy - hv2y)(avy - hv1y) - hp1x(avy - hv1y)(avy - hv2y) - hp2y(avx - hv2x)(avy - hv1y) + hp1y(avx - hv1x)(avy - hv2y) =
  //   apy[(avx - hv1x)(avy - hv2y) - (avx - hv2x)(avy - hv1y)]

/*

           ((h2.p.y - h1.p.y) * (avx - h2.v.x) * (avx - h1.v.x) -
            h2.p.x * (avy - h2.v.y) * (avx - h1.v.x) + h1.p.x * (avy - h1.v.y) * (avx - h2.v.x)) /
           ((avy - h1.v.y)*(avx - h2.v.x) - (avy - h2.v.y) * (avx - h1.v.x))

           =

           ((h3.p.y - h1.p.y) * (avx - h3.v.x) * (avx - h1.v.x) -
            h3.p.x * (avy - h3.v.y) * (avx - h1.v.x) + h1.p.x * (avy - h1.v.y) * (avx - h3.v.x)) /
           ((avy - h1.v.y)*(avx - h3.v.x) - (avy - h3.v.y) * (avx - h1.v.x));

*/

  double min = -300;
  double max = 300;

  for (long double avx = min; avx <= max; ++avx) {
    for (long double avy = min; avy <= max; ++avy) {
        Hail h1;
        Hail h2;
        bool set_h1 = false;
        bool set_h2 = false;
        for (const Hail& h : hail) {
          if (h.v.x == avx) continue;
          if (h.v.y == avy) continue;
          if (set_h1) {
            h2 = h;
            set_h2 = true;
            break;
          }
          h1 = h;
          set_h1 = true;
        }
        if (!set_h2) continue;
        long double apy = 
           ((h2.p.x - h1.p.x) * (avy - h2.v.y) * (avy - h1.v.y) -
            h2.p.y * (avx - h2.v.x) * (avy - h1.v.y) + h1.p.y * (avx - h1.v.x) * (avy - h2.v.y)) /
            ((avx - h1.v.x)*(avy - h2.v.y) - (avx - h2.v.x) * (avy - h1.v.y));
        long double apx = 
           ((h2.p.y - h1.p.y) * (avx - h2.v.x) * (avx - h1.v.x) -
            h2.p.x * (avy - h2.v.y) * (avx - h1.v.x) + h1.p.x * (avy - h1.v.y) * (avx - h2.v.x)) /
           ((avy - h1.v.y)*(avx - h2.v.x) - (avy - h2.v.y) * (avx - h1.v.x));

        bool all_match = true;
        for (const Hail& h : hail) {
          if (abs((apx - h.p.x) * (avy - h.v.y) - (apy - h.p.y) * (avx - h.v.x)) > 1e-2) {
            all_match = false;
            break;
          }
        }
        if (!all_match) continue;

      VLOG(1) << "v_xy = " << avx << "," << avy;

      for (long double avz = min; avz <= max; ++avz) {
        Hail h1;
        Hail h2;
        bool set_h1 = false;
        bool set_h2 = false;
        for (const Hail& h : hail) {
          if (h.v.x == avx) continue;
          if (h.v.y == avy) continue;
          if (h.v.z == avz) continue;
          if (set_h1) {
            h2 = h;
            set_h2 = true;
            break;
          }
          h1 = h;
          set_h1 = true;
        }
        if (!set_h2) continue;

        long double apz = 
           ((h2.p.x - h1.p.x) * (avz - h2.v.z) * (avz - h1.v.z) -
            h2.p.z * (avx - h2.v.x) * (avz - h1.v.z) + h1.p.z * (avx - h1.v.x) * (avz - h2.v.z)) /
           ((avx - h1.v.x)*(avz - h2.v.z) - (avx - h2.v.x) * (avz - h1.v.z));
        
        if (avx == -3 && avy == 1 && avz == 2) {
          VLOG(1) << apx << "," << apy << "," << apz;
          VLOG(1) << avx << "," << avy << "," << avz;
        }

        bool all_match = true;
        for (const Hail& h : hail) {
          if (abs((apx - h.p.x) * (avy - h.v.y) - (apy - h.p.y) * (avx - h.v.x)) > 1e-2) {
            all_match = false;
            break;
          }
          if (abs((apz - h.p.z) * (avy - h.v.y) - (apy - h.p.y) * (avz - h.v.z)) > 1e-2) {
            all_match = false;
            break;
          }
          if (abs((apz - h.p.z) * (avx - h.v.x) - (apx - h.p.x) * (avz - h.v.z)) > 1e-2) {
            all_match = false;
            break;
          }
        }
        if (all_match) {
          VLOG(1) << apx << "," << apy << "," << apz;
          VLOG(1) << avx << "," << avy << "," << avz;

          Point64 ans_p = {static_cast<int64_t>(roundl(apx)), static_cast<int64_t>(roundl(apy)), static_cast<int64_t>(roundl(apz))};
          Point64 ans_v = {static_cast<int64_t>(roundl(avx)), static_cast<int64_t>(roundl(avy)), static_cast<int64_t>(roundl(avz))};

        for (const Hail& h : hail) {
          absl::int128 dpx = ans_p.x - h.p.x;
          absl::int128 dpy = ans_p.y - h.p.y;
          absl::int128 dpz = ans_p.z - h.p.z;
          absl::int128 dvx = ans_v.x - h.v.x;
          absl::int128 dvy = ans_v.y - h.v.y;
          absl::int128 dvz = ans_v.z - h.v.z;
          CHECK_EQ(dpx * dvy, dpy * dvx);
          CHECK_EQ(dpz * dvy, dpy * dvz);
          CHECK_EQ(dpz * dvx, dpx * dvz);
        }

          return ans_p.x + ans_p.y + ans_p.z;
        } 
      }
    }
  }

  return std::nullopt;
}

}  // namespace

absl::StatusOr<std::string> Day_2023_24::Part1(
    absl::Span<std::string_view> input) const {
  auto [min_str, max_str] = PairSplit(param(), ",");
  int64_t min;
  int64_t max;
  if (!absl::SimpleAtoi(min_str, &min)) return Error("Bad min");
  if (!absl::SimpleAtoi(max_str, &max)) return Error("Bad max");
  VLOG(1) << "[" << min << "," << max << "]";
  struct Hail {
    Point64 p;
    Point64 v;
  };
  std::vector<Hail> hail;
  for (std::string_view line : input) {
    Hail h;
    Tokenizer tok(line);
    ASSIGN_OR_RETURN(h.p.x, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.p.y, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.p.z, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs("@"));
    ASSIGN_OR_RETURN(h.v.x, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.v.y, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.v.z, tok.NextInt());
    if (!tok.Done()) return Error("Bad line: ", line);
    hail.push_back(h);
  }
  int intersections = 0;
  for (int i = 0; i < hail.size(); ++i) {
    DPoint p1 = hail[i].p.XY();
    DPoint v1 = hail[i].v.XY();
    DLine l1 = {p1, v1};

    for (int j = 0; j < i; ++j) {
      DPoint p2 = hail[j].p.XY();
      DPoint v2 = hail[j].v.XY();
      DLine l2 = {p2, v2};

      std::optional<DPoint> intersect = l1.Intersect(l2);
      if (!intersect) continue;
      VLOG(2) << "Intersect @ " << intersect->x << "," << intersect->y;
      if (intersect->x >= min && intersect->x <= max &&
          intersect->y >= min && intersect->y <= max) {
        VLOG(2) << "Add " << i;
        VLOG(2) << "Add " << j;
        ++intersections;
      }
    }
  }
  return AdventReturn(intersections);
}

absl::StatusOr<std::string> Day_2023_24::Part2(
    absl::Span<std::string_view> input) const {
  std::vector<Hail> hail;
  for (std::string_view line : input) {
    Hail h;
    Tokenizer tok(line);
    ASSIGN_OR_RETURN(h.p.x, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.p.y, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.p.z, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs("@"));
    ASSIGN_OR_RETURN(h.v.x, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.v.y, tok.NextInt());
    RETURN_IF_ERROR(tok.NextIs(","));
    ASSIGN_OR_RETURN(h.v.z, tok.NextInt());
    if (!tok.Done()) return Error("Bad line: ", line);
    hail.push_back(h);
  }

  return AdventReturn(Solve(hail));

  return AdventReturn(GradientDescent(hail));
}

}  // namespace advent_of_code
