#!/bin/bash

# 进入项目目录
cd "$(dirname "$0")"

# 使用Bazel构建项目
bazel build //... --compilation_mode=dbg

# 检查构建是否成功
if [ $? -ne 0 ]; then
    echo "构建失败，请检查错误信息"
    exit 1
fi

# 创建软链接
# 注意：如果软链接已存在，先删除它
if [ -L "bazel-bin" ]; then
    rm bazel-bin
fi

# 获取实际的bazel-bin路径
BAZEL_BIN_PATH=$(bazel info bazel-bin)

# 创建软链接
ln -s "$BAZEL_BIN_PATH" bazel-bin

echo "构建完成，软链接已创建"
echo "你可以在VSCode中使用F5启动调试"