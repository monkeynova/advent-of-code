load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "advent_day",
    hdrs = ["advent_day.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
    ],
)

cc_library(
    name = "file_util",
    srcs = ["file_util.cc"],
    hdrs = ["file_util.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "file_test",
    testonly = 1,
    srcs = ["file_test.cc"],
    hdrs = ["file_test.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":advent_day",
        ":file_test_options",
        ":file_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/functional:bind_front",
        "@com_google_absl//absl/strings",
        "@com_google_file_based_test_driver//file_based_test_driver",
        "@com_google_file_based_test_driver//file_based_test_driver:run_test_case_result",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_googlesource_code_re2//:re2",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_library(
    name = "file_test_options",
    testonly = 1,
    srcs = ["file_test_options.cc"],
    hdrs = ["file_test_options.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/status:statusor",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "point",
    hdrs = ["point.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/hash",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "opt_cmp",
    hdrs = ["opt_cmp.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/types:optional",
    ],
)

cc_library(
    name = "bfs",
    hdrs = ["bfs.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "char_board",
    hdrs = ["char_board.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":point",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "file_benchmark",
    testonly = 1,
    srcs = ["file_benchmark.cc"],
    hdrs = ["file_benchmark.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":advent_day",
        ":file_test_options",
        ":file_util",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_benchmark//:benchmark",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_library(
    name = "md5",
    hdrs = ["md5.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//third_party/md5",
    ],
)
