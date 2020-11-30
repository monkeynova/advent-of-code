load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "int_code",
    srcs = ["int_code.cc"],
    hdrs = ["int_code.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "int_code_test",
    srcs = ["int_code_test.cc"],
    data = ["int_code.test"],
    args = ["--test_file=advent_of_code/2019/int_code.test"],
    deps = [
        ":int_code",
        "@com_google_file_based_test_driver//:file_based_test_driver",
        "@com_google_file_based_test_driver//:run_test_case_result",
        "@com_google_file_based_test_driver//:test_case_options",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)
