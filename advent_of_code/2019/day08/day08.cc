#include "advent_of_code/2019/day08/day08.h"

#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/ocr.h"

namespace advent_of_code {

absl::StatusOr<std::string> Day_2019_08::Part1(
    absl::Span<std::string_view> input) const {
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

  std::optional<int> min_zeros;
  int min_layer = -1;
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
  return AdventReturn(num_ones * num_twos);
}

absl::StatusOr<std::string> Day_2019_08::Part2(
    absl::Span<std::string_view> input) const {
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
  CharBoard render(kWidth, kHeight);
  for (int i = 0; i < kLayerSize; ++i) render[{i % kWidth, i / kWidth}] = '?';
  for (int i = 0; i < layer_count; ++i) {
    const char* layer = input[0].data() + i * kLayerSize;
    for (int j = 0; j < kLayerSize; ++j) {
      Point p = {j % kWidth, j / kWidth};
      if (render[p] == '?' && layer[j] != '2') {
        render[p] = layer[j] == '0' ? '.' : '#';
      }
    }
  }

  // TODO(@monkeynova): OCRExtract requires empty columns to break characters,
  // and we have a slight touch between the 'Y' and 'E' in the expected output.
  std::string ret;
  for (int i = 0; i < 25; i += 5) {
    ASSIGN_OR_RETURN(CharBoard single_char,
                     render.SubBoard({{i, 0}, {i + 4, kHeight - 1}}));
    ASSIGN_OR_RETURN(std::string next_char, OCRExtract(single_char));
    ret += next_char;
  }

  return AdventReturn(ret);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2019, /*day=*/8,
    []() { return std::unique_ptr<AdventDay>(new Day_2019_08()); });

}  // namespace advent_of_code
