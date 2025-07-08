package com.example.mediacodec;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

/**
 * MediaCodec辅助类，用于H.265(HEVC)视频的编码和解码
 */
public class MediaCodecHelper {
    private static final String TAG = "MediaCodecHelper";
    
    // HEVC/H.265的MIME类型
    private static final String MIME_TYPE_HEVC = "video/hevc";
    
    // 默认超时时间（微秒）
    private static final long DEFAULT_TIMEOUT_US = 10000L;
    
    // 编码器和解码器实例
    private MediaCodec mEncoder;
    private MediaCodec mDecoder;
    
    // 编码和解码的格式
    private MediaFormat mEncoderFormat;
    private MediaFormat mDecoderFormat;
    
    // 编码输出队列
    private LinkedBlockingQueue<EncodedData> mEncodedDataQueue;
    
    // 编码器和解码器状态
    private boolean mIsEncoderRunning = false;
    private boolean mIsDecoderRunning = false;
    
    /**
     * 编码后的数据结构
     */
    public static class EncodedData {
        public ByteBuffer buffer;
        public MediaCodec.BufferInfo bufferInfo;
        
        public EncodedData(ByteBuffer buffer, MediaCodec.BufferInfo bufferInfo) {
            this.buffer = buffer;
            this.bufferInfo = bufferInfo;
        }
    }
    
    /**
     * 构造函数
     */
    public MediaCodecHelper() {
        mEncodedDataQueue = new LinkedBlockingQueue<>();
    }
    
    /**
     * 初始化H.265编码器
     * 
     * @param width 视频宽度
     * @param height 视频高度
     * @param bitRate 比特率
     * @param frameRate 帧率
     * @param iFrameInterval I帧间隔（秒）
     * @throws IOException 如果编码器初始化失败
     */
    public void initEncoder(int width, int height, int bitRate, int frameRate, int iFrameInterval) throws IOException {
        // 释放之前的编码器（如果有）
        releaseEncoder();
        
        // 创建H.265编码格式
        mEncoderFormat = MediaFormat.createVideoFormat(MIME_TYPE_HEVC, width, height);
        
        // 设置编码参数
        mEncoderFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible);
        mEncoderFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
        mEncoderFormat.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
        mEncoderFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, iFrameInterval);
        
        // 对于某些设备，可能需要设置复杂度和质量参数
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            mEncoderFormat.setInteger(MediaFormat.KEY_COMPLEXITY, MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR);
        }
        
        // 创建编码器
        mEncoder = MediaCodec.createEncoderByType(MIME_TYPE_HEVC);
        mEncoder.configure(mEncoderFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mEncoder.start();
        mIsEncoderRunning = true;
        
        Log.i(TAG, "H.265编码器初始化完成: " + width + "x" + height + " @" + frameRate + "fps");
    }
    
    /**
     * 初始化H.265解码器
     * 
     * @param width 视频宽度
     * @param height 视频高度
     * @param outputSurface 输出Surface，可以为null（如果只需要解码数据而不需要显示）
     * @throws IOException 如果解码器初始化失败
     */
    public void initDecoder(int width, int height, Surface outputSurface) throws IOException {
        // 释放之前的解码器（如果有）
        releaseDecoder();
        
        // 创建H.265解码格式
        mDecoderFormat = MediaFormat.createVideoFormat(MIME_TYPE_HEVC, width, height);
        
        // 创建解码器
        mDecoder = MediaCodec.createDecoderByType(MIME_TYPE_HEVC);
        mDecoder.configure(mDecoderFormat, outputSurface, null, 0);
        mDecoder.start();
        mIsDecoderRunning = true;
        
        Log.i(TAG, "H.265解码器初始化完成: " + width + "x" + height);
    }
    
    /**
     * 编码一帧YUV数据
     * 
     * @param input YUV数据（YUV420格式）
     * @param presentationTimeUs 时间戳（微秒）
     * @return 是否成功提交编码
     */
    public boolean encodeFrame(byte[] input, long presentationTimeUs) {
        if (!mIsEncoderRunning || mEncoder == null) {
            Log.e(TAG, "编码器未初始化或未运行");
            return false;
        }
        
        try {
            // 获取输入缓冲区索引
            int inputBufferIndex = mEncoder.dequeueInputBuffer(DEFAULT_TIMEOUT_US);
            if (inputBufferIndex >= 0) {
                // 获取输入缓冲区
                ByteBuffer inputBuffer;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    inputBuffer = mEncoder.getInputBuffer(inputBufferIndex);
                } else {
                    inputBuffer = mEncoder.getInputBuffers()[inputBufferIndex];
                }
                
                // 清空缓冲区
                inputBuffer.clear();
                
                // 将YUV数据复制到缓冲区
                inputBuffer.put(input);
                
                // 提交缓冲区进行编码
                mEncoder.queueInputBuffer(inputBufferIndex, 0, input.length, presentationTimeUs, 0);
                
                // 处理编码输出
                drainEncoder(false);
                return true;
            }
        } catch (Exception e) {
            Log.e(TAG, "编码帧时出错: " + e.getMessage());
        }
        
        return false;
    }
    
    /**
     * 解码一帧H.265数据
     * 
     * @param input H.265编码数据
     * @param size 数据大小
     * @param presentationTimeUs 时间戳（微秒）
     * @param flags 标志（例如，关键帧标志）
     * @return 是否成功提交解码
     */
    public boolean decodeFrame(ByteBuffer input, int size, long presentationTimeUs, int flags) {
        if (!mIsDecoderRunning || mDecoder == null) {
            Log.e(TAG, "解码器未初始化或未运行");
            return false;
        }
        
        try {
            // 获取输入缓冲区索引
            int inputBufferIndex = mDecoder.dequeueInputBuffer(DEFAULT_TIMEOUT_US);
            if (inputBufferIndex >= 0) {
                // 获取输入缓冲区
                ByteBuffer inputBuffer;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    inputBuffer = mDecoder.getInputBuffer(inputBufferIndex);
                } else {
                    inputBuffer = mDecoder.getInputBuffers()[inputBufferIndex];
                }
                
                // 清空缓冲区
                inputBuffer.clear();
                
                // 将H.265数据复制到缓冲区
                input.position(0);
                inputBuffer.put(input);
                
                // 提交缓冲区进行解码
                mDecoder.queueInputBuffer(inputBufferIndex, 0, size, presentationTimeUs, flags);
                
                // 处理解码输出
                drainDecoder(false);
                return true;
            }
        } catch (Exception e) {
            Log.e(TAG, "解码帧时出错: " + e.getMessage());
        }
        
        return false;
    }
    
    /**
     * 处理编码器的输出
     * 
     * @param endOfStream 是否是流的结束
     */
    private void drainEncoder(boolean endOfStream) {
        if (!mIsEncoderRunning || mEncoder == null) {
            return;
        }
        
        if (endOfStream) {
            mEncoder.signalEndOfInputStream();
        }
        
        // 创建缓冲区信息对象
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        
        // 循环处理所有可用的输出
        while (true) {
            // 获取输出缓冲区索引
            int outputBufferIndex = mEncoder.dequeueOutputBuffer(bufferInfo, DEFAULT_TIMEOUT_US);
            
            if (outputBufferIndex == MediaCodec.INFO_TRY_AGAIN_LATER) {
                // 没有可用的输出
                if (!endOfStream) {
                    break;
                }
            } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                // 输出格式已更改
                MediaFormat newFormat = mEncoder.getOutputFormat();
                Log.i(TAG, "编码器输出格式已更改: " + newFormat);
            } else if (outputBufferIndex >= 0) {
                // 获取输出缓冲区
                ByteBuffer outputBuffer;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    outputBuffer = mEncoder.getOutputBuffer(outputBufferIndex);
                } else {
                    outputBuffer = mEncoder.getOutputBuffers()[outputBufferIndex];
                }
                
                // 检查是否是有效数据
                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    // 配置数据，调整缓冲区信息
                    bufferInfo.size = 0;
                }
                
                if (bufferInfo.size > 0) {
                    // 调整缓冲区位置
                    outputBuffer.position(bufferInfo.offset);
                    outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
                    
                    // 复制数据到新的缓冲区（因为原缓冲区会被释放）
                    ByteBuffer copyBuffer = ByteBuffer.allocate(bufferInfo.size);
                    copyBuffer.put(outputBuffer);
                    copyBuffer.flip();
                    
                    // 创建编码数据对象并添加到队列
                    MediaCodec.BufferInfo copyInfo = new MediaCodec.BufferInfo();
                    copyInfo.set(0, bufferInfo.size, bufferInfo.presentationTimeUs, bufferInfo.flags);
                    mEncodedDataQueue.offer(new EncodedData(copyBuffer, copyInfo));
                    
                    Log.v(TAG, "编码的帧: " + bufferInfo.size + " bytes, flags: " + bufferInfo.flags + 
                          ", time: " + bufferInfo.presentationTimeUs);
                }
                
                // 释放输出缓冲区
                mEncoder.releaseOutputBuffer(outputBufferIndex, false);
                
                // 检查是否是流的结束
                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    Log.i(TAG, "编码器到达流的结束");
                    break;
                }
            }
        }
    }
    
    /**
     * 处理解码器的输出
     * 
     * @param endOfStream 是否是流的结束
     */
    private void drainDecoder(boolean endOfStream) {
        if (!mIsDecoderRunning || mDecoder == null) {
            return;
        }
        
        // 创建缓冲区信息对象
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        
        // 循环处理所有可用的输出
        while (true) {
            // 获取输出缓冲区索引
            int outputBufferIndex = mDecoder.dequeueOutputBuffer(bufferInfo, DEFAULT_TIMEOUT_US);
            
            if (outputBufferIndex == MediaCodec.INFO_TRY_AGAIN_LATER) {
                // 没有可用的输出
                if (!endOfStream) {
                    break;
                }
            } else if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                // 输出格式已更改
                MediaFormat newFormat = mDecoder.getOutputFormat();
                Log.i(TAG, "解码器输出格式已更改: " + newFormat);
            } else if (outputBufferIndex >= 0) {
                // 检查是否是有效数据
                boolean render = (bufferInfo.size > 0);
                
                // 释放输出缓冲区（如果有Surface，则渲染到Surface）
                mDecoder.releaseOutputBuffer(outputBufferIndex, render);
                
                Log.v(TAG, "解码的帧: 时间戳: " + bufferInfo.presentationTimeUs);
                
                // 检查是否是流的结束
                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    Log.i(TAG, "解码器到达流的结束");
                    break;
                }
            }
        }
    }
    
    /**
     * 获取编码后的数据
     * 
     * @param timeoutUs 超时时间（微秒）
     * @return 编码后的数据，如果没有可用数据则返回null
     */
    public EncodedData getEncodedData(long timeoutUs) {
        try {
            return mEncodedDataQueue.poll(timeoutUs, TimeUnit.MICROSECONDS);
        } catch (InterruptedException e) {
            Log.e(TAG, "获取编码数据时被中断: " + e.getMessage());
            Thread.currentThread().interrupt();
            return null;
        }
    }
    
    /**
     * 请求关键帧（I帧）
     */
    public void requestKeyFrame() {
        if (!mIsEncoderRunning || mEncoder == null) {
            Log.e(TAG, "编码器未初始化或未运行");
            return;
        }
        
        try {
            Bundle params = new Bundle();
            params.putInt(MediaCodec.PARAMETER_KEY_REQUEST_SYNC_FRAME, 0);
            mEncoder.setParameters(params);
            Log.i(TAG, "已请求关键帧");
        } catch (Exception e) {
            Log.e(TAG, "请求关键帧时出错: " + e.getMessage());
        }
    }
    
    /**
     * 释放编码器资源
     */
    public void releaseEncoder() {
        if (mEncoder != null) {
            try {
                if (mIsEncoderRunning) {
                    // 处理剩余的输出
                    drainEncoder(true);
                    mIsEncoderRunning = false;
                }
                mEncoder.stop();
                mEncoder.release();
                Log.i(TAG, "编码器已释放");
            } catch (Exception e) {
                Log.e(TAG, "释放编码器时出错: " + e.getMessage());
            } finally {
                mEncoder = null;
            }
        }
    }
    
    /**
     * 释放解码器资源
     */
    public void releaseDecoder() {
        if (mDecoder != null) {
            try {
                if (mIsDecoderRunning) {
                    // 处理剩余的输出
                    drainDecoder(true);
                    mIsDecoderRunning = false;
                }
                mDecoder.stop();
                mDecoder.release();
                Log.i(TAG, "解码器已释放");
            } catch (Exception e) {
                Log.e(TAG, "释放解码器时出错: " + e.getMessage());
            } finally {
                mDecoder = null;
            }
        }
    }
    
    /**
     * 释放所有资源
     */
    public void release() {
        releaseEncoder();
        releaseDecoder();
        mEncodedDataQueue.clear();
    }
    
    /**
     * 检查设备是否支持H.265编码
     * 
     * @return 如果支持则返回true，否则返回false
     */
    public static boolean isHevcEncoderSupported() {
        try {
            MediaCodecInfo[] codecInfos = new MediaCodecInfo[0];
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                MediaCodecList codecList = new MediaCodecList(MediaCodecList.ALL_CODECS);
                codecInfos = codecList.getCodecInfos();
            }
            
            for (MediaCodecInfo codecInfo : codecInfos) {
                if (!codecInfo.isEncoder()) {
                    continue;
                }
                
                String[] types = codecInfo.getSupportedTypes();
                for (String type : types) {
                    if (type.equalsIgnoreCase(MIME_TYPE_HEVC)) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "检查H.265编码器支持时出错: " + e.getMessage());
        }
        
        return false;
    }
    
    /**
     * 检查设备是否支持H.265解码
     * 
     * @return 如果支持则返回true，否则返回false
     */
    public static boolean isHevcDecoderSupported() {
        try {
            MediaCodecInfo[] codecInfos = new MediaCodecInfo[0];
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                MediaCodecList codecList = new MediaCodecList(MediaCodecList.ALL_CODECS);
                codecInfos = codecList.getCodecInfos();
            }
            
            for (MediaCodecInfo codecInfo : codecInfos) {
                if (codecInfo.isEncoder()) {
                    continue;
                }
                
                String[] types = codecInfo.getSupportedTypes();
                for (String type : types) {
                    if (type.equalsIgnoreCase(MIME_TYPE_HEVC)) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "检查H.265解码器支持时出错: " + e.getMessage());
        }
        
        return false;
    }