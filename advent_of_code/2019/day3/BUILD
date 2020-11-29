cc_library(
    name = "day3",
    srcs = ["day3.cc"],
    hdrs = ["day3.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "day3_test",
    srcs = ["day3_test.cc"],
    args = ["--test_file=advent_of_code/2019/day3/day3.test"],
    data = [
        "input.txt",
        "day3.test",
    ],
    deps = [
        ":day3",
        "//advent_of_code:file_test",
    ],
)
