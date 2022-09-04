alias(
    name = "cleanup",
    actual = "@com_monkeynova_base_workspace//:cleanup",
)

alias(
    name = "update_workspace",
    actual = "@com_monkeynova_base_workspace//:update_workspace",
)

sh_test(
    name = "diff_bazelrc_test",
    srcs = ["diff_bazelrc_test.sh"],
    data = [
        "base-bazelrc",
        "@com_monkeynova_base_workspace//:base-bazelrc-file",
    ],
)
