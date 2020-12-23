#include "advent_of_code/2016/day17/day17.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/point.h"
#include "glog/logging.h"
#include "re2/re2.h"
#include "third_party/md5/md5.h"

namespace advent_of_code {

namespace {

std::string Hex(absl::string_view buf) {
  static char hexchar[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  return absl::StrJoin(buf, "", [](std::string* out, char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    char tmp[] = {hexchar[uc >> 4], hexchar[uc & 0xf], '\0'};
    absl::StrAppend(out, tmp);
  });
}

std::string MD5Hex(std::string str) {
  MD5_CTX ctx;
  unsigned char md5_result[16];
  absl::string_view md5_result_view(reinterpret_cast<char*>(md5_result), 16);
  MD5_Init(&ctx);
  MD5_Update(&ctx, str.data(), str.size());
  MD5_Final(md5_result, &ctx);
  return Hex(md5_result_view);
}

absl::StatusOr<std::string> FindPath(absl::string_view input) {
  struct Path {
    std::string dir;
    Point p;
  };
  std::deque<Path> frontier = {Path{.dir = "", .p = {0, 0}}};
  constexpr Point kKeyDirs[] = {Cardinal::kNorth, Cardinal::kSouth,
                                Cardinal::kWest, Cardinal::kEast};
  constexpr absl::string_view kDirNames = "UDLR";
  while (!frontier.empty()) {
    Path cur = frontier.front();
    frontier.pop_front();
    std::string md5 = MD5Hex(absl::StrCat(input, cur.dir));
    for (int i = 0; i < 4; ++i) {
      if (md5[i] >= 'b') {
        Path next = cur;
        next.p += kKeyDirs[i];
        next.dir.append(kDirNames.substr(i, 1));
        if (next.p == Point{3, 3}) return next.dir;
        if (next.p.x >= 0 && next.p.y >= 0 && next.p.x <= 3 && next.p.y <= 3) {
          frontier.push_back(next);
        }
      }
    }
  }

  return AdventDay::Error("No path found");
}

absl::optional<int> FindLongestPath(absl::string_view input) {
  struct Path {
    std::string dir;
    Point p;
  };
  std::deque<Path> frontier = {Path{.dir = "", .p = {0, 0}}};
  constexpr Point kKeyDirs[] = {Cardinal::kNorth, Cardinal::kSouth,
                                Cardinal::kWest, Cardinal::kEast};
  constexpr absl::string_view kDirNames = "UDLR";
  absl::optional<int> longest_path;
  while (!frontier.empty()) {
    Path cur = frontier.front();
    frontier.pop_front();
    std::string md5 = MD5Hex(absl::StrCat(input, cur.dir));
    for (int i = 0; i < 4; ++i) {
      if (md5[i] >= 'b') {
        Path next = cur;
        next.p += kKeyDirs[i];
        next.dir.append(kDirNames.substr(i, 1));
        if (next.p == Point{3, 3}) {
          longest_path = next.dir.size();
          continue;
        }
        if (next.p.x >= 0 && next.p.y >= 0 && next.p.x <= 3 && next.p.y <= 3) {
          frontier.push_back(next);
        }
      }
    }
  }

  return longest_path;
}

}  // namespace

absl::StatusOr<std::vector<std::string>> Day17_2016::Part1(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Input size");
  absl::StatusOr<std::string> path = FindPath(input[0]);
  if (!path.ok()) return path.status();
  return std::vector<std::string>{*path};
}

absl::StatusOr<std::vector<std::string>> Day17_2016::Part2(
    absl::Span<absl::string_view> input) const {
  if (input.size() != 1) return Error("Input size");
  return IntReturn(FindLongestPath(input[0]));
}

}  // namespace advent_of_code
