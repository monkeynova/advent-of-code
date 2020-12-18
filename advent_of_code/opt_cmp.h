#ifndef ADVENT_OF_CODE_2019_OPT_CMP_H
#define ADVENT_OF_CODE_2019_OPT_CMP_H

namespace advent_of_code {

template <typename T>
absl::optional<T> opt_min(absl::optional<T> a, absl::optional<T> b) {
  if (!a) return b;
  if (!b) return a;
  return std::min(*a, *b);
}

template <typename T>
absl::optional<int> opt_min(absl::optional<T> a, T b) {
  if (!a) return b;
  return std::min(*a, b);
}

template <typename T>
absl::optional<T> opt_max(absl::optional<T> a, absl::optional<T> b) {
  if (!a) return b;
  if (!b) return a;
  return std::max(*a, *b);
}

template <typename T>
absl::optional<T> opt_max(absl::optional<T> a, T b) {
  if (!a) return b;
  return std::max(*a, b);
}

template <typename T>
absl::optional<T> opt_add(T d, absl::optional<T> a) {
  if (!a) return a;
  return *a + d;
}

template <typename T>
absl::optional<T> opt_add(absl::optional<T> a, T d) {
  return opt_add(d, a);
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_2019_OPT_CMP_H
