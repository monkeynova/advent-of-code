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

std::optional<int64_t> Solve(const std::vector<Hail>& hail, double min, double max) {
  // (apx - hp1x)(avy - hv1y) = (apy - hp1y)(avx - hv1x)

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
        
        Point64 ans_p = {
          static_cast<int64_t>(roundl(apx)),
          static_cast<int64_t>(roundl(apy)),
          static_cast<int64_t>(roundl(apz))
        };
        Point64 ans_v = {
          static_cast<int64_t>(roundl(avx)),
          static_cast<int64_t>(roundl(avy)),
          static_cast<int64_t>(roundl(avz))
        };

        bool all_match = true;
        for (const Hail& h : hail) {
          absl::int128 dpx = ans_p.x - h.p.x;
          absl::int128 dpy = ans_p.y - h.p.y;
          absl::int128 dpz = ans_p.z - h.p.z;
          absl::int128 dvx = ans_v.x - h.v.x;
          absl::int128 dvy = ans_v.y - h.v.y;
          absl::int128 dvz = ans_v.z - h.v.z;
          if (dpx * dvy != dpy * dvx) {
            all_match = false;
            break;
          }
          if (dpz * dvy != dpy * dvz) {
            all_match = false;
            break;
          }
          if (dpz * dvx != dpx * dvz) {
            all_match = false;
            break;
          }
        }

        if (all_match) {
          VLOG(1) << apx << "," << apy << "," << apz;
          VLOG(1) << avx << "," << avy << "," << avz;
          return ans_p.x + ans_p.y + ans_p.z;
        } 
      }
    }
  }

  return std::nullopt;
}

std::optional<int64_t> Solve(const std::vector<Hail>& hail) {
  for (int range = 20; range < 100000; range *= 2) {
    std::optional<int64_t> ret = Solve(hail, -range, range);
    if (ret) return ret;
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
}

}  // namespace advent_of_code
