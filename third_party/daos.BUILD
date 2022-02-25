package(default_visibility = ["//visibility:public"])

cc_library(
    name = "daos",
    srcs = glob(
        [
            "src/client/dfs/dfs.c",
            "src/client/array/dc_array.c",
            "src/gurt/debug.c",
            "src/common/debug.c",
            "src/gurt/fault_inject.c",
            "src/gurt/dlog.c",
            "src/client/api/container.c",
            "src/client/api/init.c",
            "src/container/cli.c",
            "src/container/rpc.c",
            "src/client/kv/dc_kv.c",
            "src/object/cli_obj.c",
            "src/client/api/*.h",
            "src/container/*.h",
            "src/mgmt/*.h",
            "src/object/*.h",
            "src/client/dfs/*.h",
            "src/client/dfus/*.h",
            "src/client/serialize/*.h",
            "src/client/array/*.h",
            "src/gurt/*.h",
        ],
        exclude = [
        ],
    ),
    hdrs = glob(
        [
            "src/include/**/*.h",
        ],
    ) + [
        "src/include/daos_version.h",
    ],
    copts = [],
    defines = [
        "_POSIX_SOURCE",
        "_POSIX_C_SOURCE=200809L",
        "_DEFAULT_SOURCE",
    ],
    includes = [
        "src/client/api",
        "src/client/dfs",
        "src/gurt",
        "src/include",
        "src/include/daos",
        "src/mgmt",
    ],
    deps = [
        "@boost",
        "@libyaml",
        "@protobuf-c",
        "@util_linux//:uuid",
    ],
)

genrule(
    name = "daos_version_h",
    srcs = [
        "src/include/daos_version.h.in",
    ],
    outs = [
        "src/include/daos_version.h",
    ],
    cmd = ("sed " +
           "-e 's/@TMPL_MAJOR@/2/g' " +
           "-e 's/@TMPL_MINOR@/0/g' " +
           "-e 's/@TMPL_FIX@/0/g' " +
           "$< >$@"),
)
