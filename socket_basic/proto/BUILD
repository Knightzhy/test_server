load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

proto_library(
    name = "helloworld_proto",
    srcs = ["helloworld.proto"],
)
proto_library(
    name = "yy_proto",
    srcs = ["yy.proto"],
)

cc_proto_library(
    name = "helloworld_proto_cc",
    deps = [":helloworld_proto"],
)
cc_proto_library(
    name = "yy_proto_cc",
    deps = [":yy_proto"],
)
