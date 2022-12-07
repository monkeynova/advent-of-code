// http://adventofcode.com/2022/day/07

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
    absl::Span<absl::string_view> input) {
  std::string cur_dir = "/";
  absl::flat_hash_map<std::string, int> file2size;
  bool listing = false;
  for (absl::string_view line : input) {
    absl::string_view dname;
    absl::string_view fname;
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
        cur_dir = "/";
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
      // ignore
    } else if (RE2::FullMatch(line, "(\\d+) (.*)", &size, &fname)) {
      if (!listing) return Error("Not listing");
      std::string file = cur_dir;
      file.append("/");
      file.append(fname);
      file2size[file] = size;
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
    for (size_t idx = dir.rfind("/"); idx != std::string::npos; idx = dir.rfind("/")) {
      dir = dir.substr(0, idx);
      dir2size[dir] += size;
    }
  }
  return dir2size;
}

absl::StatusOr<absl::flat_hash_map<std::string, int>> ParseDirSizes(
    absl::Span<absl::string_view> input) {
  absl::StatusOr<absl::flat_hash_map<std::string, int>> file2size =
      ParseFileSizes(input);
  if (!file2size.ok()) return file2size.status();
  return FileSizes2DirSizes(*file2size);
}

}  // namespace

absl::StatusOr<std::string> Day_2022_07::Part1(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, int>> dir2size =
      ParseDirSizes(input);
  if (!dir2size.ok()) return dir2size.status();
  
  int small_size = 0;
  for (const auto& [dname, size] : *dir2size) {
    if (size < 100000) small_size += size; 
  }
  return IntReturn(small_size);
}

absl::StatusOr<std::string> Day_2022_07::Part2(
    absl::Span<absl::string_view> input) const {
  absl::StatusOr<absl::flat_hash_map<std::string, int>> dir2size =
      ParseDirSizes(input);
  if (!dir2size.ok()) return dir2size.status();

  int total = (*dir2size)["/"];
  int min_free = total - 40000000;
  int best = std::numeric_limits<int>::max();
  for (const auto& [dname, size] : *dir2size) {
    if (size > min_free && size < best) best = size;
  }
  return IntReturn(best);
}

}  // namespace advent_of_code
