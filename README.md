## A place to work on personal attempts for [adventofcode.com](https://adventofcode.com) ##

### Directory Organization ###

The organization of this repository is as follows:

 * Day specific work is found in advent_of_code/YYYY/dayDD
 * Useful tools across multiple years is in advent_of_code/.
 * Useful tools across multiple days in a single year is in
 advent_of_code/YYYY/. (int_code.{h,cc} is the best example)
 * Infrastructure around how to build tests and benchmarks is in
 advent_of_code/infra
 * Code not created by me is in third_party/... (currently only an MD5
 implementation)

### Top Level Tools ###

* ```:run_year [--year=<year>]``` runs all of my input for the given year
(which defaults to the current year) and displays the results as well as
timings in an ASCII table.
* ```:benchmark``` runs benchmarks for my input for each day in all years.
```--benchmark_flags=--benchmark_filter=.*YYYY/DD.*``` will filter the run
to a single day.
 
### Per-Day File Organization ###

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
 
### Solve Process ###

 The process for solving a new day is as follows
 
 1. Before unlock: ```tools/new_day.pl YYYY DD``` -- Creates an empty path
 2. Before unlock: export AOC_SESSION=&lt;session cookie from browser&gt;
 3. Before unlock: export USE_BAZEL_VERSION=`bazel --version | cut -f2 -d\ ` --
 Ensure that bazelisk doesn't make network calls that delay builds
 4. At unlock: ```tools/get_input.pl YYYY DD``` -- Downloads personal input to
 input.txt
 5. At unlock: ```bazel run --config=compete
 advent_of_code/YYYY/dayDD:dayDD_test -- --part_filter=1```
 6. After solving part 1: ```bazel run --config=compete
 advent_of_code/YYYY/dayDD:dayDD_test -- --part_filter=2```
 7. After solving part 2: ```git add .; git commit -m 'YYYY/DD Answer'``` --
 Ensure "as solved" is visible in the repo
 8. After solving part 2: ```tools/get_problem.pl YYYY DD``` -- Downloads the
 problem and builds a README.md
 9. After ensuring a full leaderboard: ```git push```
