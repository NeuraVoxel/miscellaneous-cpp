# Hello Java JAR

一个简单的Java Hello World项目，演示如何将Java程序打包成JAR文件。

## 项目结构

```
hello-java-jar/
├── src/                  # 源代码目录
│   └── com/example/      # Java包结构
│       └── HelloWorld.java  # Hello World程序
├── build.sh              # 构建脚本
└── README.md             # 项目说明文件
```

构建后，将生成以下目录和文件：

```
hello-java-jar/
├── build/                # 构建输出目录
│   ├── classes/          # 编译后的类文件
│   └── hello-world.jar   # 打包后的JAR文件
```

## 构建说明

要构建项目，请在项目根目录下运行构建脚本：

```bash
./build.sh
```

这将编译Java源文件并创建一个可执行的JAR文件。

## 运行说明

构建完成后，可以使用以下命令运行JAR文件：

```bash
java -jar build/hello-world.jar
```

或者按照构建脚本输出的指示运行。

## 要求

- Java开发工具包(JDK) 8或更高版本