#include "advent_of_code/2022/day07/day07.h"

#include "absl/algorithm/container.h"
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

absl::StatusOr<absl::flat_hash_map<std::string, int>> ParseFileSizes(
    absl::Span<std::string_view> input) {
  std::string cur_dir = "";
  absl::flat_hash_map<std::string, int> file2size;
  bool listing = false;
  for (std::string_view line : input) {
    std::string_view dname;
    std::string_view fname;
    int size;
    if (line == "$ ls") {
      listing = true;
    } else if (absl::StartsWith(line, "$ cd ")) {
      dname = line.substr(5);
      listing = false;
      if (dname == "..") {
        size_t idx = cur_dir.rfind("/");
        if (idx == std::string::npos) return Error("Can't cd ..");
        cur_dir = cur_dir.substr(0, idx);
      } else if (dname == "/") {
        cur_dir = "";
      } else {
        if (dname.find(".") != std::string::npos) {
          return Error("Unhandled subdir", dname);
        }
        if (dname.find("/") != std::string::npos) {
          return Error("Unhandled subdir: ", dname);
        }
        cur_dir.append("/");
        cur_dir.append(dname);
      }
    } else if (absl::StartsWith(line, "dir ")) {
      if (!listing) return Error("Not listing");
      // Ignore directory listings.
    } else if (RE2::FullMatch(line, "(\\d+) (.*)", &size, &fname)) {
      if (!listing) return Error("Not listing");
      std::string file = cur_dir;
      file.append("/");
      file.append(fname);
      if (!file2size.emplace(file, size).second) {
        return Error("Duplicate file: ", file);
      }
    } else {
      return Error("Bad line: ", line);
    }
  }
  return file2size;
}

absl::flat_hash_map<std::string, int> FileSizes2DirSizes(
    const absl::flat_hash_map<std::string, int>& file2size) {
  absl::flat_hash_map<std::string, int> dir2size;
  for (const auto& [fname, size] : file2size) {
    std::string_view dir = fname;
    for (size_t idx = dir.rfind("/"); idx != std::string::npos;
         idx = dir.rfind("/")) {
      dir = dir.substr(0, idx);
      dir2size[dir] += size;
    }
  }
  return dir2size;
}

absl::StatusOr<absl::flat_hash_map<std::string, int>> ParseDirSizes(
    absl::Span<std::string_view> input) {
  absl::flat_hash_map<std::string, int> file2size;
  ASSIGN_OR_RETURN(file2size, ParseFileSizes(input));
  return FileSizes2DirSizes(file2size);
}

}  // namespace

absl::StatusOr<std::string> Day_2022_07::Part1(
    absl::Span<std::string_view> input) const {
  const int kSmallSizeThreshold = 100'000;

  absl::flat_hash_map<std::string, int> dir2size;
  ASSIGN_OR_RETURN(dir2size, ParseDirSizes(input));

  int small_size = 0;
  for (const auto& [dname, size] : dir2size) {
    if (size > kSmallSizeThreshold) continue;
    small_size += size;
  }
  return AdventReturn(small_size);
}

absl::StatusOr<std::string> Day_2022_07::Part2(
    absl::Span<std::string_view> input) const {
  const int kDiskSize = 70'000'000;
  const int kFreeSize = 30'000'000;

  absl::flat_hash_map<std::string, int> dir2size;
  ASSIGN_OR_RETURN(dir2size, ParseDirSizes(input));

  int total = dir2size[""];
  int min_free = total - (kDiskSize - kFreeSize);
  int best = std::numeric_limits<int>::max();
  for (const auto& [dname, size] : dir2size) {
    if (size < min_free) continue;
    best = std::min(size, best);
  }
  return AdventReturn(best);
}

static AdventRegisterEntry registry = RegisterAdventDay(
    /*year=*/2022, /*day=*/7, []() {
  return std::unique_ptr<AdventDay>(new Day_2022_07());
});

}  // namespace advent_of_code
