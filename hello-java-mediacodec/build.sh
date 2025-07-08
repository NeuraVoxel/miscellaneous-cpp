#!/bin/bash

# MediaCodecHelper JAR构建脚本
# 此脚本将创建一个包含MediaCodecHelper存根类的JAR文件
# 注意：实际使用时，应在Android项目中使用真正的MediaCodecHelper类

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# 设置目录
SRC_DIR="src"
BUILD_DIR="build"
CLASSES_DIR="$BUILD_DIR/classes"
JAR_FILE="$BUILD_DIR/mediacodec-helper.jar"
TEMP_DIR="$BUILD_DIR/temp"

# 创建构建目录
echo -e "${GREEN}创建构建目录...${NC}"
mkdir -p "$CLASSES_DIR"
mkdir -p "$TEMP_DIR"

# 由于MediaCodecHelper.java依赖于Android SDK，我们将创建一个存根类
echo -e "${YELLOW}注意: MediaCodecHelper.java依赖于Android SDK${NC}"
echo -e "${YELLOW}创建存根类以演示JAR打包过程...${NC}"

# 创建存根类目录
mkdir -p "$TEMP_DIR/com/example/mediacodec"

# 创建MediaCodecHelper存根类
cat > "$TEMP_DIR/com/example/mediacodec/MediaCodecHelper.java" << EOF
package com.example.mediacodec;

/**
 * MediaCodec辅助类存根，用于H.265(HEVC)视频的编码和解码
 * 
 * 注意：这只是一个存根类，用于演示JAR打包过程
 * 实际使用时，应在Android项目中使用真正的MediaCodecHelper类
 */
public class MediaCodecHelper {
    private static final String TAG = "MediaCodecHelper";
    
    /**
     * 编码后的数据结构
     */
    public static class EncodedData {
        public Object buffer;
        public Object bufferInfo;
        
        public EncodedData(Object buffer, Object bufferInfo) {
            this.buffer = buffer;
            this.bufferInfo = bufferInfo;
        }
    }
    
    /**
     * 构造函数
     */
    public MediaCodecHelper() {
        System.out.println("MediaCodecHelper初始化");
        System.out.println("注意：这只是一个存根类，用于演示JAR打包过程");
        System.out.println("实际使用时，应在Android项目中使用真正的MediaCodecHelper类");
    }
    
    /**
     * 初始化H.265编码器
     */
    public void initEncoder(int width, int height, int bitRate, int frameRate, int iFrameInterval) {
        System.out.println("初始化编码器（存根方法）");
    }
    
    /**
     * 初始化H.265解码器
     */
    public void initDecoder(int width, int height, Object outputSurface) {
        System.out.println("初始化解码器（存根方法）");
    }
    
    /**
     * 编码一帧YUV数据
     */
    public boolean encodeFrame(byte[] input, long presentationTimeUs) {
        System.out.println("编码帧（存根方法）");
        return true;
    }
    
    /**
     * 解码一帧H.265数据
     */
    public boolean decodeFrame(Object input, int size, long presentationTimeUs, int flags) {
        System.out.println("解码帧（存根方法）");
        return true;
    }
    
    /**
     * 获取编码后的数据
     */
    public EncodedData getEncodedData(long timeoutUs) {
        System.out.println("获取编码数据（存根方法）");
        return null;
    }
    
    /**
     * 请求关键帧（I帧）
     */
    public void requestKeyFrame() {
        System.out.println("请求关键帧（存根方法）");
    }
    
    /**
     * 释放资源
     */
    public void release() {
        System.out.println("释放资源（存根方法）");
    }
    
    /**
     * 检查设备是否支持H.265编码
     */
    public static boolean isHevcEncoderSupported() {
        System.out.println("检查H.265编码器支持（存根方法）");
        return false;
    }
    
    /**
     * 检查设备是否支持H.265解码
     */
    public static boolean isHevcDecoderSupported() {
        System.out.println("检查H.265解码器支持（存根方法）");
        return false;
    }
}
EOF

# 编译存根类
echo -e "${GREEN}编译存根类...${NC}"
javac "$TEMP_DIR/com/example/mediacodec/MediaCodecHelper.java" -d "$CLASSES_DIR"

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo -e "${RED}编译存根类失败!${NC}"
    exit 1
fi

# 创建示例类
echo -e "${GREEN}创建示例类...${NC}"
cat > "$TEMP_DIR/com/example/mediacodec/MediaCodecHelperDemo.java" << EOF
package com.example.mediacodec;

/**
 * MediaCodecHelper使用示例类
 */
public class MediaCodecHelperDemo {
    public static void main(String[] args) {
        System.out.println("MediaCodecHelper JAR演示");
        
        // 创建MediaCodecHelper实例
        MediaCodecHelper helper = new MediaCodecHelper();
        
        // 初始化编码器
        helper.initEncoder(1280, 720, 2000000, 30, 1);
        
        // 初始化解码器
        helper.initDecoder(1280, 720, null);
        
        // 释放资源
        helper.release();
        
        System.out.println("\n注意：这只是一个存根演示");
        System.out.println("实际使用时，应在Android项目中使用真正的MediaCodecHelper类");
    }
}
EOF

# 编译示例类
echo -e "${GREEN}编译示例类...${NC}"
javac "$TEMP_DIR/com/example/mediacodec/MediaCodecHelperDemo.java" -cp "$CLASSES_DIR" -d "$CLASSES_DIR"

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo -e "${RED}编译示例类失败!${NC}"
    exit 1
fi

# 清理临时文件
rm -rf "$TEMP_DIR"

# 创建JAR文件
echo -e "${GREEN}创建JAR文件...${NC}"
jar cf "$JAR_FILE" -C "$CLASSES_DIR" .

# 检查JAR创建是否成功
if [ $? -ne 0 ]; then
    echo -e "${RED}创建JAR文件失败!${NC}"
    exit 1
fi

echo -e "${GREEN}构建成功!${NC}"
echo -e "${GREEN}JAR文件位置: $JAR_FILE${NC}"

# 提供使用说明
echo -e "\n${YELLOW}使用说明:${NC}"
echo -e "${YELLOW}1. 在Android项目中，将此JAR文件添加到libs目录${NC}"
echo -e "${YELLOW}2. 在build.gradle文件中添加:${NC}"
echo -e "${YELLOW}   implementation files('libs/mediacodec-helper.jar')${NC}"
echo -e "${YELLOW}3. 在代码中导入并使用MediaCodecHelper类${NC}"
echo -e "${YELLOW}   import com.example.mediacodec.MediaCodecHelper;${NC}"
echo -e "\n${YELLOW}注意: 此JAR文件依赖于Android SDK，只能在Android项目中使用${NC}"