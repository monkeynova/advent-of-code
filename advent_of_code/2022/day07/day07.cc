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

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2022_07::Part1(
    absl::Span<absl::string_view> input) const {
  std::string cur_dir = "/";
  absl::flat_hash_map<std::string, int> file2size;
  bool listing = false;
  for (absl::string_view line : input) {
    absl::string_view dname;
    absl::string_view fname;
    int size;
    if (line == "$ ls") {
      listing = true;
    } else if (RE2::FullMatch(line, "\\$ cd (.*)", &dname)) {
      listing = false;
      if (dname == "..") {
        size_t idx = cur_dir.rfind("/");
        if (idx == std::string::npos) return Error("Can't cd ..");
        cur_dir = cur_dir.substr(0, idx);
      } else if (dname == "/") {
        cur_dir = "/";
      } else {
        cur_dir.append("/");
        cur_dir.append(dname);
      }
    } else if (RE2::FullMatch(line, "dir (.*)", &dname)) {
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
  absl::flat_hash_map<std::string, int> dir2size;
  for (const auto& [fname, size] : file2size) {
    std::string_view dir = fname;
    for (size_t idx = dir.rfind("/"); idx != std::string::npos; idx = dir.rfind("/")) {
      dir = dir.substr(0, idx);
      dir2size[dir] += size;
    }
  }
  int small_size = 0;
  for (const auto& [dname, size] : dir2size) {
    if (size < 100000) small_size += size; 
  }
  return IntReturn(small_size);
}

absl::StatusOr<std::string> Day_2022_07::Part2(
    absl::Span<absl::string_view> input) const {
  std::string cur_dir = "/";
  absl::flat_hash_map<std::string, int> file2size;
  bool listing = false;
  for (absl::string_view line : input) {
    absl::string_view dname;
    absl::string_view fname;
    int size;
    if (line == "$ ls") {
      listing = true;
    } else if (RE2::FullMatch(line, "\\$ cd (.*)", &dname)) {
      listing = false;
      if (dname == "..") {
        size_t idx = cur_dir.rfind("/");
        if (idx == std::string::npos) return Error("Can't cd ..");
        cur_dir = cur_dir.substr(0, idx);
      } else if (dname == "/") {
        cur_dir = "/";
      } else {
        cur_dir.append("/");
        cur_dir.append(dname);
      }
    } else if (RE2::FullMatch(line, "dir (.*)", &dname)) {
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
  absl::flat_hash_map<std::string, int> dir2size;
  for (const auto& [fname, size] : file2size) {
    std::string_view dir = fname;
    for (size_t idx = dir.rfind("/"); idx != std::string::npos; idx = dir.rfind("/")) {
      dir = dir.substr(0, idx);
      dir2size[dir] += size;
    }
  }
  int total = dir2size["/"];
  int min_free = 30000000 - (70000000 - total);
  int best = std::numeric_limits<int>::max();
  for (const auto& [dname, size] : dir2size) {
    if (size > min_free && size < best) best = size;
  }
  return IntReturn(best);
}

}  // namespace advent_of_code
