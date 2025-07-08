# MediaCodec H.265 编解码示例

这个项目展示了如何使用Android的MediaCodec API封装一个简单的H.265(HEVC)视频编解码器。项目提供了一个易于使用的MediaCodecHelper类，用于处理H.265视频的编码和解码操作。

## 项目结构

```
hello-java-mediacodec/
├── src/
│   └── com/
│       └── example/
│           └── mediacodec/
│               ├── MediaCodecHelper.java     # 核心编解码辅助类
│               ├── MediaCodecExample.java    # 使用示例类
│               ├── MediaCodecActivity.java   # 示例Activity
│               └── activity_media_codec.xml  # Activity布局文件
└── README.md                                 # 项目说明文件
```

## 主要类说明

### MediaCodecHelper

`MediaCodecHelper`是一个封装了Android MediaCodec API的辅助类，专门用于H.265视频的编码和解码。它提供了以下主要功能：

- 初始化H.265编码器和解码器
- 编码原始YUV视频帧为H.265比特流
- 解码H.265比特流为原始视频帧
- 请求关键帧（I帧）
- 检查设备是否支持H.265编解码
- 资源管理和释放

### MediaCodecExample

`MediaCodecExample`是一个示例类，展示了如何使用`MediaCodecHelper`进行H.265编解码。它提供了：

- 编码和解码的回调接口
- 简化的API，用于初始化编解码器和处理视频帧
- 完整的使用示例

### MediaCodecActivity

`MediaCodecActivity`是一个Android Activity示例，展示了如何在实际应用中集成`MediaCodecHelper`。它包括：

- 使用Camera2 API获取相机预览帧
- 将相机预览帧编码为H.265
- 解码H.265并显示在TextureView上
- 简单的用户界面，用于控制编解码过程

## 使用方法

### 1. 初始化编码器

```java
MediaCodecHelper codecHelper = new MediaCodecHelper();
codecHelper.initEncoder(width, height, bitRate, frameRate, iFrameInterval);
```

### 2. 初始化解码器

```java
MediaCodecHelper codecHelper = new MediaCodecHelper();
codecHelper.initDecoder(width, height, outputSurface);
```

### 3. 编码视频帧

```java
// 编码YUV数据
byte[] yuvData = ...; // 从相机或其他源获取的YUV数据
long timestamp = System.nanoTime() / 1000;
codecHelper.encodeFrame(yuvData, timestamp);

// 获取编码后的数据
MediaCodecHelper.EncodedData encodedData = codecHelper.getEncodedData(timeoutUs);
if (encodedData != null) {
    // 处理编码后的H.265数据
    ByteBuffer buffer = encodedData.buffer;
    MediaCodec.BufferInfo bufferInfo = encodedData.bufferInfo;
    
    // 可以将数据保存到文件或通过网络发送
}
```

### 4. 解码H.265数据

```java
// 解码H.265数据
ByteBuffer encodedData = ...; // 从文件或网络获取的H.265数据
int size = ...; // 数据大小
long timestamp = ...; // 时间戳
int flags = ...; // 标志（例如，关键帧标志）

codecHelper.decodeFrame(encodedData, size, timestamp, flags);
```

### 5. 释放资源

```java
// 释放编码器和解码器资源
codecHelper.release();
```

## 注意事项和要求

1. **Android版本要求**：
   - 基本功能需要Android 4.3 (API 18)或更高版本
   - 某些高级功能（如特定的编码参数）可能需要更高版本

2. **H.265支持**：
   - 并非所有Android设备都支持H.265编解码
   - 使用前应调用`MediaCodecHelper.isHevcEncoderSupported()`和`MediaCodecHelper.isHevcDecoderSupported()`检查设备支持

3. **性能考虑**：
   - 视频编解码是资源密集型操作，应在后台线程中执行
   - 对于高分辨率视频，应考虑设备性能限制

4. **权限要求**：
   - 使用相机需要`android.permission.CAMERA`权限
   - 读写外部存储（如保存编码视频）需要相应的存储权限

## 示例代码

完整的使用示例可以参考`MediaCodecExample.java`中的`exampleUsage()`方法：

```java
// 创建示例实例
MediaCodecExample example = new MediaCodecExample();

// 设置编码回调
example.setEncodedFrameCallback(new EncodedFrameCallback() {
    @Override
    public void onEncodedFrame(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo) {
        // 处理编码后的H.265数据
        Log.d(TAG, "收到编码帧: " + bufferInfo.size + " bytes");
    }
});

try {
    // 初始化编码器
    if (example.initEncoder()) {
        // 编码帧
        byte[] yuvFrame = ...; // 从相机或其他源获取的YUV数据
        example.encodeFrame(yuvFrame, System.nanoTime() / 1000);
    }
    
    // 初始化解码器
    if (example.initDecoder(surface)) {
        // 解码操作会在编码回调中进行
    }
} finally {
    // 释放资源
    example.release();
}
```

## 进一步改进

这个项目提供了H.265编解码的基本功能，但在实际应用中，你可能需要考虑以下改进：

1. 添加错误处理和恢复机制
2. 实现更高效的缓冲区管理
3. 添加对不同颜色格式的支持
4. 实现异步编解码
5. 添加更多编码参数控制（如比特率控制模式、质量级别等）