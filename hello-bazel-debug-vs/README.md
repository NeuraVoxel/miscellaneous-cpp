# Hello Bazel Debug VS Project

这是一个使用Bazel构建系统的C++示例项目，主要用于演示VSCode中的调试功能。

## 项目结构

```
.
├── .bazelrc            # Bazel配置文件，包含构建选项和设置
├── .vscode/            # VSCode配置目录
├── BUILD.bazel         # Bazel构建规则文件
├── MODULE.bazel        # Bazel模块定义文件
├── MODULE.bazel.lock   # Bazel模块依赖锁定文件
├── WORKSPACE           # Bazel工作空间定义文件
├── bazel-output/       # Bazel构建输出目录
├── build/              # 构建输出目录
├── main.cpp            # 主程序源文件
└── README.md           # 项目说明文档（本文件）
```

## 主要文件说明

### 源代码文件
- `main.cpp`: 主程序源文件，包含以下功能：
  - 斐波那契数列计算
  - 质数判断
  - 数字因子分析
  - 用户交互界面

### Bazel配置文件
- `.bazelrc`: Bazel的配置文件，定义构建选项和设置
- `BUILD.bazel`: 定义构建规则和目标
- `MODULE.bazel`: 定义项目模块和依赖关系
- `WORKSPACE`: 定义Bazel工作空间和外部依赖

### 输出目录
- `bazel-output/`: Bazel的构建输出目录，包含：
  - 构建缓存
  - 编译产物
  - 构建日志
  - 性能分析数据
  - 外部依赖
  
- `build/`: 项目构建输出目录

### IDE配置
- `.vscode/`: VSCode配置目录，包含：
  - 调试配置
  - 任务定义
  - 编辑器设置

## 功能特性

主程序（main.cpp）提供以下功能：

1. **数字分析工具**
   - 计算斐波那契数列
   - 判断质数
   - 查找数字的所有因子

2. **用户交互**
   - 命令行交互界面
   - 输入验证和错误处理
   - 历史记录追踪

3. **调试功能**
   - 支持断点调试
   - 变量监视
   - 调用栈跟踪

## 构建和运行

使用Bazel构建项目：
```bash
bazel build //:hello_debug --config=debug --spawn_strategy=local --verbose_failures
```

运行程序：
```bash
bazel run //:main
```

## 调试

本项目配置了VSCode调试支持，可以：
1. 在代码中设置断点
2. 使用VSCode调试器启动程序
3. 监视变量和调用栈
4. 单步执行代码

推荐的断点位置：
- fibonacci函数：观察递归调用
- isPrime函数：检查质数判断逻辑
- processNumber函数：跟踪数字处理流程
- 主循环：监控用户输入处理

## 注意事项

1. 不要手动修改`bazel-output`目录中的文件
2. 使用`bazel clean`清理构建缓存
3. 确保安装了所需的构建工具和依赖