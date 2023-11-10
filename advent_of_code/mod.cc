#include "advent_of_code/mod.h"

#include "absl/numeric/int128.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

std::optional<int64_t> ExtendedEuclideanAlgorithm(
    int64_t a, int64_t b, OnExtendedEuclidean on_result) {
  int64_t r_0 = a;
  int64_t r_1 = b;
  if (r_0 < r_1) std::swap(r_0, r_1);
  int64_t s_0 = 1;
  int64_t s_1 = 0;
  int64_t t_0 = 0;
  int64_t t_1 = 1;
  while (r_1 != 0) {
    int64_t q = r_0 / r_1;
    r_0 = r_0 % r_1;
    std::swap(r_0, r_1);
    s_0 = s_0 - q * s_1;
    std::swap(s_0, s_1);
    t_0 = t_0 - q * t_1;
    std::swap(t_0, t_1);
  }
  return on_result(r_0, r_1, s_0, s_1, t_0, t_1);
}

std::optional<int64_t> ExtendedEuclideanAlgorithmInvert(int64_t n,
                                                        int64_t mod) {
  return ExtendedEuclideanAlgorithm(
      n, mod,
      [&](int64_t r_0, int64_t r_1, int64_t s_0, int64_t s_1, int64_t t_0,
          int64_t t_1) -> std::optional<int64_t> {
        if (r_0 != 1) return {};
        return t_0 < 0 ? t_0 + mod : t_0;
      });
}

std::optional<int64_t> ChineseRemainder(int64_t mod_a, int64_t a, int64_t mod_b,
                                        int64_t b) {
  VLOG(2) << "  " << mod_a << " MOD " << a << " AND " << mod_b << " MOD " << b;
  if (b < a) {
    std::swap(a, b);
    std::swap(mod_a, mod_b);
  }
  if (a * b < 0x1'0000'0000) {
    return ExtendedEuclideanAlgorithm(
        b, a,
        [&](int64_t r_0, int64_t r_1, int64_t s_0, int64_t s_1, int64_t t_0,
            int64_t t_1) -> std::optional<int64_t> {
          if (r_0 != 1) return {};
          DCHECK_EQ(s_0 * b + t_0 * a, r_0);
          if (s_0 < 0) s_0 += a;
          if (t_0 < 0) t_0 += b;
          return (mod_a * s_0 * b + mod_b * t_0 * a) % (a * b);
        });
  } else {
    return ExtendedEuclideanAlgorithm(
        b, a,
        [&](int64_t r_0, int64_t r_1, int64_t s_0, int64_t s_1, int64_t t_0,
            int64_t t_1) -> std::optional<int64_t> {
          if (r_0 != 1) return {};
          DCHECK_EQ(s_0 * b + t_0 * a, r_0);
          if (s_0 < 0) s_0 += a;
          if (t_0 < 0) t_0 += b;
          absl::int128 b_c = mod_a;
          b_c *= s_0;
          absl::int128 a_c = mod_b;
          a_c *= t_0;
          absl::int128 ret = (b_c * b + a_c * a) % (a * b);
          CHECK_EQ(Uint128High64(ret), 0) << ret;
          return Uint128Low64(ret);
        });
  }
}

std::optional<int64_t> ChineseRemainder(
    std::vector<std::pair<int64_t, int64_t>> list) {
  if (list.empty()) return {};
  int64_t cur_v = list.begin()->first;
  int64_t cur_mod = list.begin()->second;
  for (auto it = list.begin() + 1; it != list.end(); ++it) {
    auto [next_v, next_mod] = *it;
    std::optional<int64_t> merged =
        ChineseRemainder(cur_v, cur_mod, next_v, next_mod);
    if (!merged) return {};
    cur_v = *merged;
    cur_mod *= next_mod;
  }
  return cur_v;
}

}  // namespace advent_of_code