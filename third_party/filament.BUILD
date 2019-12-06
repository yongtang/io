# Description:
#   Google Filament Library

licenses(["notice"])  # Apache 2.0 License

exports_files(["LICENSE"])

cc_import(
    name = "libvulkan",
    hdrs = glob([
        "include/backend/*.h",
    ]),
    shared_library = "lib/x86_64/libvulkan.1.dylib",
    alwayslink = 1,
)

cc_import(
    name = "libbluegl",
    hdrs = glob([
        "include/backend/*.h",
    ]),
    static_library = "lib/x86_64/libbluegl.a",
    alwayslink = 1,
)

cc_import(
    name = "libsmol-v",
    hdrs = glob([
        "include/backend/*.h",
    ]),
    static_library = "lib/x86_64/libsmol-v.a",
    alwayslink = 1,
)

cc_import(
    name = "libbluevk",
    hdrs = glob([
        "include/backend/*.h",
    ]),
    static_library = "lib/x86_64/libbluevk.a",
    alwayslink = 1,
)

cc_import(
    name = "libbackend",
    hdrs = glob([
        "include/backend/*.h",
    ]),
    static_library = "lib/x86_64/libbackend.a",
    alwayslink = 1,
)

cc_import(
    name = "libfilaflat",
    hdrs = glob([
        "include/filament/*.h",
    ]),
    static_library = "lib/x86_64/libfilaflat.a",
    alwayslink = 1,
)

cc_import(
    name = "libfilament",
    hdrs = glob([
        "include/filament/*.h",
        "include/utils/*.h",
    ]),
    static_library = "lib/x86_64/libfilament.a",
    alwayslink = 1,
)

cc_import(
    name = "libfilamat",
    hdrs = glob([
        "include/filament/*.h",
    ]),
    static_library = "lib/x86_64/libfilamat.a",
    alwayslink = 1,
)

cc_import(
    name = "libutils",
    hdrs = glob([
        "include/utils/*.h",
    ]),
    static_library = "lib/x86_64/libutils.a",
    alwayslink = 1,
)

cc_import(
    name = "libibl",
    hdrs = glob([
        "include/utils/*.h",
    ]),
    static_library = "lib/x86_64/libibl.a",
    alwayslink = 1,
)

cc_import(
    name = "libgeometry",
    hdrs = glob([
        "include/utils/*.h",
    ]),
    static_library = "lib/x86_64/libgeometry.a",
    alwayslink = 1,
)

cc_import(
    name = "libfilabridge",
    hdrs = glob([
        "include/utils/*.h",
    ]),
    static_library = "lib/x86_64/libfilabridge.a",
    alwayslink = 1,
)

cc_library(
    name = "filament",
    srcs = [],
    hdrs = glob([
        "include/math/*.h",
        "include/backend/*.h",
    ]),
    copts = [],
    includes = [
        "include",
    ],
    linkopts = [],
    visibility = ["//visibility:public"],
    deps = [
        ":libfilaflat",
        ":libfilabridge",
        ":libfilament",
        ":libutils",
        ":libibl",
        ":libbackend",
        ":libvulkan",
        ":libbluevk",
        ":libsmol-v",
        ":libbluegl",
        ":libgeometry",
        #":libfilamat",
    ],
)
