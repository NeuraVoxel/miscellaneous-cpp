cc_binary(
    name = "hello_debug",
    srcs = ["main.cpp"],
    copts = [
        "-g",
        "-O0",        # 禁用优化以获得更好的调试体验
        "-fno-omit-frame-pointer",  # 保留帧指针
        "-fno-optimize-sibling-calls",  # 禁用尾调用优化
    ],
)