load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "boot_code",
    srcs = ["boot_code.cc"],
    hdrs = ["boot_code.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_googlesource_code_re2//:re2",
    ],
)
