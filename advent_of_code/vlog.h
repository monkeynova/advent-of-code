#ifndef ADVENT_OF_CODE_VLOG_H
#define ADVENT_OF_CODE_VLOG_H

#include "absl/flags/flag.h"
#include "advent_of_code/infra/file_flags.h"

#define VLOG_IS_ON(s) (s <= absl::GetFlag(FLAGS_v))

// TODO(@monkeynova): Remove when part of absl/log public API.
#define VLOG(s) LOG_IF(INFO, VLOG_IS_ON(s))
#define VLOG_IF(s, cond) LOG_IF(INFO, VLOG_IS_ON(s) && (cond))
#define VLOG_EVERY_N(s, n) LOG_IF_EVERY_N(INFO, VLOG_IS_ON(s), n)
#define VLOG_IF_EVERY_N(s, cond, n) LOG_IF(INFO, VLOG_IS_ON(s) && (cond), n)

#endif  // ADVENT_OF_CODE_VLOG_H
