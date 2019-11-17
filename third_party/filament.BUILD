package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

cc_library(
    name = "filament",
    srcs = glob([
        "libs/utils/include/utils/*.h",
        "libs/utils/include/utils/generic/*.h",
        "libs/image/include/image/*.h",
        "libs/utils/src/*.cpp",
        "libs/utils/src/*.h",
        "libs/math/include/math/*.h",
        "libs/math/src/*.cpp",
        "libs/filabridge/include/filament/*.h",
        "libs/filabridge/include/private/filament/*.h",

"libs/filaflat/include/filaflat/*.h",
        "filament/src/*.cpp",
        "filament/src/components/*.cpp",
        "filament/src/components/*.h",
        "filament/src/fg/*.cpp",
        "filament/src/fg/*.h",
        "filament/src/fg/fg/*.h",
        "filament/src/*.h",
        "filament/src/details/*.h",
        "filament/backend/include/private/backend/*.h",
        "filament/backend/include/private/backend/*.inc",
        "third_party/robin-map/tsl/*.h",
    ]) + [
    ],
    hdrs = glob([
        "filament/include/filament/*.h",
        "filament/backend/include/backend/*.h",
    ]) + [
    ],
    copts = [
        "-std=c++14",
        "-UDEBUG",
    ],
    includes = [
        "filament/backend/include",
        "filament/include",
        "filament/src",
        "libs/math/include",
        "libs/image/include",
        "libs/utils/include",
        "libs/utils/src",
"libe/filabridge/include/private/",
"libs/filabridge/include",
"libs/filaflat/include",
        "third_party/robin-map",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
)
