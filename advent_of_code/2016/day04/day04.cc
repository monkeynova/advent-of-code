// https://adventofcode.com/2016/day/4
//
// --- Day 4: Security Through Obscurity ---
// -----------------------------------------
// 
// Finally, you come across an information kiosk with a list of rooms. Of
// course, the list is encrypted and full of decoy data, but the
// instructions to decode the list are barely hidden nearby. Better
// remove the decoy data first.
// 
// Each room consists of an encrypted name (lowercase letters separated
// by dashes) followed by a dash, a sector ID, and a checksum in square
// brackets.
// 
// A room is real (not a decoy) if the checksum is the five most common
// letters in the encrypted name, in order, with ties broken by
// alphabetization. For example:
// 
// * aaaaa-bbb-z-y-x-123[abxyz] is a real room because the most common
// letters are a (5), b (3), and then a tie between x, y, and z,
// which are listed alphabetically.
// 
// * a-b-c-d-e-f-g-h-987[abcde] is a real room because although the
// letters are all tied (1 of each), the first five are listed
// alphabetically.
// 
// * not-a-real-room-404[oarel] is a real room.
// 
// * totally-real-room-200[decoy] is not.
// 
// Of the real rooms from the list above, the sum of their sector IDs is
// 1514.
// 
// What is the sum of the sector IDs of the real rooms?
//
// --- Part Two ---
// ----------------
// 
// With all the decoy data out of the way, it's time to decrypt this list
// and get moving.
// 
// The room names are encrypted by a state-of-the-art shift cipher, which
// is nearly unbreakable without the right software. However, the
// information kiosk designers at Easter Bunny HQ were not expecting to
// deal with a master cryptographer like yourself.
// 
// To decrypt a room name, rotate each letter forward through the
// alphabet a number of times equal to the room's sector ID. A becomes B,
// B becomes C, Z becomes A, and so on. Dashes become spaces.
// 
// For example, the real name for qzmt-zixmtkozy-ivhz-343 is very
// encrypted name.
// 
// What is the sector ID of the room where North Pole objects are stored?


#include "advent_of_code/2016/day04/day04.h"

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

bool Validate(absl::string_view room, absl::string_view sum) {
  absl::flat_hash_map<char, int> counts;
  for (char c : room) {
    if (c != '-') {
      ++counts[c];
    }
  }
  struct Sortable {
    char c;
    int count;
    bool operator<(const Sortable& o) const {
      if (count != o.count) return count > o.count;
      return c < o.c;
    }
  };
  std::vector<Sortable> sorted;
  for (const auto& [c, count] : counts) {
    sorted.push_back({.c = c, .count = count});
  }
  std::sort(sorted.begin(), sorted.end());
  std::string assert;
  assert.resize(5);
  for (int i = 0; i < 5; ++i) {
    assert[i] = sorted[i].c;
  }
  VLOG(2) << sum << " =?= " << assert;
  return sum == assert;
  ;
}

std::string Decrypt(absl::string_view name, int sector) {
  std::string ret = std::string(name);
  for (int i = 0; i < ret.size(); ++i) {
    if (ret[i] == '-') {
      ret[i] = ' ';
    } else {
      ret[i] = (((ret[i] - 'a') + sector) % 26) + 'a';
    }
  }
  return ret;
}

// Helper methods go here.

}  // namespace

absl::StatusOr<std::string> Day_2016_04::Part1(
    absl::Span<absl::string_view> input) const {
  int sector_sum = 0;
  for (absl::string_view in : input) {
    absl::string_view room;
    int sector;
    absl::string_view sum;
    if (!RE2::FullMatch(in, "([a-z\\-]+)-(\\d+)\\[(.....)\\]", &room, &sector,
                        &sum)) {
      return Error("Bad input: ", in);
    }
    if (Validate(room, sum)) {
      sector_sum += sector;
    }
  }
  return IntReturn(sector_sum);
}

absl::StatusOr<std::string> Day_2016_04::Part2(
    absl::Span<absl::string_view> input) const {
  std::string ret;
  for (absl::string_view in : input) {
    absl::string_view room;
    int sector;
    absl::string_view sum;
    if (!RE2::FullMatch(in, "([a-z\\-]+)-(\\d+)\\[(.....)\\]", &room, &sector,
                        &sum)) {
      return Error("Bad input: ", in);
    }
    if (!Validate(room, sum)) continue;
    std::string decryped_room = Decrypt(room, sector);
    if (input.size() < 10 || RE2::PartialMatch(decryped_room, "northpole")) {
      if (!ret.empty()) return Error("Mutiple matching rooms");
      ret = absl::StrCat(sector, ": ", decryped_room);
    }
  }
  return ret;
}

}  // namespace advent_of_code
