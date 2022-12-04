// http://adventofcode.com/2019/day/12

#include "advent_of_code/2019/day12/day12.h"

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "absl/log/log.h"
#include "re2/re2.h"

namespace advent_of_code {
namespace {

struct Moon {
  Point3 position = Cardinal3::kOrigin;
  Point3 velocity = Cardinal3::kOrigin;
  bool operator==(const Moon& other) const {
    return position == other.position && velocity == other.velocity;
  }
  bool operator!=(const Moon& other) const { return !operator==(other); }
  int energy() const { return position.dist() * velocity.dist(); }

  template <typename H>
  friend H AbslHashValue(H h, const Moon& m) {
    return H::combine(std::move(h), m.position, m.velocity);
  }

  friend std::ostream& operator<<(std::ostream& out, const Moon& m) {
    return out << "{" << m.position << "," << m.velocity << "}";
  }
};

void ApplyGravity(Moon* m1, Moon* m2) {
  Point3 gravity = m2->position - m1->position;
  gravity.x = gravity.x > 0 ? 1 : gravity.x < 0 ? -1 : 0;
  gravity.y = gravity.y > 0 ? 1 : gravity.y < 0 ? -1 : 0;
  gravity.z = gravity.z > 0 ? 1 : gravity.z < 0 ? -1 : 0;
  m1->velocity += gravity;
  m2->velocity -= gravity;
  return;
}

int TotalEnergy(const std::vector<Moon>& moons) {
  int energy = 0;
  for (auto& moon : moons) {
    energy += moon.energy();
  }
  return energy;
}

}  // namespace

absl::StatusOr<std::string> Day_2019_12::Part1(
    absl::Span<absl::string_view> input) const {
  RE2 pattern{"<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>"};
  std::vector<Moon> moons;
  for (absl::string_view str : input) {
    Moon moon;
    if (!RE2::FullMatch(str, pattern, &moon.position.x, &moon.position.y,
                        &moon.position.z)) {
      return absl::InvalidArgumentError("Could not parse moon");
    }
    moons.push_back(moon);
  }
  for (int i = 0; i < 1000; ++i) {
    for (int i = 0; i < moons.size(); ++i) {
      for (int j = 0; j < i; ++j) {
        ApplyGravity(&moons[i], &moons[j]);
      }
    }
    for (auto& moon : moons) {
      moon.position += moon.velocity;
    }
  }

  return IntReturn(TotalEnergy(moons));
}

absl::StatusOr<std::string> Day_2019_12::Part2(
    absl::Span<absl::string_view> input) const {
  RE2 pattern{"<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>"};
  std::vector<Moon> moons;
  for (absl::string_view str : input) {
    Moon moon;
    if (!RE2::FullMatch(str, pattern, &moon.position.x, &moon.position.y,
                        &moon.position.z)) {
      return absl::InvalidArgumentError("Could not parse moon");
    }
    moons.push_back(moon);
  }
  // Simulate on dimension at a time, look for loops then multiply for when they
  // align.
  std::vector<Moon> start = moons;
  int64_t steps = 0;
  absl::optional<int> x_cycle_size;
  absl::optional<int> y_cycle_size;
  absl::optional<int> z_cycle_size;

  bool done = false;
  while (!done) {
    ++steps;
    if (steps % 77777777 == 0) {
      LOG(WARNING) << steps;
    }
    for (int i = 0; i < moons.size(); ++i) {
      for (int j = 0; j < i; ++j) {
        ApplyGravity(&moons[i], &moons[j]);
      }
    }
    for (auto& moon : moons) {
      moon.position += moon.velocity;
    }
    bool x_same = true;
    bool y_same = true;
    bool z_same = true;
    for (int i = 0; i < start.size(); ++i) {
      if (start[i].position.x != moons[i].position.x ||
          start[i].velocity.x != moons[i].velocity.x) {
        x_same = false;
      }
      if (start[i].position.y != moons[i].position.y ||
          start[i].velocity.y != moons[i].velocity.y) {
        y_same = false;
      }
      if (start[i].position.z != moons[i].position.z ||
          start[i].velocity.z != moons[i].velocity.z) {
        z_same = false;
      }
    }
    if (!x_cycle_size && x_same) {
      x_cycle_size = steps;
      if (y_cycle_size && z_cycle_size) {
        done = true;
      }
    }
    if (!y_cycle_size && y_same) {
      y_cycle_size = steps;
      if (x_cycle_size && z_cycle_size) {
        done = true;
      }
    }
    if (!z_cycle_size && z_same) {
      z_cycle_size = steps;
      if (x_cycle_size && y_cycle_size) {
        done = true;
      }
    }
  }

  int64_t product = *x_cycle_size;
  product = product * *y_cycle_size / std::gcd(product, *y_cycle_size);
  product = product * *z_cycle_size / std::gcd(product, *z_cycle_size);

  return IntReturn(product);
}

}  // namespace advent_of_code
