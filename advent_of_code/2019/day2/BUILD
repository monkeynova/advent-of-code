cc_library(
    name = "day2",
    srcs = ["day2.cc"],
    hdrs = ["day2.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "day2_test",
    srcs = ["day2_test.cc"],
    args = ["--test_file=advent_of_code/2019/day2/day2.test"],
    data = [
        "day2.test",
        "input.txt",
    ],
    deps = [
        ":day2",
        "//advent_of_code:file_test",
    ],
)
