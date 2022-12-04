#ifndef ADVENT_OF_CODE_VLOG_H
#define ADVENT_OF_CODE_VLOG_H

namespace internal {

// We use a simple boolean cache of the flag value to avoid any costs
// associated with acquiring the value (like mutex acquisition). The
// value must _never_ be set after flag parsing.
extern int verbosity_level;

}  // namespace internal

#define VLOG_IS_ON(s) (s <= internal::verbosity_level)

// TODO(@monkeynova): Remove when part of absl/log public API.
#define VLOG(s) LOG_IF(INFO, VLOG_IS_ON(s))
#define VLOG_IF(s, cond) LOG_IF(INFO, VLOG_IS_ON(s) && (cond))
#define VLOG_EVERY_N(s, n) LOG_IF_EVERY_N(INFO, VLOG_IS_ON(s), n)
#define VLOG_IF_EVERY_N(s, cond, n) LOG_IF(INFO, VLOG_IS_ON(s) && (cond), n)

#endif  // ADVENT_OF_CODE_VLOG_H
