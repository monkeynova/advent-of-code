// https://adventofcode.com/2015/day/24
//
// --- Day 24: It Hangs in the Balance ---
// ---------------------------------------
//
// It's Christmas Eve, and Santa is loading up the sleigh for this year's
// deliveries. However, there's one small problem: he can't get the
// sleigh to balance. If it isn't balanced, he can't defy physics, and
// nobody gets presents this year.
//
// No pressure.
//
// Santa has provided you a list of the weights of every package he needs
// to fit on the sleigh. The packages need to be split into three groups
// of exactly the same weight, and every package has to fit. The first
// group goes in the passenger compartment of the sleigh, and the second
// and third go in containers on either side. Only when all three groups
// weigh exactly the same amount will the sleigh be able to fly. Defying
// physics has rules, you know!
//
// Of course, that's not the only problem. The first group - the one
// going in the passenger compartment - needs as few packages as possible
// so that Santa has some legroom left over. It doesn't matter how many
// packages are in either of the other two groups, so long as all of the
// groups weigh the same.
//
// Furthermore, Santa tells you, if there are multiple ways to arrange
// the packages such that the fewest possible are in the first group, you
// need to choose the way where the first group has the smallest quantum
// entanglement to reduce the chance of any "complications". The quantum
// entanglement of a group of packages is the product of their weights,
// that is, the value you get when you multiply their weights together.
// Only consider quantum entanglement if the first group has the fewest
// possible number of packages in it and all groups weigh the same
// amount.
//
// For example, suppose you have ten packages with weights 1 through 5
// and 7 through 11. For this situation, some of the unique first groups,
// their quantum entanglements, and a way to divide the remaining
// packages are as follows:
//
// Group 1;             Group 2; Group 3
// 11 9       (QE= 99); 10 8 2;  7 5 4 3 1
// 10 9 1     (QE= 90); 11 7 2;  8 5 4 3
// 10 8 2     (QE=160); 11 9;    7 5 4 3 1
// 10 7 3     (QE=210); 11 9;    8 5 4 2 1
// 10 5 4 1   (QE=200); 11 9;    8 7 3 2
// 10 5 3 2   (QE=300); 11 9;    8 7 4 1
// 10 4 3 2 1 (QE=240); 11 9;    8 7 5
// 9 8 3      (QE=216); 11 7 2;  10 5 4 1
// 9 7 4      (QE=252); 11 8 1;  10 5 3 2
// 9 5 4 2    (QE=360); 11 8 1;  10 7 3
// 8 7 5      (QE=280); 11 9;    10 4 3 2 1
// 8 5 4 3    (QE=480); 11 9;    10 7 2 1
// 7 5 4 3 1  (QE=420); 11 9;    10 8 2
//
// Of these, although 10 9 1 has the smallest quantum entanglement (90),
// the configuration with only two packages, 11 9, in the passenger
// compartment gives Santa the most legroom and wins. In this situation,
// the quantum entanglement for the ideal configuration is therefore 99.
// Had there been two configurations with only two packages in the first
// group, the one with the smaller quantum entanglement would be chosen.
//
// What is the quantum entanglement of the first group of packages in the
// ideal configuration?
//
// --- Part Two ---
// ----------------
//
// That's weird... the sleigh still isn't balancing.
//
// "Ho ho ho", Santa muses to himself. "I forgot the trunk".
//
// Balance the sleigh again, but this time, separate the packages into
// four groups instead of three. The other constraints still apply.
//
// Given the example packages above, this would be some of the new unique
// first groups, their quantum entanglements, and one way to divide the
// remaining packages:
//
// 11 4    (QE=44); 10 5;   9 3 2 1; 8 7
// 10 5    (QE=50); 11 4;   9 3 2 1; 8 7
// 9 5 1   (QE=45); 11 4;   10 3 2;  8 7
// 9 4 2   (QE=72); 11 3 1; 10 5;    8 7
// 9 3 2 1 (QE=54); 11 4;   10 5;    8 7
// 8 7     (QE=56); 11 4;   10 5;    9 3 2 1
//
// Of these, there are three arrangements that put the minimum (two)
// number of packages in the first group: 11 4, 10 5, and 8 7. Of these,
// 11 4 has the lowest quantum entanglement, and so it is selected.
//
// Now, what is the quantum entanglement of the first group of packages
// in the ideal configuration?

#include "advent_of_code/2015/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "re2/re2.h"

namespace advent_of_code {

namespace {

// Helper methods go here.

struct Partition {
  absl::flat_hash_set<int> weights;
  int64_t entanglement = 0;
  bool operator<(const Partition& o) const {
    return entanglement < o.entanglement;
  }
};

struct WeightsSet {
  absl::flat_hash_set<int> weights;
  int max_weight = 0;
};

std::vector<Partition> FindPartitions(const WeightsSet& weights_set, int size,
                                      int target_weight,
                                      absl::flat_hash_set<int> skip_set = {}) {
  VLOG(2) << "FindPartitions(" << size << "," << target_weight << ")";
  if (target_weight < 0) return {};
  if (size == 1) {
    if (skip_set.contains(target_weight)) return {};
    if (!weights_set.weights.contains(target_weight)) return {};
    return {
        Partition{.weights = {target_weight}, .entanglement = target_weight}};
  }
  int best = size * weights_set.max_weight;
  if (best < target_weight) return {};

  std::vector<Partition> ret;
  for (int w : weights_set.weights) {
    if (skip_set.contains(w)) continue;
    skip_set.insert(w);
    std::vector<Partition> list =
        FindPartitions(weights_set, size - 1, target_weight - w, skip_set);
    for (const Partition& p : list) {
      CHECK_EQ(p.weights.size(), size - 1) << absl::StrJoin(p.weights, ",");
      Partition p_new = p;
      p_new.entanglement *= w;
      CHECK_GE(p_new.entanglement, 0) << absl::StrJoin(p.weights, ",");
      p_new.weights.insert(w);
      ret.push_back(p_new);
    }
    skip_set.erase(w);
  }
  return ret;
}

bool CanPartitionRemainder(const WeightsSet& weights_set, const Partition& p,
                           int target_weight) {
  std::vector<int> remaining_weights;
  remaining_weights.reserve(weights_set.weights.size() - p.weights.size());
  for (int w : weights_set.weights) {
    if (!p.weights.contains(w)) remaining_weights.push_back(w);
  }
  VLOG(1) << "Remaining = {" << absl::StrJoin(remaining_weights, ",") << "}";
  for (int i = 0; i < (1 << remaining_weights.size()); ++i) {
    int test_weight = 0;
    std::vector<int> part_1_weights;
    std::vector<int> part_2_weights;
    for (int b = 0; (1 << b) <= i; ++b) {
      if (i & (1 << b)) {
        if (VLOG_IS_ON(1)) part_1_weights.push_back(remaining_weights[b]);
        test_weight += remaining_weights[b];
      } else {
        if (VLOG_IS_ON(1)) part_2_weights.push_back(remaining_weights[b]);
      }
    }
    if (test_weight == target_weight) {
      VLOG(1) << "Found Partitioning: {" << absl::StrJoin(part_1_weights, ",")
              << "} {" << absl::StrJoin(part_2_weights, ",") << "...}";
      return true;
    }
  }
  return false;
}

bool CanPartitionRemainder2(const WeightsSet& weights_set, const Partition& p,
                            int target_weight) {
  std::vector<int> remaining_weights;
  remaining_weights.reserve(weights_set.weights.size() - p.weights.size());
  for (int w : weights_set.weights) {
    if (!p.weights.contains(w)) remaining_weights.push_back(w);
  }
  VLOG(1) << "Remaining = {" << absl::StrJoin(remaining_weights, ",") << "}";
  for (int i = 0; i < (1 << remaining_weights.size()); ++i) {
    Partition sub_p = p;
    int test_weight = 0;
    std::vector<int> part_1_weights;
    std::vector<int> part_2_weights;
    for (int b = 0; (1 << b) <= i; ++b) {
      if (i & (1 << b)) {
        if (VLOG_IS_ON(1)) part_1_weights.push_back(remaining_weights[b]);
        test_weight += remaining_weights[b];
        sub_p.weights.insert(remaining_weights[b]);
      } else {
        if (VLOG_IS_ON(1)) part_2_weights.push_back(remaining_weights[b]);
      }
    }
    if (test_weight == target_weight) {
      VLOG(1) << "Found Partial Partitioning: {"
              << absl::StrJoin(part_1_weights, ",") << "}";
      return CanPartitionRemainder(weights_set, sub_p, target_weight);
    }
  }
  return false;
}

absl::StatusOr<int64_t> FindMinPartition3(const WeightsSet& weights_set) {
  int total_weight = 0;
  for (int w : weights_set.weights) total_weight += w;
  if (total_weight % 3 != 0) {
    return Error("Total weight isn't partitionable: ", total_weight);
  }
  int partition_weight = total_weight / 3;
  VLOG(1) << "Target partition weight = " << partition_weight;

  for (int first_partition_size = 1;
       first_partition_size < weights_set.weights.size() / 3;
       ++first_partition_size) {
    VLOG(1) << "Finding partitions of size " << first_partition_size;
    std::vector<Partition> p_list =
        FindPartitions(weights_set, first_partition_size, partition_weight);
    // Order by entanglement.
    std::sort(p_list.begin(), p_list.end());
    VLOG(1) << " Found " << p_list.size() << " of size "
            << first_partition_size;
    for (const Partition& p : p_list) {
      if (p.weights.size() != first_partition_size) {
        return Error("Bad partition size (expected=", first_partition_size,
                     "): ", absl::StrJoin(p.weights, ","));
      }
      VLOG(1) << "Testing partition: " << absl::StrJoin(p.weights, ",");
      if (CanPartitionRemainder(weights_set, p, partition_weight)) {
        return p.entanglement;
      }
    }
  }

  return -1;
}

absl::StatusOr<int64_t> FindMinPartition4(const WeightsSet& weights_set) {
  int total_weight = 0;
  for (int w : weights_set.weights) total_weight += w;
  if (total_weight % 4 != 0) {
    return Error("Total weight isn't partitionable: ", total_weight);
  }
  int partition_weight = total_weight / 4;
  VLOG(1) << "Target partition weight = " << partition_weight;

  for (int first_partition_size = 1;
       first_partition_size < weights_set.weights.size() / 4;
       ++first_partition_size) {
    VLOG(1) << "Finding partitions of size " << first_partition_size;
    std::vector<Partition> p_list =
        FindPartitions(weights_set, first_partition_size, partition_weight);
    // Order by entanglement.
    std::sort(p_list.begin(), p_list.end());
    VLOG(1) << " Found " << p_list.size() << " of size "
            << first_partition_size;
    for (const Partition& p : p_list) {
      if (p.weights.size() != first_partition_size) {
        return Error("Bad partition size (expected=", first_partition_size,
                     "): ", absl::StrJoin(p.weights, ","));
      }
      VLOG(1) << "Testing partition: " << absl::StrJoin(p.weights, ",");
      if (CanPartitionRemainder2(weights_set, p, partition_weight)) {
        return p.entanglement;
      }
    }
  }

  return -1;
}

}  // namespace

absl::StatusOr<std::string> Day_2015_24::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> weights = ParseAsInts(input);
  WeightsSet weights_set;
  for (int64_t v : *weights) {
    if (weights_set.weights.contains(v)) return Error("Can't handle dupes");
    weights_set.weights.insert(v);
    weights_set.max_weight = std::max<int>(weights_set.max_weight, v);
  }

  return IntReturn(FindMinPartition3(weights_set));
}

absl::StatusOr<std::string> Day_2015_24::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<std::vector<int64_t>> weights = ParseAsInts(input);
  WeightsSet weights_set;
  for (int64_t v : *weights) {
    if (weights_set.weights.contains(v)) return Error("Can't handle dupes");
    weights_set.weights.insert(v);
    weights_set.max_weight = std::max<int>(weights_set.max_weight, v);
  }

  return IntReturn(FindMinPartition4(weights_set));
}

}  // namespace advent_of_code
