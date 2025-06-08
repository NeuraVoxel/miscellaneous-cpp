# Hello World with Bazel

这是一个使用Bazel构建系统的Hello World示例项目，支持调试功能。

## 项目结构

```
hello-bazel/
├── WORKSPACE    # Bazel工作空间文件
├── BUILD        # Bazel构建规则
├── main.cpp     # 源代码
├── .bazelrc     # Bazel配置文件
├── .vscode/     # VSCode配置目录
│   ├── launch.json    # 调试配置
│   └── tasks.json     # 任务配置
└── README.md    # 本文档
```

## 前置要求

- 安装Bazel构建系统
- 安装C++编译器（如GCC或Clang）
- 安装调试器（如GDB或LLDB）
- （可选）安装Visual Studio Code及C/C++扩展

## 构建步骤

1. 普通构建：
```bash
bazel build //:hello_world
```

2. 调试构建：
```bash
bazel build --config=debug //:hello_world
```

## 运行程序

```bash
bazel run //:hello_world
```

## 调试程序

### 使用VSCode调试（推荐）

1. 在VSCode中打开项目目录
2. 打开要调试的源文件（如main.cpp）
3. 设置断点（点击行号左侧或按F9）
4. 按F5启动调试
   - 这会自动触发带调试信息的构建
   - 然后启动调试器

VSCode中可用的预定义任务（通过 Cmd+Shift+P 或 Ctrl+Shift+P 打开命令面板并输入 "Tasks: Run Task"）：
- `bazel-build-debug`: 构建带调试信息的版本
- `bazel-build`: 构建常规版本
- `bazel-clean`: 清理构建产物

### 使用命令行调试器

#### LLDB调试：
```bash
# 首先构建带调试信息的版本
bazel build --config=debug //:hello_world

# 启动LLDB
lldb ./bazel-bin/hello_world
```

在LLDB中的基本命令：
```bash
(lldb) breakpoint set --file main.cpp --line 4  # 设置断点
(lldb) run                                      # 运行程序
(lldb) step                                     # 单步执行
(lldb) continue                                 # 继续执行
(lldb) quit                                     # 退出调试器
```

#### GDB调试：
```bash
# 首先构建带调试信息的版本
bazel build --config=debug //:hello_world

# 启动GDB
gdb ./bazel-bin/hello_world
```

# 启动nemiver
```
nemiver
```

在GDB中的基本命令：
```bash
(gdb) break main.cpp:4  # 设置断点
(gdb) run              # 运行程序
(gdb) step             # 单步执行
(gdb) continue         # 继续执行
(gdb) quit             # 退出调试器
```

## 注意事项

- 确保使用`--config=debug`选项进行构建以包含完整的调试信息
- 调试二进制文件位于`bazel-bin`目录下
- `.bazelrc`文件包含了调试相关的编译选项配置
- VSCode调试配置已预设置好，可以直接使用F5启动调试