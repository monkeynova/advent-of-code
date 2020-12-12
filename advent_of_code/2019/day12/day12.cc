#include "advent_of_code/2019/day12/day12.h"

#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"
#include "re2/re2.h"

struct Vector {
  int x = 0;
  int y = 0;
  int z = 0;
  Vector& operator+=(const Vector& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }
  bool operator==(const Vector& other) const {
    return x == other.x && y == other.y && z == other.z;
  }
  bool operator!=(const Vector& other) const { return !operator==(other); }
  int norm() const { return abs(x) + abs(y) + abs(z); }
};

template <typename H>
H AbslHashValue(H h, const Vector& v) {
  return H::combine(std::move(h), v.x, v.y, v.z);
}

std::ostream& operator<<(std::ostream& out, const Vector& v) {
  return out << "{" << v.x << "," << v.y << "," << v.z << "}";
}

struct Moon {
  Vector position;
  Vector velocity;
  bool operator==(const Moon& other) const {
    return position == other.position && velocity == other.velocity;
  }
  bool operator!=(const Moon& other) const { return !operator==(other); }
  int energy() const { return position.norm() * velocity.norm(); }
};

template <typename H>
H AbslHashValue(H h, const Moon& m) {
  return H::combine(std::move(h), m.position, m.velocity);
}

std::ostream& operator<<(std::ostream& out, const Moon& m) {
  return out << "{" << m.position << "," << m.velocity << "}";
}

void ApplyGravity(int pos1, int pos2, int* vel1, int* vel2) {
  if (pos1 < pos2) {
    ++*vel1;
    --*vel2;
  } else if (pos1 > pos2) {
    --*vel1;
    ++*vel2;
  }
}

void ApplyGravity(Moon* m1, Moon* m2) {
  ApplyGravity(m1->position.x, m2->position.x, &m1->velocity.x,
               &m2->velocity.x);
  ApplyGravity(m1->position.y, m2->position.y, &m1->velocity.y,
               &m2->velocity.y);
  ApplyGravity(m1->position.z, m2->position.z, &m1->velocity.z,
               &m2->velocity.z);
}

int TotalEnergy(const std::vector<Moon>& moons) {
  int energy = 0;
  for (auto& moon : moons) {
    energy += moon.energy();
  }
  return energy;
}

absl::StatusOr<std::vector<std::string>> Day12_2019::Part1(
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

absl::StatusOr<std::vector<std::string>> Day12_2019::Part2(
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
