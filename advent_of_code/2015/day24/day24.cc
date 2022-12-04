// http://adventofcode.com/2015/day/24

#include "advent_of_code/2015/day24/day24.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "absl/log/log.h"
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
