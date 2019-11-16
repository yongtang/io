package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

cc_library(
    name = "filament",
    srcs = glob([
        "libs/utils/include/utils/*.h",
        "libs/math/include/math/*.h",
    ]) + [
    ],
    hdrs = glob([
        "filament/include/filament/*.h",
        "filament/backend/include/backend/*.h",
    ]) + [
    ],
    copts = [
        "-std=c++14",
    ],
    includes = [
        "filament/include",
        "libs/utils/include",
        "libs/math/include",
        "filament/backend/include",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)
