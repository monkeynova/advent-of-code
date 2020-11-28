cc_library(
    name = "new_day",
    srcs = ["new_day.cc"],
    hdrs = ["new_day.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "new_day_test",
    srcs = ["new_day_test.cc"],
    args = ["--test_file=advent_of_code/new_day_path/new_day.test"],
    data = [
        "new_day.test",
        "input.txt",
    ],
    deps = [
        ":new_day",
        "//advent_of_code:file_test",
    ],
)
