load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "day16",
    srcs = ["day16.cc"],
    hdrs = ["day16.h"],
    data = [
        "input.txt",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":bits_expr",
        "//advent_of_code:advent_day",
        "@com_google_absl//absl/algorithm:container",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/strings",
        "@com_googlesource_code_re2//:re2",
    ],
    alwayslink = 1,
)

cc_library(
    name = "bits_expr",
    srcs = ["bits_expr.cc"],
    hdrs = ["bits_expr.h"],
    deps = [
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_test(
    name = "bits_expr_test",
    size = "small",
    srcs = ["bits_expr_test.cc"],
    deps = [
        ":bits_expr",
        # Allow file flag options on all tests to avoid test failures in
        # bazelisk test ... --test_arg=<file arg>.
        "//advent_of_code/infra:file_flags",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "day16_test",
    size = "small",
    srcs = ["day16_test.cc"],
    data = [
        "day16.test",
    ],
    deps = [
        ":day16",
        "//advent_of_code/infra:file_benchmark",
        "//advent_of_code/infra:file_test",
    ],
)
