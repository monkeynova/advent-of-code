#include "advent_of_code/2019/day12/day12.h"

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
  int norm() const { return abs(x) + abs(y) + abs(z); }
};

std::ostream& operator<<(std::ostream& out, const Vector& v) {
  return out << "{" << v.x << "," << v.y << "," << v.z <<"}";
}

struct Moon {
  Vector position;
  Vector velocity;
  int energy() const { return position.norm() * velocity.norm(); }
};

std::ostream& operator<<(std::ostream& out, const Moon& m) {
  return out << "{" << m.position << "," << m.velocity <<"}";
}

void ApplyGravity(int pos1, int pos2, int* vel1, int* vel2) {
  if (pos1 < pos2) {
    ++*vel1;
    --*vel2;
  } else  if (pos1 > pos2) {
    --*vel1;
    ++*vel2;
  }
}

void ApplyGravity(Moon* m1, Moon* m2) {
  ApplyGravity(m1->position.x, m2->position.x, &m1->velocity.x, &m2->velocity.x);
  ApplyGravity(m1->position.y, m2->position.y, &m1->velocity.y, &m2->velocity.y);
  ApplyGravity(m1->position.z, m2->position.z, &m1->velocity.z, &m2->velocity.z);
}

absl::StatusOr<std::vector<std::string>> Day12_2019::Part1(
    const std::vector<absl::string_view>& input) const {
  RE2 pattern{"<x=(-?\\d+), y=(-?\\d+), z=(-?\\d+)>"};
  std::vector<Moon> moons;
  for (absl::string_view str : input) {
    Moon moon;
    if (!RE2::FullMatch(str, pattern, &moon.position.x, &moon.position.y, &moon.position.z)) {
      return absl::InvalidArgumentError("Could not parse moon");
    }
    moons.push_back(moon);
  }  
  for (int i = 0; i < 1000; ++i) {
    LOG(WARNING) << "Step: " << i;
    for (auto& moon : moons) {
      LOG(WARNING) << moon;
    }
    for (int i = 0; i < moons.size(); ++i) {
      for (int j = 0; j < i; ++j) {
        ApplyGravity(&moons[i], &moons[j]);
      }
    }
    for (auto& moon : moons) {
      moon.position += moon.velocity;
    }
  }
  int energy = 0;
  for (auto& moon : moons) {
    energy += moon.energy();
  }

  return std::vector<std::string>{absl::StrCat(energy)};
}

absl::StatusOr<std::vector<std::string>> Day12_2019::Part2(
    const std::vector<absl::string_view>& input) const {
  return std::vector<std::string>{""};
}
