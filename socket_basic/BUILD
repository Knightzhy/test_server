load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "test",
    srcs = ["test.cpp"],
    deps = ["//test_server/socket_basic/proto:helloworld_proto_cc"],
)

cc_binary(
    name = "server",
    srcs = ["server.cpp", "types.h"],
    deps = [
        "//test_server/socket_basic/proto:helloworld_proto_cc",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-pthread"],
)

cc_binary(
    name = "client",
    srcs = ["client.cpp", "types.h"],
    deps = [
        "//test_server/socket_basic/proto:helloworld_proto_cc",
        "@com_google_googletest//:gtest_main",
    ],
    linkopts = ["-pthread"],
)