cc_library(
    name = "leveldb",
    srcs = glob([
        "db/*.cc",
        "helpers/memenv/*.cc",
        "table/*.cc",
        "util/*.cc",
    ], exclude = [
        "db/db_bench.cc",
        "**/*_test.cc",  # 排除所有测试文件
        "util/testutil.cc",  # 明确排除 testutil.cc
        "util/env_test.cc",
        "util/hash_test.cc",
        "util/env_windows.cc",  # 排除 Windows 平台代码
    ]),
    hdrs = glob([
        "include/leveldb/*.h",
        "db/*.h",
        "helpers/memenv/*.h",
        "table/*.h",
        "util/*.h",
        "port/*.h",
    ], exclude = [
        "**/*_test.h",  # 排除所有测试头文件
        "util/testutil.h",  # 明确排除 testutil.h
    ]),
    includes = ["include/"],
    visibility = ["//visibility:public"],
    copts = [
        "-DLEVELDB_PLATFORM_POSIX",
        "-DLEVELDB_ATOMIC_PRESENT",
    ],
)