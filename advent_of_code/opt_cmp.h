#ifndef ADVENT_OF_CODE_2019_OPT_CMP_H
#define ADVENT_OF_CODE_2019_OPT_CMP_H

absl::optional<int> opt_min(absl::optional<int> a, absl::optional<int> b) {
  if (!a) return b;
  if (!b) return a;
  return std::min(*a, *b);
}

absl::optional<int> opt_min(absl::optional<int> a, int b) {
  if (!a) return b;
  return std::min(*a, b);
}

absl::optional<int> opt_max(absl::optional<int> a, absl::optional<int> b) {
  if (!a) return b;
  if (!b) return a;
  return std::max(*a, *b);
}

absl::optional<int> opt_max(absl::optional<int> a, int b) {
  if (!a) return b;
  return std::max(*a, b);
}

absl::optional<int> opt_add(int d, absl::optional<int> a) {
  if (!a) return a;
  return *a + d;
}

absl::optional<int> opt_add(absl::optional<int> a, int d) {
  return opt_add(d, a);
}

#endif  // ADVENT_OF_CODE_2019_OPT_CMP_H
