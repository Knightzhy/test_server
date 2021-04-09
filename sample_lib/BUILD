load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")


cc_binary(
    name = "test_lib",
    srcs = ["test_lib.cpp"],
    deps = [":func"],
    linkopts = ["-ldl"],
)

cc_library(
    name = "func",
    srcs = ["func.c"],
    hdrs = ["func.h"],
)
