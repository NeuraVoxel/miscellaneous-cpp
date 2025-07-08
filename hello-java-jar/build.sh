#!/bin/bash

# 设置变量
PROJECT_ROOT=$(pwd)
SRC_DIR="$PROJECT_ROOT/src"
BUILD_DIR="$PROJECT_ROOT/build"
CLASSES_DIR="$BUILD_DIR/classes"
JAR_FILE="$BUILD_DIR/hello-world.jar"
MAIN_CLASS="com.example.HelloWorld"

# 创建构建目录
echo "创建构建目录..."
mkdir -p "$CLASSES_DIR"

# 编译Java文件
echo "编译Java文件..."
javac -d "$CLASSES_DIR" $(find "$SRC_DIR" -name "*.java")

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo "编译失败！"
    exit 1
fi

# 创建JAR文件
echo "创建JAR文件..."
jar cvfe "$JAR_FILE" "$MAIN_CLASS" -C "$CLASSES_DIR" .

# 检查JAR创建是否成功
if [ $? -ne 0 ]; then
    echo "JAR文件创建失败！"
    exit 1
fi

echo "构建成功！"
echo "JAR文件位置: $JAR_FILE"
echo "运行JAR文件: java -jar $JAR_FILE"