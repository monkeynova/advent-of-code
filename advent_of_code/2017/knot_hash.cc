#include "advent_of_code/2017/knot_hash.h"

#include "absl/log/log.h"
#include "absl/strings/str_join.h"
#include "absl/types/span.h"
#include "advent_of_code/vlog.h"

namespace advent_of_code {

void KnotHash::RunLoop(std::string_view lengths,
                       std::vector<unsigned char>* loop, int round_count) {
  int position = 0;
  int skip_size = 0;
  const int loop_size = loop->size();
  absl::Span<unsigned char> loop_span = absl::MakeSpan(*loop);
  for (int round = 0; round < round_count; ++round) {
    for (int i = 0; i < lengths.size(); ++i) {
      unsigned char len = lengths[i];
      VLOG(1) << "len=" << len
              << "; loop=" << absl::StrJoin(loop_span.subspan(0, position), ",")
              << (position > 0 ? ",(" : "(") << loop->at(position)
              << (position < loop_span.size() - 1 ? ")," : ")")
              << absl::StrJoin(loop_span.subspan(position + 1), ",");
      if (len > 0) {
        int first = position;
        int last = (position + len - 1) % loop_size;
        while (first != last) {
          VLOG(2) << "first=" << first << "; last=" << last;
          std::swap((*loop)[first], (*loop)[last]);
          first = (first + 1) % loop_size;
          // Check needed here for even sized loops.
          if (first == last) break;
          last = (loop_size + last - 1) % loop_size;
        }
      }

      position = (position + len + skip_size) % loop_size;
      ++skip_size;
    }
  }
  VLOG(1) << "len=<DONE>; loop="
          << absl::StrJoin(loop_span.subspan(0, position), ",")
          << (position > 0 ? ",(" : "(") << loop->at(position)
          << (position < loop_span.size() - 1 ? ")," : ")")
          << absl::StrJoin(loop_span.subspan(position + 1), ",");
}

std::string KnotHash::DigestHex(std::string_view input) {
  std::string lengths = std::string(input);
  const char kSuffix[] = {17, 31, 73, 47, 23, '\0'};
  lengths.append(kSuffix);

  std::vector<unsigned char> loop(256);
  for (int i = 0; i < loop.size(); ++i) loop[i] = i;
  RunLoop(lengths, &loop, 64);

  std::string dense_hash;
  std::string_view kHexStr = "0123456789abcdef";
  for (int i = 0; i < 256; i += 16) {
    int next = 0;
    for (int j = 0; j < 16; ++j) {
      next ^= loop[i + j];
    }
    dense_hash.append(kHexStr.substr(next >> 4, 1));
    dense_hash.append(kHexStr.substr(next & 0xf, 1));
  }

  return dense_hash;
}

}  // namespace advent_of_code
