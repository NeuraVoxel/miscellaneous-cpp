# LevelDB 使用示例

这个项目展示了如何使用 Bazel 构建系统和 C++ 来操作 LevelDB 数据库。

## 功能演示

这个示例程序展示了以下 LevelDB 操作：
- 创建/打开数据库
- 写入键值对
- 读取键值对
- 批量操作（写入和删除）
- 遍历数据库中的所有键值对

## 构建和运行

### 前提条件

- 安装 [Bazel](https://bazel.build/install)
- C++ 编译器（支持 C++17）

### 构建

在项目根目录下运行：

```bash
bazel build :leveldb_example
```

### 运行

```bash
bazel run :leveldb_example
```

## 项目结构

- `main.cc` - 包含 LevelDB 操作示例的源代码
- `BUILD` - Bazel 构建配置
- `WORKSPACE` - Bazel 工作空间配置，包含 LevelDB 依赖

## 注意事项

- 示例程序会在当前目录下创建一个名为 `testdb` 的 LevelDB 数据库
- 每次运行程序时，都会使用相同的数据库，因此多次运行可能会看到累积的结果