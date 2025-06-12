# WebSocket示例

这是一个使用Boost.Beast库实现的WebSocket客户端和服务器示例，支持断点调试。

## 功能特点

- 基于Boost.Beast的WebSocket实现
- 异步I/O操作
- 服务器支持多客户端连接
- 客户端可发送消息并接收服务器响应
- 添加了详细的调试输出和断点位置标记
- 支持CMake构建系统

## 依赖项

- C++17兼容的编译器
- Boost库 (1.70.0或更高版本)
- CMake (3.10或更高版本)

## 构建说明

### 1. 创建构建目录

```bash
cd websocket-example
mkdir build
cd build
```

### 2. 配置CMake项目

```bash
cmake ..
```

### 3. 构建项目

```bash
cmake --build .
```

构建完成后，可执行文件将位于`bin`目录中。

## 运行说明

### 启动服务器

```bash
./bin/websocket_server 0.0.0.0 8080
```

这将启动WebSocket服务器，监听所有网络接口的8080端口。

### 运行客户端

```bash
./bin/websocket_client localhost 8080 "Hello, WebSocket!"
```

这将启动WebSocket客户端，连接到本地服务器，并发送消息"Hello, WebSocket!"。

## 断点调试说明

代码中已经添加了详细的调试输出，并标记了适合设置断点的位置。以下是一些关键的断点位置：

### 服务器端断点位置

- `main()`: 服务器配置和启动
- `listener::run()`: 服务器开始监听连接
- `listener::on_accept()`: 接受新连接
- `session::run()`: 会话开始运行
- `session::on_accept()`: WebSocket握手完成
- `session::on_read()`: 接收到客户端消息
- `session::on_write()`: 消息发送完成

### 客户端断点位置

- `session::run()`: 客户端配置
- `session::on_resolve()`: 域名解析成功
- `session::on_connect()`: 连接成功
- `session::on_handshake()`: WebSocket握手成功
- `session::on_write()`: 消息发送成功
- `session::on_read()`: 接收到服务器响应
- `session::on_close()`: 连接关闭

### 使用VSCode进行调试

1. 安装C/C++扩展
2. 创建`.vscode/launch.json`文件，配置如下：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug WebSocket Server",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/websocket_server",
            "args": ["0.0.0.0", "8080"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        },
        {
            "name": "Debug WebSocket Client",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/websocket_client",
            "args": ["localhost", "8080", "Hello, WebSocket!"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

注意：在macOS上，您可能需要将`MIMode`更改为`lldb`。

### 使用CLion进行调试

1. 打开项目文件夹
2. CLion会自动识别CMakeLists.txt
3. 配置运行/调试配置，添加命令行参数
4. 在代码中设置断点
5. 点击调试按钮开始调试

## 代码结构

- `CMakeLists.txt`: CMake构建配置
- `websocket_server.cpp`: WebSocket服务器实现
- `websocket_client.cpp`: WebSocket客户端实现