# VSCode Bazel C++ 调试示例

这是一个演示如何在 VSCode 中使用断点调试 Bazel 构建的 C++ 代码的示例项目。

## 环境要求

- Bazel (最新版本)
- VSCode
- C/C++ VSCode 扩展
- GDB 调试器

## 使用步骤

1. 打开项目：
   ```bash
   code hello-bazel-debug
   ```

2. 在 VSCode 中安装 C/C++ 扩展（如果尚未安装）。

3. 在 main.cpp 中设置断点：
   - 点击行号左侧设置断点
   - 推荐的断点位置已在代码注释中标注

4. 启动调试：
   - 按 F5 或点击调试菜单中的"开始调试"
   - VSCode 将自动执行以下操作：
     * 使用 Bazel 构建项目（通过 tasks.json 中定义的任务）
     * 启动调试器
     * 在断点处暂停执行

5. 调试操作：
   - F10：单步执行（跳过函数）
   - F11：单步执行（进入函数）
   - F5：继续执行
   - Shift+F5：停止调试

## 调试提示

1. 观察变量：
   - 使用 VSCode 的"变量"窗口查看当前作用域中的变量值
   - 将鼠标悬停在代码中的变量上查看其值

2. 调试控制台：
   - 使用调试控制台执行表达式
   - 在暂停时检查变量值

3. 断点类型：
   - 条件断点：右键点击断点，添加条件
   - 日志点：右键点击行号，添加日志点，打印信息而不暂停

## 项目结构

```
hello-bazel-debug/
├── WORKSPACE          # Bazel 工作区文件
├── src/
│   ├── BUILD         # Bazel 构建配置
│   └── main.cpp      # 示例 C++ 代码
└── .vscode/
    ├── launch.json   # VSCode 调试配置
    └── tasks.json    # VSCode 任务配置
```

## 故障排除

1. 如果调试器无法启动：
   - 确保已安装 GDB
   - 检查 launch.json 中的 miDebuggerPath 是否正确

2. 如果构建失败：
   - 检查 Bazel 是否正确安装
   - 确保 BUILD 文件中的目标名称正确

3. 如果断点不起作用：
   - 确保使用了 -g 编译选项（已在 BUILD 文件中配置）
   - 重新构建项目并重启调试会话