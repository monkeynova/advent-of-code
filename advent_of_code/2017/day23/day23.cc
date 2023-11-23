#include "advent_of_code/2017/day23/day23.h"

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_split.h"
#include "advent_of_code/2017/vm.h"
#include "re2/re2.h"

namespace advent_of_code {

/*
Scratch space working with the input.

b = 65;
c = b;
if (a != 0) {
  b *= 100;
  b += 100000;  // 106500
  c = b;
  c += 17000    // 123500
  f = 1;        // 1
}
while (true) {
  for (d = 2; d != b; ++d) {
    for (e = 2; e != b; ++e) {
      if (d * e == b) f = 0;
    }
  }

  if (f == 0) ++h;
  if (b == c) break;
  b += 17;
}

set b 65
set c b
jnz a 2 {
  jnz 1 5 {
    mul b 100
    sub b -100000
    set c b
    sub c -17000
  }
}
{
  set f 1
  set d 2
  {
    set e 2
    {
      set g d
      mul g e
      sub g b
      jnz g 2 {
        set f 0
      }
      sub e -1
      set g e
      sub g b
      jnz g -8
    }
    sub d -1
    set g d
    sub g b
    jnz g -13
  }
  jnz f 2 {
    sub h -1
  }
  set g b
  sub g c
  jnz g 2 {
    jnz 1 3    // term
  }
  sub b -17
  jnz 1 -23
}

*/

namespace {

std::optional<int64_t> TryMatchProgram(absl::Span<std::string_view> input) {
  std::string program_re =
      "set b (\\d+)\n"
      "set c b\n"
      "jnz a 2\n"
      "jnz 1 5\n"
      "mul b (\\d+)\n"
      "sub b (-?\\d+)\n"
      "set c b\n"
      "sub c (-?\\d+)\n"
      "set f 1\n"
      "set d 2\n"
      "set e 2\n"
      "set g d\n"
      "mul g e\n"
      "sub g b\n"
      "jnz g 2\n"
      "set f 0\n"
      "sub e -1\n"
      "set g e\n"
      "sub g b\n"
      "jnz g -8\n"
      "sub d -1\n"
      "set g d\n"
      "sub g b\n"
      "jnz g -13\n"
      "jnz f 2\n"
      "sub h -1\n"
      "set g b\n"
      "sub g c\n"
      "jnz g 2\n"
      "jnz 1 3\n"
      "sub b (-?\\d+)\n"
      "jnz 1 -23";

  int b_init, b_mul, b_sub, c_sub, b_neg_delta;
  if (!RE2::FullMatch(absl::StrJoin(input, "\n"), program_re, &b_init, &b_mul,
                      &b_sub, &c_sub, &b_neg_delta)) {
    return std::nullopt;
  }

  const int b_start = b_init * b_mul - b_sub;
  const int b_end = b_start - c_sub;
  const int b_delta = -b_neg_delta;
  int h = 0;
  for (int b = b_start; b <= b_end; b += b_delta) {
    bool f = true;
    for (int d = 2; d * d <= b; ++d) {
      if (b % d == 0) {
        f = false;
        break;
      }
    }
    if (!f) ++h;
  }
  return h;
}

}  // namespace

absl::StatusOr<std::string> Day_2017_23::Part1(
    absl::Span<std::string_view> input) const {
  ASSIGN_OR_RETURN(VM_2017 vm, VM_2017::Parse(input));
  vm.ExecuteToRecv();
  return AdventReturn(vm.mul_count());
}

absl::StatusOr<std::string> Day_2017_23::Part2(
    absl::Span<std::string_view> input) const {
  std::optional<int> reimplement_val = TryMatchProgram(input);
  if (reimplement_val) return AdventReturn(reimplement_val);

  ASSIGN_OR_RETURN(VM_2017 vm, VM_2017::Parse(input));
  vm.set_register("a", 1);
  vm.ExecuteToRecv();
  return AdventReturn(vm.get_register("h"));
}

}  // namespace advent_of_code
