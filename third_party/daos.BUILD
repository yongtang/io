package(default_visibility = ["//visibility:public"])

cc_library(
    name = "daos",
    srcs = glob(
        [
            "src/client/dfus/*.c",
            "src/client/dfus/*.h",
            "src/client/dfs/*.c",
            "src/client/dfs/*.h",
            "src/client/array/*.c",
            "src/client/array/*.h",
            "src/gurt/*.c",
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
        "src/client/dfs",
        "src/gurt",
        "src/include",
    ],
    deps = [
        "@boost",
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
