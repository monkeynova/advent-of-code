// http://adventofcode.com/2023/day/24

#include "advent_of_code/2023/day24/day24.h"

#include "absl/log/log.h"
#include "advent_of_code/tokenizer.h"

namespace advent_of_code {

namespace {

struct Point64 {
  int64_t x;
  int64_t y;
  int64_t z;
};

struct Hail {
  Point64 p;
  Point64 v;

  bool IntersectXYInRange(Hail o, int64_t min, int64_t max) const {
    // d1.p + t1 * d1.d = d2.p + t2 * d2.d
    // t1 * d1.d - t2 * d2.d = d2.p - d1.p
    // [d1.d.x -d2.d.x] [t1]   [d2.p.x - d1.p.x]
    //                *      =
    // [d1.d.y -d2.d.y] [t2]   [d1.p.y - d1.p.y]
    //auto transform = StaticMatrix<double, 2, 2>({
    //    {d.x, -o.d.x},
    //    {d.y, -o.d.y},
    //});
    double det = v.x * -o.v.y - -o.v.x * v.y;
    if (det == 0) return false;

    std::array<double, 2> t = {
      -o.v.y / det * (o.p.x - p.x) - (-o.v.x) / det * (o.p.y - p.y),
      -v.y / det * (o.p.x - p.x) + v.x / det * (o.p.y - p.y),
    };
    if (t[0] < 0) return false;
    if (t[1] < 0) return false;
    if (p.x + t[0] * v.x < min) return false;
    if (p.x + t[0] * v.x > max) return false;
    if (p.y + t[0] * v.y < min) return false;
    if (p.y + t[0] * v.y > max) return false;
    return true;
  }
};

std::optional<int64_t> Solve(const std::vector<Hail>& hail, double range) {
  // (apx - hp1x)(avy - hv1y) = (apy - hp1y)(avx - hv1x)

  for (long double avx = -range; avx <= range; ++avx) {
    for (long double avy = -range; avy <= range; ++avy) {
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

      long double apx = 
         ((h2.p.y - h1.p.y) * (avx - h2.v.x) * (avx - h1.v.x) -
          h2.p.x * (avy - h2.v.y) * (avx - h1.v.x) + 
          h1.p.x * (avy - h1.v.y) * (avx - h2.v.x)) /
          ((avy - h1.v.y) * (avx - h2.v.x) - (avy - h2.v.y) * (avx - h1.v.x));

      long double apy = h1.p.y + (apx - h1.p.x) * (avy - h1.v.y) / (avx - h1.v.x);

      bool all_match = true;
      for (const Hail& h : hail) {
        double xy_check =
            (apx - h.p.x) * (avy - h.v.y) - (apy - h.p.y) * (avx - h.v.x);
        if (abs(xy_check) > 1e-2) {
          all_match = false;
          break;
        }
      }
      if (!all_match) continue;

      VLOG(1) << "v_xy = " << avx << "," << avy;

      for (long double avz = -range; avz <= range; ++avz) {
        long double apz = h1.p.z + (apx - h1.p.x) * (avz - h1.v.z) / (avx - h1.v.x);
        
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
  int last_range = -1;
  for (int range = 20; range < 100000; range *= 2) {
    std::optional<int64_t> ret = Solve(hail, range);
    if (ret) return ret;
    last_range = range;
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
    for (int j = 0; j < i; ++j) {
      if (hail[i].IntersectXYInRange(hail[j], min, max)) {
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
