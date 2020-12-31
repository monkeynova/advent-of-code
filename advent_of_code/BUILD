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
    name = "md5",
    hdrs = ["md5.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//third_party/md5",
    ],
)

cc_library(
    name = "mod",
    hdrs = ["mod.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "directed_graph",
    hdrs = ["directed_graph.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_glog//:glog",
    ],
)
