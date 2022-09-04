#include "advent_of_code/infra/file_flags.h"

ABSL_FLAG(absl::Duration, run_long_tests, absl::Seconds(10),
          "Any tests marked [long=$duration] where $duration < "
          "$run_long_tests will be skipped and ignored");

ABSL_FLAG(int64_t, part_filter, 0, "If non-zero, only runs the specified part");
