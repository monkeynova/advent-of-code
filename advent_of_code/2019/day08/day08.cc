#include "advent_of_code/2019/day08/day08.h"

#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "glog/logging.h"

namespace advent_of_code {
namespace {}  // namespace

absl::StatusOr<std::vector<std::string>> Day_2019_08::Part1(
    absl::Span<absl::string_view> input) const {
  const int kWidth = 25;
  const int kHeight = 6;
  const int kLayerSize = kWidth * kHeight;
  if (input.size() != 1) {
    return absl::InvalidArgumentError("Not exactly one image");
  }
  if (input[0].size() % kLayerSize != 0) {
    return absl::InvalidArgumentError("Image isn't 25x6");
  }
  const int layer_count = input[0].size() / kLayerSize;
  if (layer_count <= 0) {
    return absl::InvalidArgumentError("Image is empty");
  }

  absl::optional<int> min_zeros;
  int min_layer;
  for (int i = 0; i < layer_count; ++i) {
    const char* layer = input[0].data() + i * kLayerSize;
    int zero_count = 0;
    for (int j = 0; j < kLayerSize; ++j) {
      if (layer[j] == '0') ++zero_count;
    }
    if (!min_zeros || *min_zeros > zero_count) {
      min_zeros = zero_count;
      min_layer = i;
    }
  }
  const char* layer = input[0].data() + min_layer * kLayerSize;
  int num_ones = 0;
  int num_twos = 0;
  for (int j = 0; j < kLayerSize; ++j) {
    if (layer[j] == '1') ++num_ones;
    if (layer[j] == '2') ++num_twos;
  }
  return IntReturn(num_ones * num_twos);
}

absl::StatusOr<std::vector<std::string>> Day_2019_08::Part2(
    absl::Span<absl::string_view> input) const {
  const int kWidth = 25;
  const int kHeight = 6;
  const int kLayerSize = kWidth * kHeight;
  if (input.size() != 1) {
    return absl::InvalidArgumentError("Not exactly one image");
  }
  if (input[0].size() % kLayerSize != 0) {
    return absl::InvalidArgumentError("Image isn't 25x6");
  }
  const int layer_count = input[0].size() / kLayerSize;
  if (layer_count <= 0) {
    return absl::InvalidArgumentError("Image is empty");
  }
  std::string render;
  render.resize(kLayerSize);
  for (int i = 0; i < kLayerSize; ++i) render[i] = '?';
  for (int i = 0; i < layer_count; ++i) {
    const char* layer = input[0].data() + i * kLayerSize;
    for (int j = 0; j < kLayerSize; ++j) {
      if (render[j] == '?' && layer[j] != '2') {
        render[j] = layer[j] == '0' ? ' ' : 'X';
      }
    }
  }
  std::vector<std::string> ret;
  for (int i = 0; i < kHeight; ++i) {
    ret.push_back(std::string(render.substr(i * kWidth, kWidth)));
  }

  return ret;
}

}  // namespace advent_of_code
