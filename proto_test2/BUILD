load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

proto_library(
    name = "msg_proto",
    srcs = ["msg.proto"],
)

cc_proto_library(
    name = "msg_proto_cc",
    deps =[
        ":msg_proto"
    ],
)