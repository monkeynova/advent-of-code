## A place to work on personal attempts for [adventofcode.com](https://adventofcode.com) ##

The organization of this repository is as follows:

 * Day specific work is found in advent_of_code/YYYY/dayDD
 * Useful tools across various days is in advent_of_code/.
 * Infrastructure around how to build tests and benchmarks is in advent_of_code/infra
 * Code not created by me is in third_party/... (currently only an MD5 implementation)
 
 Any given day is broken down into the following files:
 
 * dayDD.cc - The code implementing a solution of the days puzzle
 * dayDD.test - A [file_based_test_driver](https://github.com/google/file-based-test-driver) test suite for the day puzzle
 * input.txt - My input from [adventofcode.com](https://adventofcode.com) (and referenced by dayDD.test)
 * dayDD_test.cc - A test verifying the correctness of dayDD.cc
 * dayDD_benchmark.cc - A benchmark that will run (and time) dayDD.cc
 * README.md - A copy of the problem from [adventofcode.com](https://adventofcode.com) translated to markdown
 
 The process for solving a new day is as follows
 
 1. Before unlock: ```tools/new_day.pl YYYY DD``` -- Creates an empty path
 2. Before unlock: export AOC_SESSION=&lt;session cookie from browser&gt;
 3. At unlock: ```tools/get_input.pl YYYY DD``` -- Downloads personal input to input.txt
 4. At unlock: ```bazelisk run --config=compete advent_of_code/YYYY/dayDD:dayDD_test -- --part_filter=1```
 5. After solving part 1: ```bazelisk run --config=compete advent_of_code/YYYY/dayDD:dayDD_test -- --part_filter=2```
 6. After solving part 2: ```git add .; git commit -m 'YYYY/DD Answer'``` -- Ensure "as solved" is visible in the repo
 7. After solving part 2: ```tools/get_problem.pl YYYY DD`` -- Downloads the problem and builds a README.md
 8. After ensuring enough folks (300?) have solved: ```git push```
