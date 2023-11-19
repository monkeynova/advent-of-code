#ifndef ADVENT_OF_CODE_VLOG_H
#define ADVENT_OF_CODE_VLOG_H

#include "absl/log/log.h"

// TODO(@monkeynova): Remove when part of absl/log public API.

namespace internal {

// We use a simple boolean cache of the flag value to avoid any costs
// associated with acquiring the value (like mutex acquisition). The
// value must _never_ be set after flag parsing.
extern int verbosity_level;

}  // namespace internal

#define VLOG_IS_ON(s) (s <= internal::verbosity_level)

#define VLOG(s) LOG_IF(INFO, VLOG_IS_ON(s))
#define VLOG_IF(s, cond) LOG_IF(INFO, VLOG_IS_ON(s) && (cond))
#define VLOG_EVERY_N(s, n) LOG_IF_EVERY_N(INFO, VLOG_IS_ON(s), n)
#define VLOG_IF_EVERY_N(s, cond, n) LOG_IF(INFO, VLOG_IS_ON(s) && (cond), n)

#define DVLOG(s) DLOG_IF(INFO, VLOG_IS_ON(s))
#define DVLOG_IF(s, cond) DLOG_IF(INFO, VLOG_IS_ON(s) && (cond))
#define DVLOG_EVERY_N(s, n) DLOG_IF_EVERY_N(INFO, VLOG_IS_ON(s), n)
#define DVLOG_IF_EVERY_N(s, cond, n) DLOG_IF(INFO, VLOG_IS_ON(s) && (cond), n)

#define RETURN_IF_ERROR(arg) {absl::Status st = arg; if (!st.ok()) return st;}

#define ASSIGN_OR_RETURN_CONCAT(x, y) ASSIGN_OR_RETURN_CONCAT_(x, y)
#define ASSIGN_OR_RETURN_CONCAT_(x, y) x ## y

#define ASSIGN_OR_RETURN(lhs, rhs)                      \
  ASSIGN_OR_RETURN_IMPL(                                \
    ASSIGN_OR_RETURN_CONCAT(_status_or_, __LINE__),     \
    lhs, rhs)

#define ASSIGN_OR_RETURN_IMPL(statusor_var, lhs, rhs)   \
  auto statusor_var = (rhs);                            \
  if (!statusor_var.ok()) return statusor_var.status(); \
  lhs = *std::move(statusor_var)

#endif  // ADVENT_OF_CODE_VLOG_H
