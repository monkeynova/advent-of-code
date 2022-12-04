#include "advent_of_code/infra/file_flags.h"

ABSL_FLAG(absl::Duration, run_long_tests, absl::Minutes(1),
          "Any tests marked [long=$duration] where $duration < "
          "$run_long_tests will be skipped and ignored");

ABSL_FLAG(bool, fail_if_long_skip, false,
          "If true, any test that is skipped as a result of --run_long_tests "
          "results in a test failure. This is off by default, but useful "
          "for finding tests that take longer than $TIME to run.");

ABSL_FLAG(int64_t, part_filter, 0, "If non-zero, only runs the specified part");
