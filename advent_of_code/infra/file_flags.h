#ifndef ADVENT_OF_CODE_INFRA_FILE_FLAGS_H

#include "absl/flags/flag.h"

extern absl::Flag<absl::Duration> FLAGS_run_long_tests;
extern absl::Flag<int64_t> FLAGS_part_filter;

#endif