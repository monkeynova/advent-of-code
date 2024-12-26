# A place to work on personal attempts for [adventofcode.com](https://adventofcode.com) #

## Directory Organization ##

The organization of this repository is as follows:

 * Day specific work is found in advent_of_code/YYYY/dayDD
 * Useful tools across multiple years is in advent_of_code/.
 * Useful tools across multiple days in a single year is in
 advent_of_code/YYYY/. (int_code.{h,cc} is the best example)
 * Infrastructure around how to build tests and benchmarks is in
 advent_of_code/infra
 * Code not created by me is in third_party/... (currently only an MD5
 implementation)

## Top Level Tools ##

* ```:run_year [--year=<year>]``` runs all of my input for the given year
(which defaults to the current year) and displays the results as well as
timings in an ASCII table.
* ```:benchmark``` runs benchmarks for my input for each day in all years.
```--benchmark_flags=--benchmark_filter=.*YYYY/DD.*``` will filter the run
to a single day.
* ```:latest``` runs the test for the most recent day. Tools use this day when
interacting with [adventofcode.com](https://adventofcode.com/).
 
## Per-Day File Organization ##

 Any given day is broken down into the following files:
 
 * dayDD.cc - The code implementing a solution of the days puzzle
 * dayDD.test - A [file_based_test_driver](
https://github.com/google/file-based-test-driver)
 test suite for the day puzzle. It also contains a benchmark for each of
 the tests in the suite.
 * input.txt - My input from [adventofcode.com](https://adventofcode.com)
 (and referenced by dayDD.test)
 * dayDD_test.cc - A test verifying the correctness of dayDD.cc
 * README.md - A copy of the problem from [adventofcode.com](
https://adventofcode.com) translated to markdown
 
## Solve Process ##

 The process for solving a new day is as follows
 
### Before Unlock
 1. ```tools/new_day.pl YYYY DD``` (Creates an empty path)
 2. ```export AOC_SESSION=&lt;session cookie from browser&gt;```
 (Allow command line tools access to my account)
 3. ```export USE_BAZEL_VERSION=`bazel --version | cut -f2 -d\ ` ```
 (Ensure that bazelisk doesn't make network calls that delay builds)

### Solving
 4. ```tools/get_input.pl``` (Downloads personal input to input.txt)
 5. ```bazel run --config=compete :latest -- --part_filter=1``` (Solve part 1)
 6. ```bazel run --config=compete :latest -- --part_filter=2``` (Solve part 2)
 7. ```git add .; git commit -m 'YYYY/DD Answer'```
 (Ensure "as solved" is visible in the repo)
 8. ```tools/get_problem.pl``` (Downloads the problem and builds a README.md)

### After ensuring a full leaderboard (if relevant)
 9. ```git push```
