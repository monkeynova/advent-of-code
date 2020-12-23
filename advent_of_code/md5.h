#ifndef ADVENT_OF_CODE_MD5_H
#define ADVENT_OF_CODE_MD5_H

#include "third_party/md5/md5.h"

namespace advent_of_code {

class MD5 {
 public:
  MD5()
      : md5_result_view_(reinterpret_cast<char*>(md5_result_), 16),
        md5_result_hex_view_(reinterpret_cast<char*>(md5_result_hex_), 32) {}

  absl::string_view Digest(absl::string_view in) {
    MD5_Init(&ctx);
    MD5_Update(&ctx, in.data(), in.size());
    MD5_Final(md5_result_, &ctx);
    return md5_result_view_;
  }

  absl::string_view DigestHex(absl::string_view in) {
    static constexpr char hexchar[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                       '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    Digest(in);
    for (int i = 0; i < 16; ++i) {
      md5_result_hex_[2 * i] = hexchar[md5_result_[i] >> 4];
      md5_result_hex_[2 * i + 1] = hexchar[md5_result_[i] & 0xf];
    }
    return md5_result_hex_view_;
  }

 private:
  unsigned char md5_result_[16];
  absl::string_view md5_result_view_;
  unsigned char md5_result_hex_[32];
  absl::string_view md5_result_hex_view_;
  MD5_CTX ctx;
};

}  // namespace advent_of_code

#endif  // ADVENT_OF_CODE_MD5_H