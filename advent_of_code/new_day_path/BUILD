load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "new_day",
    srcs = ["new_day.cc"],
    hdrs = ["new_day.h"],
    deps = [
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_test(
    name = "new_day_test",
    srcs = ["new_day_test.cc"],
    args = ["--test_file=advent_of_code/new_day_path/new_day.test"],
    data = [
        "input.txt",
        "new_day.test",
    ],
    deps = [
        ":new_day",
        "//advent_of_code/infra:file_test",
    ],
)

cc_test(
    name = "new_day_benchmark",
    srcs = ["new_day_benchmark.cc"],
    args = ["--test_file=advent_of_code/new_day_path/new_day.test"],
    data = [
        "input.txt",
        "new_day.test",
    ],
    deps = [
        ":new_day",
        "//advent_of_code/infra:file_benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
