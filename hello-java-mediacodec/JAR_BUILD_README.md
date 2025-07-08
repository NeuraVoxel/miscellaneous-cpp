# MediaCodecHelper JAR构建说明

这个文档提供了关于如何构建和使用MediaCodecHelper JAR文件的说明。

## 构建JAR文件

### 前提条件

- JDK (Java Development Kit) 8或更高版本
- Bash shell环境（Linux、macOS或Windows的Git Bash）

### 构建步骤

1. 确保`build.sh`脚本具有执行权限：
   ```bash
   chmod +x build.sh
   ```

2. 运行构建脚本：
   ```bash
   ./build.sh
   ```

3. 构建成功后，JAR文件将位于`build/mediacodec-helper.jar`

## 关于JAR文件

生成的JAR文件包含以下类：

- `com.example.mediacodec.MediaCodecHelper` - 主类，提供H.265视频编解码功能
- `com.example.mediacodec.MediaCodecHelper$EncodedData` - 编码数据的容器类
- `com.example.mediacodec.MediaCodecHelperDemo` - 演示类，展示如何使用MediaCodecHelper

## 在Android项目中使用JAR文件

### 添加JAR文件到项目

1. 将`mediacodec-helper.jar`复制到Android项目的`libs`目录中
2. 在模块级`build.gradle`文件中添加依赖：
   ```gradle
   dependencies {
       implementation files('libs/mediacodec-helper.jar')
   }
   ```

### 使用MediaCodecHelper类

```java
import com.example.mediacodec.MediaCodecHelper;

// 创建MediaCodecHelper实例
MediaCodecHelper codecHelper = new MediaCodecHelper();

// 初始化编码器
codecHelper.initEncoder(width, height, bitRate, frameRate, iFrameInterval);

// 编码视频帧
byte[] yuvData = ...; // 从相机或其他源获取的YUV数据
long timestamp = System.nanoTime() / 1000;
codecHelper.encodeFrame(yuvData, timestamp);

// 获取编码后的数据
MediaCodecHelper.EncodedData encodedData = codecHelper.getEncodedData(timeoutUs);
if (encodedData != null) {
    // 处理编码后的H.265数据
    // ...
}

// 初始化解码器
Surface outputSurface = ...; // 输出Surface
codecHelper.initDecoder(width, height, outputSurface);

// 解码H.265数据
// ...

// 释放资源
codecHelper.release();
```

## 注意事项

1. **Android依赖**：MediaCodecHelper类依赖于Android SDK，只能在Android项目中使用
2. **设备兼容性**：使用前应检查设备是否支持H.265编解码：
   ```java
   if (MediaCodecHelper.isHevcEncoderSupported()) {
       // 设备支持H.265编码
   }
   
   if (MediaCodecHelper.isHevcDecoderSupported()) {
       // 设备支持H.265解码
   }
   ```
3. **性能考虑**：视频编解码是资源密集型操作，应在后台线程中执行

## 故障排除

如果在使用JAR文件时遇到问题：

1. 确保Android项目的minSdkVersion至少为18（Android 4.3），因为MediaCodec API在此版本中引入
2. 检查设备是否支持H.265编解码
3. 查看Android Studio的Logcat输出，了解可能的错误信息

## 进一步开发

如果需要修改MediaCodecHelper类：

1. 编辑`src/com/example/mediacodec/MediaCodecHelper.java`文件
2. 重新运行构建脚本生成新的JAR文件
3. 更新Android项目中的JAR文件