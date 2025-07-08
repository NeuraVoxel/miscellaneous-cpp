package com.example.mediacodec;

import android.media.MediaCodec;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * MediaCodecHelper使用示例类
 * 展示如何使用MediaCodecHelper进行H.265编解码
 */
public class MediaCodecExample {
    private static final String TAG = "MediaCodecExample";
    
    // 视频参数
    private static final int VIDEO_WIDTH = 1280;
    private static final int VIDEO_HEIGHT = 720;
    private static final int VIDEO_BIT_RATE = 2000000; // 2 Mbps
    private static final int VIDEO_FRAME_RATE = 30;
    private static final int VIDEO_I_FRAME_INTERVAL = 1; // 1秒一个I帧
    
    // MediaCodecHelper实例
    private MediaCodecHelper mMediaCodecHelper;
    
    // 编码和解码的回调接口
    public interface EncodedFrameCallback {
        void onEncodedFrame(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo);
    }
    
    public interface DecodedFrameCallback {
        void onDecodedFrame(long presentationTimeUs);
    }
    
    // 回调实例
    private EncodedFrameCallback mEncodedFrameCallback;
    private DecodedFrameCallback mDecodedFrameCallback;
    
    /**
     * 构造函数
     */
    public MediaCodecExample() {
        mMediaCodecHelper = new MediaCodecHelper();
    }
    
    /**
     * 设置编码帧回调
     * 
     * @param callback 回调接口
     */
    public void setEncodedFrameCallback(EncodedFrameCallback callback) {
        mEncodedFrameCallback = callback;
    }
    
    /**
     * 设置解码帧回调
     * 
     * @param callback 回调接口
     */
    public void setDecodedFrameCallback(DecodedFrameCallback callback) {
        mDecodedFrameCallback = callback;
    }
    
    /**
     * 初始化编码器
     * 
     * @return 是否成功初始化
     */
    public boolean initEncoder() {
        try {
            // 检查设备是否支持H.265编码
            if (!MediaCodecHelper.isHevcEncoderSupported()) {
                Log.e(TAG, "设备不支持H.265编码");
                return false;
            }
            
            // 初始化编码器
            mMediaCodecHelper.initEncoder(
                VIDEO_WIDTH,
                VIDEO_HEIGHT,
                VIDEO_BIT_RATE,
                VIDEO_FRAME_RATE,
                VIDEO_I_FRAME_INTERVAL
            );
            
            return true;
        } catch (IOException e) {
            Log.e(TAG, "初始化编码器失败: " + e.getMessage());
            return false;
        }
    }
    
    /**
     * 初始化解码器
     * 
     * @param outputSurface 输出Surface，可以为null
     * @return 是否成功初始化
     */
    public boolean initDecoder(Surface outputSurface) {
        try {
            // 检查设备是否支持H.265解码
            if (!MediaCodecHelper.isHevcDecoderSupported()) {
                Log.e(TAG, "设备不支持H.265解码");
                return false;
            }
            
            // 初始化解码器
            mMediaCodecHelper.initDecoder(
                VIDEO_WIDTH,
                VIDEO_HEIGHT,
                outputSurface
            );
            
            return true;
        } catch (IOException e) {
            Log.e(TAG, "初始化解码器失败: " + e.getMessage());
            return false;
        }
    }
    
    /**
     * 编码一帧YUV数据
     * 
     * @param yuvData YUV数据（YUV420格式）
     * @param timestamp 时间戳（微秒）
     * @return 是否成功提交编码
     */
    public boolean encodeFrame(byte[] yuvData, long timestamp) {
        // 提交帧进行编码
        boolean result = mMediaCodecHelper.encodeFrame(yuvData, timestamp);
        
        // 获取编码后的数据
        if (result && mEncodedFrameCallback != null) {
            MediaCodecHelper.EncodedData encodedData = mMediaCodecHelper.getEncodedData(0);
            while (encodedData != null) {
                // 回调编码后的数据
                mEncodedFrameCallback.onEncodedFrame(encodedData.buffer, encodedData.bufferInfo);
                
                // 获取下一帧编码数据
                encodedData = mMediaCodecHelper.getEncodedData(0);
            }
        }
        
        return result;
    }
    
    /**
     * 解码一帧H.265数据
     * 
     * @param encodedData 编码后的H.265数据
     * @param bufferInfo 缓冲区信息
     * @return 是否成功提交解码
     */
    public boolean decodeFrame(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo) {
        // 提交帧进行解码
        boolean result = mMediaCodecHelper.decodeFrame(
            encodedData,
            bufferInfo.size,
            bufferInfo.presentationTimeUs,
            bufferInfo.flags
        );
        
        // 回调解码完成
        if (result && mDecodedFrameCallback != null) {
            mDecodedFrameCallback.onDecodedFrame(bufferInfo.presentationTimeUs);
        }
        
        return result;
    }
    
    /**
     * 请求关键帧（I帧）
     */
    public void requestKeyFrame() {
        mMediaCodecHelper.requestKeyFrame();
    }
    
    /**
     * 释放资源
     */
    public void release() {
        if (mMediaCodecHelper != null) {
            mMediaCodecHelper.release();
        }
    }
    
    /**
     * 使用示例
     */
    public static void exampleUsage() {
        // 创建示例实例
        MediaCodecExample example = new MediaCodecExample();
        
        // 设置编码回调
        example.setEncodedFrameCallback(new EncodedFrameCallback() {
            @Override
            public void onEncodedFrame(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo) {
                // 处理编码后的H.265数据
                // 例如，保存到文件或通过网络发送
                Log.d(TAG, "收到编码帧: " + bufferInfo.size + " bytes, 时间戳: " + bufferInfo.presentationTimeUs);
                
                // 在实际应用中，这里可能会将数据传递给解码器
                // example.decodeFrame(encodedData, bufferInfo);
            }
        });
        
        // 设置解码回调
        example.setDecodedFrameCallback(new DecodedFrameCallback() {
            @Override
            public void onDecodedFrame(long presentationTimeUs) {
                // 处理解码后的帧
                // 如果使用Surface，帧已经渲染到Surface上
                Log.d(TAG, "帧已解码并渲染: 时间戳: " + presentationTimeUs);
            }
        });
        
        try {
            // 初始化编码器
            if (example.initEncoder()) {
                Log.i(TAG, "编码器初始化成功");
                
                // 在实际应用中，这里会从相机或其他源获取YUV数据
                // 这里仅作为示例，创建一个假的YUV帧
                byte[] fakeYuvFrame = new byte[VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2]; // YUV420格式
                
                // 编码帧
                example.encodeFrame(fakeYuvFrame, System.nanoTime() / 1000);
                
                // 请求关键帧
                example.requestKeyFrame();
            }
            
            // 初始化解码器（这里传null表示不渲染到Surface）
            if (example.initDecoder(null)) {
                Log.i(TAG, "解码器初始化成功");
                
                // 在实际应用中，这里会从编码回调或网络接收H.265数据
                // 解码操作会在编码回调中进行
            }
        } finally {
            // 释放资源
            example.release();
        }
    }
}