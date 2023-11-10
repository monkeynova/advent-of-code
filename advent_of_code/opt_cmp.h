#ifndef ADVENT_OF_CODE_OPT_CMP_H
#define ADVENT_OF_CODE_OPT_CMP_H

namespace advent_of_code {

template <typename T>
std::optional<T> opt_min(std::optional<T> a, absl::optional<T> b) {
  if (!a) return b;
  if (!b) return a;
  return std::min(*a, *b);
}

template <typename T>
std::optional<int> opt_min(std::optional<T> a, T b) {
  if (!a) return b;
  return std::min(*a, b);
}

template <typename T>
std::optional<T> opt_max(std::optional<T> a, absl::optional<T> b) {
  if (!a) return b;
  if (!b) return a;
  return std::max(*a, *b);
}

template <typename T>
std::optional<T> opt_max(std::optional<T> a, T b) {
  if (!a) return b;
  return std::max(*a, b);
}

template <typename T>
std::optional<T> opt_add(T d, std::optional<T> a) {
  if (!a) return a;
  return *a + d;
}

template <typename T>
std::optional<T> opt_add(std::optional<T> a, T d) {
  return opt_add(d, a);
}

template <typename K, typename Container>
auto opt_find(const Container& c, const K& k)
    -> std::optional<std::remove_reference_t<decltype(c.find(k)->second)>> {
  auto it = c.find(k);
  if (it == c.end()) return absl::nullopt;
  return it->second;
}

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_OPT_CMP_H
